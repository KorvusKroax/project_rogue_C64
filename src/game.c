#include "game.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "platform/key.h"
#include "platform/platform.h"
#include "chase.h"
#include "combat.h"
#include "discoveries.h"
#include "equipment.h"
#include "explore.h"
#include "item_names.h"
#include "items.h"
#include "level.h"
#include "map.h"
#include "message.h"
#include "move.h"
#include "pack.h"
#include "view.h"
#include "player.h"
#include "state.h"
#include "status.h"
#include "texts.h"

#ifdef DEVMODE
    #include "dev/dev.h"
#endif

#define KEY_NAME_SIZE  3    // legfeljebb két karakter és a lezáró nulla

#define REPEAT_COUNT_MAX  255

// a számprefixes ismétlés: hányszor jön még a parancs (futás közben nem fogy)
static uint8_t repeat_count;
static uint8_t repeat_code;

// az `a` ezt ismétli; 0: még nem volt parancs
static uint8_t last_code;
// az előtte beolvasott parancs: ha az utolsó parancs iránykérdése ESC-vel elmarad, ez lesz megint az utolsó
static uint8_t previous_last_code;
// az utolsó irányt kérő parancs iránya: az `a` ezt használja, nem kérdez újra
static uint8_t last_direction_index;
// az utolsó parancs bekért tárgya: az `a` ezt használja, nem kérdez újra, ha a tárgy még a csomagban van (J30); NO_ITEM, ha a parancs
// nem kért tárgyat
static uint8_t last_item_index;
// az előtte beolvasott parancs tárgya: ha az utolsó parancs tárgykérdése ESC-vel elmarad, ez lesz megint az utolsó parancs tárgya
static uint8_t previous_last_item_index;

// az `m` után a lépés nem veszi fel a tárgyat; a számprefixes ismétlés lépései is ilyenek, a következő beolvasott parancsig
static bool is_pickup_skipped;

void stop_running_and_repeating(void)
{
    stop_running();
    repeat_count = 0;
}

// a billentyű kiírható alakja: a vezérlőkód ^ + nagybetű (^A, ^[), a törlés ^?, a többi maga a karakter
static void write_key_name(char *name, uint8_t code)
{
    if (code < ' ') {
        name[0] = '^';
        name[1] = code + '@';
        name[2] = 0;
    } else if (code == KEY_DELETE) {
        name[0] = '^';
        name[1] = '?';
        name[2] = 0;
    } else {
        name[0] = code;
        name[1] = 0;
    }
}

static void report_illegal_command(uint8_t code)
{
    repeat_count = 0;

    char name[KEY_NAME_SIZE];
    write_key_name(name, code);
    message_format_without_recall(text_illegal_command, name, NULL, 0);
}

#define DIRECTION_COUNT  8

// az irányok betűi és lépései, azonos sorrendben
static const char direction_letters[DIRECTION_COUNT] = { 'h', 'j', 'k', 'l', 'y', 'u', 'b', 'n' };
static const int8_t direction_steps_x[DIRECTION_COUNT] = { -1, 0, 0, 1, -1, 1, -1, 1 };
static const int8_t direction_steps_y[DIRECTION_COUNT] = { 0, 1, -1, 0, -1, -1, 1, 1 };

static bool is_uppercase_key(uint8_t code)
{
    return code >= 'A' && code <= 'Z';
}

static bool is_ctrl_letter_key(uint8_t code)
{
    return code >= KEY_CTRL('a') && code <= KEY_CTRL('z');
}

// a nagybetű és a CTRL + betű kisbetűvé válik, más kód változatlan
static uint8_t get_lowercase_letter(uint8_t code)
{
    if (is_uppercase_key(code)) return code - 'A' + 'a';
    if (is_ctrl_letter_key(code)) return code - KEY_CTRL('a') + 'a';
    return code;
}

