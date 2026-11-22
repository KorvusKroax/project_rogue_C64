#include "dev.h"

#include <stddef.h>
#include <string.h>

#include "../platform/charset.h"
#include "../platform/key.h"
#include "../platform/screen.h"
#include "../explore.h"
#include "../input.h"
#include "../item_names.h"
#include "../items.h"
#include "../level.h"
#include "../map.h"
#include "../message.h"
#include "../monsters.h"
#include "../num.h"
#include "../state.h"
#include "../view.h"
#include "checks.h"

#define DEPTH_DIGITS_MAX  3         // 1..DUNGEON_LEVEL_MAX (254)

#define FULL_MAP_BORDER_COLOR  2   // piros: a teljes térkép be van kapcsolva

static bool full_map_shown = false;
static uint8_t full_map_row[VIEW_WINDOW_WIDTH];
static uint8_t normal_border_color;

// a titkos folyosó-mező a játékban üresnek, a titkos ajtó falnak látszik
static uint8_t choose_full_map_character(uint8_t character, uint8_t flags)
{
    if (flags & TILE_REAL) return character;
    return (flags & TILE_PASSAGE ? CHAR_PASSAGE : CHAR_DOOR) + INVERSE_OFFSET;
}

bool is_full_map_shown(void)
{
    return full_map_shown;
}

// csak a nézőablak oszlopai, közvetlenül a térkép soraiból: görgetéskor mind a 22 sor újraszámolódik
const uint8_t *build_full_map_row(uint8_t y)
{
    const uint8_t *characters = game.map_characters[y - MAP_TOP_ROW] + game.view_window_x;
    const uint8_t *flags = game.map_flags[y - MAP_TOP_ROW] + game.view_window_x;

    for (uint8_t column = 0; column < VIEW_WINDOW_WIDTH; column++) {
        full_map_row[column] = choose_full_map_character(characters[column], flags[column]);
    }

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        uint8_t monster_column = monster->x - game.view_window_x;
        if (monster->y == y && monster_column < VIEW_WINDOW_WIDTH) {
            full_map_row[monster_column] = monster->type;
            if (monster->flags & MONSTER_RUNNING) {
                full_map_row[monster_column] += INVERSE_OFFSET;
            }
        }
    }

    uint8_t player_column = game.player.x - game.view_window_x;
    if (y == game.player.y && player_column < VIEW_WINDOW_WIDTH) {
        full_map_row[player_column] = CHAR_PLAYER;
    }
    return full_map_row;
}

static void toggle_full_map(void)
{
    full_map_shown = !full_map_shown;

    if (full_map_shown) {
        normal_border_color = get_border_color();
        set_border_color(FULL_MAP_BORDER_COLOR);
    } else {
        set_border_color(normal_border_color);
    }
    redraw_map();
}

static const char *get_option_state_text(uint8_t option)
{
    return (options.flags & option) ? "on" : "off";
}

// a futás két beállítása együtt, mint egy két bites számláló (a follow_passages az alsó bit), körbe:
// egyik sem → follow_passages → jump_to_run_end → mindkettő → egyik sem; az új állapot az üzenetsorba
static void switch_run_options(void)
{
    if (!(options.flags & OPTION_FOLLOW_PASSAGES)) {
        options.flags |= OPTION_FOLLOW_PASSAGES;
    } else {
        options.flags &= ~OPTION_FOLLOW_PASSAGES;
        options.flags ^= OPTION_JUMP_TO_RUN_END;
    }
    message_format_without_recall("follow passages: \1, jump to run end: \2",
        get_option_state_text(OPTION_FOLLOW_PASSAGES), get_option_state_text(OPTION_JUMP_TO_RUN_END), 0);
}

// új szint a bekért mélységen; üres mezőn RETURN-re ugyanazon a mélységen; ESC-re vagy hibás számra nem történik semmi
static void create_level_at_asked_depth(void)
{
    char answer[DEPTH_DIGITS_MAX + 1];
    uint32_t depth;

    bool answered = ask_text("depth (1-254): ", answer, DEPTH_DIGITS_MAX);
    message_clear();
    if (!answered) return;

    if (answer[0] == 0) {
        new_level();
    } else if (convert_text_to_number(answer, DUNGEON_LEVEL_MAX, &depth) && depth >= 1) {
        game.dungeon_level = depth;
        new_level();
    }
}