// az irány sorszáma a parancs betűjéből (kis- és nagybetű, CTRL); DIRECTION_COUNT, ha nem irány
static uint8_t get_direction_index(uint8_t code)
{
    uint8_t letter = get_lowercase_letter(code);
    uint8_t index = 0;
    while (index < DIRECTION_COUNT && direction_letters[index] != letter) {
        index++;
    }
    return index;
}

// a parancsok, amelyeknél a számprefix megmarad
static bool is_repeatable_command(uint8_t code)
{
    if (get_direction_index(code) < DIRECTION_COUNT) return true;

    switch (code) {
        case '.':
        case 'a':
        case 's':
        case 'm':
            return true;
        default:
            return false;
    }
}

// lépés (kisbetű; az `m` után felvétel nélkül), futás (nagybetű) vagy óvatos futás (CTRL); igaz, ha időt vitt
static bool execute_direction_command(uint8_t code, uint8_t direction_index)
{
    int8_t step_x = direction_steps_x[direction_index];
    int8_t step_y = direction_steps_y[direction_index];

    // a futás indítása nem visz időt
    if (is_uppercase_key(code)) {
        start_running(step_x, step_y, false);
        return false;
    }
    if (is_ctrl_letter_key(code)) {
        start_running(step_x, step_y, true);
        return false;
    }
    if (is_pickup_skipped) return move_player_without_pickup(step_x, step_y);
    return move_player(step_x, step_y);
}

static bool is_digit_key(uint8_t code)
{
    return code >= '0' && code <= '9';
}

// a nézőablak görgetése és a játékosra állítása nem parancs: az `a` nem ismétli
static bool is_view_window_key(uint8_t code)
{
    return code == KEY_CURSOR_LEFT || code == KEY_CURSOR_RIGHT || code == KEY_HOME;
}

// a következő parancs a billentyűzetről, az esetleges számprefixszel együtt
static uint8_t read_command(void)
{
    set_key_repeat(true);
    uint8_t code = wait_for_key();
    message_clear();
    is_pickup_skipped = false;

    if (is_digit_key(code)) {
        uint8_t count = 0;
        while (is_digit_key(code)) {
            uint8_t digit = code - '0';
            if (count > (REPEAT_COUNT_MAX - digit) / 10) {
                count = REPEAT_COUNT_MAX;
            } else {
                count = count * 10 + digit;
            }
            code = wait_for_key();
        }
        // az első végrehajtás is a számból fogy
        if (count > 0 && is_repeatable_command(code)) {
            repeat_count = count - 1;
            repeat_code = code;
        }
    }
    set_key_repeat(false);

    if (code == 'a' || code == KEY_ESCAPE || is_view_window_key(code)) return code;
    #ifdef DEVMODE
        if (is_function_key(code)) return code;
    #endif
    previous_last_code = last_code;
    last_code = code;
    previous_last_item_index = last_item_index;
    last_item_index = NO_ITEM;
    return code;
}

// az irány bekérése: csak az irány betűje (kis- vagy nagybetű) és az ESC számít, más billentyűre a kérdés marad;
// az irány sorszáma, ESC-re DIRECTION_COUNT
static uint8_t ask_direction(void)
{
    message_prompt(text_which_direction);

    uint8_t direction_index = DIRECTION_COUNT;
    while (direction_index == DIRECTION_COUNT) {
        uint8_t code = wait_for_key();
        if (code == KEY_ESCAPE) break;
        if (!is_ctrl_letter_key(code)) {
            direction_index = get_direction_index(code);
        }
    }
    message_clear();
    return direction_index;
}

// a parancs kérdése (irány, tárgy) ESC-vel elmaradt: az utolsó parancs és a tárgya az előző marad, a számprefixes ismétlés véget ér (J27)
static void cancel_command(void)
{
    last_code = previous_last_code;
    last_item_index = previous_last_item_index;
    repeat_count = 0;
}

// az irányt kérő parancs iránya: az `a` ismétlésnél a tárolt, különben a bekért; DIRECTION_COUNT, ha a kérdés ESC-vel elmaradt
// (cancel_command)
static uint8_t get_command_direction(bool is_repeat)
{
    if (!is_repeat) {
        uint8_t direction_index = ask_direction();
        if (direction_index == DIRECTION_COUNT) {
            cancel_command();
            return DIRECTION_COUNT;
        }
        last_direction_index = direction_index;
    }
    // ide jön: zavarodottan 20% eséllyel véletlen irány
    return last_direction_index;
}

// f (F: kamikaze) + irány: harc a szörnnyel; igaz, ha időt vitt
static bool execute_fight_command(uint8_t code, bool is_repeat)
{
    uint8_t direction_index = get_command_direction(is_repeat);
    if (direction_index == DIRECTION_COUNT) return false;

    return start_fight(direction_steps_x[direction_index], direction_steps_y[direction_index], code == 'F');
}

// m + irány: lépés felvétel nélkül; a számprefixes ismétlés innen ennek az iránynak a lépése, az irányt nem kérdezi újra; igaz, ha időt vitt
static bool execute_move_without_pickup_command(bool is_repeat)
{
    uint8_t direction_index = get_command_direction(is_repeat);
    if (direction_index == DIRECTION_COUNT) return false;

    is_pickup_skipped = true;
    repeat_code = direction_letters[direction_index];
    return execute_direction_command(repeat_code, direction_index);
}

// a tárgy bekérése a csomagból (get_item): a betűje; `*`-ra a csomag filter szerinti listája, hibás betűre „'<x>' is not a valid item”,
// és utána újra a kérdés; a csomag nem üres; NO_ITEM, ha ESC jött (cancel_command), vagy ha a listába nem került semmi
// („you don't have anything appropriate”)
static uint8_t ask_item(const char *purpose, uint8_t filter)
{
    while (true) {
        message_prompt_format(text_which_object, purpose);
        uint8_t code = wait_for_key();
        message_clear();

        if (code == KEY_ESCAPE) {
            cancel_command();
            return NO_ITEM;
        }
        if (code == '*') {
            if (!show_pack_list(filter)) return NO_ITEM;
            continue;
        }

        uint8_t index = find_pack_item_by_letter(code);
        if (index != NO_ITEM) return index;

        char name[KEY_NAME_SIZE];
        write_key_name(name, code);
        message_format(text_not_valid_item, name, NULL, 0);
    }
}

// a tárgyat kérő parancs előtt: üres csomagnál „you aren't carrying anything”, az `a` ismétlésnél, ha az előző parancs tárgya
// már nincs a csomagban, „you ran out”; igaz, ha így a parancsnak nincs tárgya
static bool report_missing_command_item(bool is_repeat)
{
    if (game.first_pack_item == NO_ITEM) {
        message(text_not_carrying_anything);
        return true;
    }
    if (is_repeat && !is_item_in_pack(last_item_index)) {
        message(text_ran_out);
        return true;
    }
    return false;
}

// a tárgyat kérő parancs tárgya (report_missing_command_item után): az `a` ismétlésnél az előző parancsé, különben a bekért (ask_item),
// és ez lesz az `a` tárgya; NO_ITEM, ha a kérdés elmaradt
static uint8_t get_command_item(bool is_repeat, const char *purpose, uint8_t filter)
{
    if (is_repeat) return last_item_index;

    // ESC-re a cancel_command már az előző parancs tárgyát állította vissza
    uint8_t index = ask_item(purpose, filter);
    if (index != NO_ITEM) {
        last_item_index = index;
    }
    return index;
}