static const char *const inventory_style_names[INVENTORY_STYLE_COUNT] = { "clear", "overwrite", "slow" };

// az inven opció a következő módra (clear → overwrite → slow → clear), és az új mód az üzenetsorba
static void switch_inventory_style(void)
{
    options.inventory_style = (options.inventory_style + 1) % INVENTORY_STYLE_COUNT;
    message_format_without_recall("inventory style: \1", inventory_style_names[options.inventory_style], NULL, 0);
}

// a játékos körüli első szabad mező (soronként, balról jobbra): járható, látszik a játékos mezőjéről, és nem áll rajta szörny;
// tárgynak ezen felül üres padló vagy folyosó-mező
static bool find_free_neighbor(bool is_for_item, uint8_t *x, uint8_t *y)
{
    uint8_t player_x = game.player.x;
    uint8_t player_y = game.player.y;

    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t tile_x = player_x + offset_x;
            uint8_t tile_y = player_y + offset_y;
            if (!is_on_map(tile_x, tile_y)) continue;
            if (tile_x == player_x && tile_y == player_y) continue;

            uint8_t character = get_map_character(tile_x, tile_y);
            if (!is_walkable_character(character)) continue;
            if (is_for_item && character != CHAR_FLOOR && character != CHAR_PASSAGE) continue;
            if (!is_neighbor_visible(player_x, player_y, tile_x, tile_y)) continue;
            if (get_map_flags(tile_x, tile_y) & TILE_MONSTER) continue;

            *x = tile_x;
            *y = tile_y;
            return true;
        }
    }
    return false;
}

// a szörny betűjének bekérése, és létrehozása a játékos mellett; ESC-re vagy nem betűre nem történik semmi
// utána a szörnytábla foglaltsága, előtte a hiba, ha nem jött létre; a kirajzolást a következő look_around végzi
static void create_monster_by_letter(void)
{
    char answer[2];
    bool answered = ask_text("monster (A-Z): ", answer, 1);
    message_clear();
    if (!answered) return;

    uint8_t type = answer[0];
    if (type >= 'a' && type <= 'z') {
        type -= 'a' - 'A';
    }
    if (type < 'A' || type > 'Z') return;

    const char *failure = "";
    uint8_t x;
    uint8_t y;

    if (game.monster_count == MAX_MONSTERS) {
        failure = "the monster table is full, ";
    } else if (!find_free_neighbor(false, &x, &y)) {
        failure = "no free tile, ";
    } else {
        create_monster(type, x, y);
        if (!are_monster_tiles_valid()) {
            failure = "monster tiles invalid, ";
        }
    }

    char max_monsters_text[FORMAT_BUFFER_SIZE];
    format_int(max_monsters_text, MAX_MONSTERS, 0);
    message_format_without_recall("\1monsters: \3/\2", failure, max_monsters_text, game.monster_count);
}

static uint8_t item_count_peak = 0;

void update_item_count_peak(void)
{
    if (game.item_count > item_count_peak) {
        item_count_peak = game.item_count;
    }
}

// az altípusok száma; 0, ha a típusnak nincs altípusa (arany, amulett)
static uint8_t get_subtype_count(uint8_t type)
{
    if (type == ITEM_TYPE_FOOD) return FOOD_SUBTYPE_COUNT;
    if (type >= RANDOM_ITEM_TYPE_COUNT) return 0;
    return item_subtype_tables[type].count;
}

#define ITEM_WHICH_DIGITS_MAX  2                // az altípus száma legfeljebb kétjegyű

// az altípus számának bekérése, a tartománnyal (pl. "which (0-13): "); hamis ESC-re vagy hibás számra
static bool ask_item_which(uint8_t subtype_count, uint8_t *which)
{
    static const char prompt_start[] = "which (0-";
    char prompt[sizeof(prompt_start) + FORMAT_BUFFER_SIZE + sizeof("): ")];
    char answer[ITEM_WHICH_DIGITS_MAX + 1];
    uint32_t number;

    uint8_t length = sizeof(prompt_start) - 1;
    memcpy(prompt, prompt_start, length);
    length += format_int(prompt + length, subtype_count - 1, 0);
    strcpy(prompt + length, "): ");

    bool answered = ask_text(prompt, answer, ITEM_WHICH_DIGITS_MAX);
    message_clear();
    if (!answered || !convert_text_to_number(answer, subtype_count - 1, &number)) return false;

    *which = number;
    return true;
}