// d: egy tárgy lerakása a csomagból a játékos mezőjére; igaz, ha időt vitt
static bool execute_drop_command(bool is_repeat)
{
    // foglalt mezőn a tárgyat sem kérdezi
    uint8_t character = get_map_character(game.player.x, game.player.y);
    if (character != CHAR_FLOOR && character != CHAR_PASSAGE) {
        message(text_something_there_already);
        return false;
    }
    // az üres csomag és az elfogyott tárgy is időbe kerül
    if (report_missing_command_item(is_repeat)) return true;

    uint8_t index = get_command_item(is_repeat, text_item_purpose_drop, PACK_FILTER_ALL);
    // egy kifejezésben: a külön `if (index == NO_ITEM) return false;` után a `return drop_item(index);` eredményét az Oscar64 1.32.273
    // elveszti (README, Oscar64-hibák, 14.)
    return index != NO_ITEM && drop_item(index);
}

// w: a tárgy kézbe vétele (wield_item); igaz, ha időt vitt
static bool execute_wield_command(bool is_repeat)
{
    // ha a kézben tartott tárgy átkozott, a tárgyat sem kérdezi
    if (report_cursed_wielded_item()) return true;
    // az üres csomag és az elfogyott tárgy nem visz időt
    if (report_missing_command_item(is_repeat)) return false;

    uint8_t index = get_command_item(is_repeat, text_item_purpose_wield, PACK_FILTER_WEAPON);
    if (index == NO_ITEM) return false;
    return wield_item(index);
}

// W: páncél felvétele (wear_armor); igaz, ha időt vitt
static bool execute_wear_command(bool is_repeat)
{
    // az üres csomag és az elfogyott tárgy időbe kerül
    if (report_missing_command_item(is_repeat)) return true;

    uint8_t index = get_command_item(is_repeat, text_item_purpose_wear, PACK_FILTER_ARMOR);
    // egy kifejezésben, mint a lerakásnál (README, Oscar64-hibák, 14.)
    return index != NO_ITEM && wear_armor(index);
}

// c: a tárgy vagy a típusa elnevezése (name_item); nem visz időt, az üres csomag és az elfogyott tárgy sem
static void execute_name_command(bool is_repeat)
{
    if (report_missing_command_item(is_repeat)) return;

    uint8_t index = get_command_item(is_repeat, text_item_purpose_call, PACK_FILTER_NAMEABLE);
    if (index != NO_ITEM) {
        name_item(index);
        #ifdef DEVMODE
            report_name_pool_usage();
        #endif
    }
}

// `,`: a játékos mezőjén fekvő tárgy felvétele; üres mezőn csak üzenet
static void pick_up_item_here(void)
{
    // ide jön: lebegve „You can't. You're floating off the ground!”
    uint8_t index = find_floor_item_at(game.player.x, game.player.y);
    if (index == NO_ITEM) {
        message(text_nothing_to_pick_up);
        return;
    }
    pick_up_item(index);
}

// a parancs végrehajtása; igaz, ha időt vitt
static bool execute_command(uint8_t code)
{
    bool is_repeat = false;
    if (code == 'a') {
        if (last_code == 0) {
            message(text_no_command_yet);
            return false;
        }
        code = last_code;
        is_repeat = true;
    }

    uint8_t direction_index = get_direction_index(code);
    if (direction_index < DIRECTION_COUNT) {
        return execute_direction_command(code, direction_index);
    }

    switch (code) {
        case '.':
            return true;
        case 's':
            search_around();
            return true;
        case 'f':
        case 'F':
            return execute_fight_command(code, is_repeat);
        case 'm':
            return execute_move_without_pickup_command(is_repeat);
        case ',':
            // üres mezőn is időbe kerül
            pick_up_item_here();
            return true;
        case 'i':
            show_pack_list(PACK_FILTER_ALL);
            return false;
        case 'd':
            return execute_drop_command(is_repeat);
        case 'w':
            return execute_wield_command(is_repeat);
        case 'W':
            return execute_wear_command(is_repeat);
        case 'T':
            return take_off_armor();
        case 'c':
            execute_name_command(is_repeat);
            return false;
        case 'D':
            show_discoveries();
            return false;
        case '>':
            go_down_stairs();
            return false;
        case '<':
            go_up_stairs();
            return false;
        case KEY_ESCAPE:
        case ' ':
            return false;
        case KEY_CTRL('p'):
            repeat_last_message();
            return false;
        case KEY_CURSOR_LEFT:
            scroll_view_window(-VIEW_WINDOW_SCROLL_STEP);
            return false;
        case KEY_CURSOR_RIGHT:
            scroll_view_window(VIEW_WINDOW_SCROLL_STEP);
            return false;
        case KEY_HOME:
            center_view_window(game.player.x);
            return false;
        default:
            #ifdef DEVMODE
                if (is_function_key(code)) {
                    return dev_execute_command(code);
                }
            #endif
            report_illegal_command(code);
            return false;
    }
}

// alvás, ájulás, fagyás alatt az akció pihenés; az utolsó ilyen akcióban a játékos újra mozoghat
static void rest_while_unable_to_act(void)
{
    game.turns_unable_to_act--;
    if (game.turns_unable_to_act == 0) {
        message(text_you_can_move_again);
    }
}

// a kör eleje, a játékos akciói előtt
static void run_before_phase(void)
{
    // ide jön: a vándorló szörny dobása, a hallucináció, a BEFORE-időzítők
}

// a kör vége, a játékos akciói után
static void run_after_phase(void)
{
    move_monsters();
    // ide jön: az éhség; az állapotváltása (Hungry, Weak, Faint) a futást, az ismétlést és az f-harcot is megszakítja
}

void waste_time(void)
{
    run_before_phase();
    run_after_phase();
}

// egy kör: a BEFORE fázis, a játékos akciói, az AFTER fázis
static void play_turn(void)
{
    run_before_phase();

    // az ingyenes parancs nem fogyaszt akciót
    uint8_t actions = 1;
    while (actions) {
        look_around();
        // futás és ismétlés közben jump_to_run_end mellett a képernyő csak a végén frissül; az f-harc minden köre kirajzolódik
        bool is_repeating = is_running() || repeat_count > 0;
        if (!(is_repeating && (options.flags & OPTION_JUMP_TO_RUN_END))) {
            show_map_changes();
            draw_status();
        }

        // cselekvésképtelenül, f-harc, futás és ismétlés közben nincs billentyűolvasás: az előre leütött billentyűk a pufferben várnak;
        // a cselekvésképtelenség alatt az f-harc, a futás és az ismétlés szünetel, utána folytatódik
        bool took_time;
        if (game.turns_unable_to_act > 0) {
            rest_while_unable_to_act();
            took_time = true;
        } else if (is_fighting()) {
            took_time = take_fight_step();
        } else if (is_running()) {
            took_time = take_run_step();
        } else if (repeat_count > 0) {
            repeat_count--;
            took_time = execute_command(repeat_code);
        } else {
            took_time = execute_command(read_command());
        }
        if (took_time) {
            actions--;
        }
    }

    run_after_phase();
    // ide jön: a gyűrűhatások (searching, teleportation); az üres körben nem futnak le
}

void run_game(void)
{
    init_player();
    init_item_names();
    last_code = 0;
    last_item_index = NO_ITEM;
    game.dungeon_level = 1;
    game.max_level = 1;
    game.levels_without_food = 0;
    clear_item_table();
    create_starting_equipment();
    new_level();

    while (!is_player_dead()) {
        play_turn();
    }

    // ideiglenes, a játék végéig (M10-01): a halál oka az üzenetsávban, a képernyő a halál pillanatában, és billentyűre vár
    show_map_changes();
    draw_status();
    uint8_t death_cause = get_death_cause();
    if (death_cause < DEATH_CAUSE_COUNT) {
        message_format(text_killed_by, "", get_text_from_table(text_death_causes, death_cause), 0);
    } else {
        message_format(text_killed_by, text_monster_article, get_text_from_table(text_monster_names, death_cause - 'A'), 0);
    }
    wait_for_key_and_clear_message();
}