// a tárgy jelének bekérése, altípusos típusnál a számáé is, és létrehozása a játékos melletti szabad padló- vagy folyosó-mezőre,
// a véletlen generálással azonos kezdőértékekkel (az aranynak a mélység szerinti véletlen mennyisége van); ESC-re, ismeretlen jelre
// vagy hibás számra nem történik semmi; utána a tárgy neve, a tárgytábla foglaltsága és csúcsértéke, vagy ha a tárgy nem jött létre,
// a hiba és a foglaltság; a kirajzolást a következő look_around végzi
static void create_item_by_character(void)
{
    char answer[2];
    bool answered = ask_text("item (! ? : ) ] = / * ,): ", answer, 1);
    message_clear();
    if (!answered) return;

    uint8_t type = 0;
    while (type < ITEM_TYPE_COUNT && get_item_character(type) != answer[0]) {
        type++;
    }
    if (type == ITEM_TYPE_COUNT) return;

    uint8_t which = 0;
    uint8_t subtype_count = get_subtype_count(type);
    if (subtype_count > 0 && !ask_item_which(subtype_count, &which)) return;

    const char *failure = NULL;
    uint8_t index = NO_ITEM;
    uint8_t x;
    uint8_t y;

    if (game.item_count == MAX_ITEMS) {
        failure = "the item table is full, ";
    } else if (!find_free_neighbor(true, &x, &y)) {
        failure = "no free tile, ";
    } else {
        index = create_item(type, which);
        roll_curse_or_bonus(index);
        if (type == ITEM_TYPE_GOLD) {
            set_random_gold_amount(index);
        }
        put_item_on_floor(index, x, y);
    }

    // "96, peak 52": a message_format egyetlen számot ír, a többi szövegként megy
    char limits_text[FORMAT_BUFFER_SIZE + sizeof(", peak ") + FORMAT_BUFFER_SIZE];
    uint8_t length = format_int(limits_text, MAX_ITEMS, 0);
    strcpy(limits_text + length, ", peak ");
    length += sizeof(", peak ") - 1;
    format_int(limits_text + length, item_count_peak, 0);

    if (failure != NULL) {
        message_format_without_recall("\1items: \3/\2", failure, limits_text, game.item_count);
    } else {
        message_format_without_recall("created \1; items: \3/\2", format_item_name(get_item(index)), limits_text, game.item_count);
    }
}

// a szint összes szörnye felébred és üldöz; utána a számuk az üzenetsorba
static void start_all_monsters_running(void)
{
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        start_monster_running(get_monster(index));
    }
    message_format_without_recall("running monsters: \3", NULL, NULL, game.monster_count);
}

// ha van nem lassított szörny, mindegyik lassított lesz, különben egyik sem; az új állapot az üzenetsorba
static void toggle_all_monsters_slow(void)
{
    bool has_unslowed_monster = false;
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        if (!(get_monster(index)->flags & MONSTER_SLOW)) {
            has_unslowed_monster = true;
        }
    }

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        struct Monster *monster = get_monster(index);
        if (has_unslowed_monster) {
            monster->flags |= MONSTER_SLOW;
        } else {
            monster->flags &= ~MONSTER_SLOW;
        }
    }
    message_format_without_recall("slow monsters: \1", has_unslowed_monster ? "on" : "off", NULL, 0);
}

bool dev_execute_command(uint8_t code)
{
    switch (code) {
        case KEY_F1:
            create_level_at_asked_depth();
            break;
        case KEY_F2:
            create_monster_by_letter();
            break;
        case KEY_F3:
            switch_inventory_style();
            break;
        case KEY_F4:
            start_all_monsters_running();
            break;
        case KEY_F5:
            toggle_full_map();
            break;
        case KEY_F6:
            toggle_all_monsters_slow();
            break;
        case KEY_F7:
            create_item_by_character();
            break;
        case KEY_F8:
            switch_run_options();
            break;
    }

    return false;
}
