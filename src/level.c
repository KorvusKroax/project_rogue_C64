#include "level.h"

#include "corridors.h"
#include "explore.h"
#include "map.h"
#include "message.h"
#include "monsters.h"
#include "rng.h"
#include "rooms.h"
#include "state.h"
#include "texts.h"
#include "view.h"

// mire keresünk mezőt
#define FIND_FOR_ITEM     0                     // üres padló, labirintusban üres járat
#define FIND_FOR_PLAYER   1                     // járható jel, szörny nélkül
#define FIND_FOR_MONSTER  2                     // járható jel, szörny nélkül, és nem a játékos helye

// a legnagyobb szoba a szektor utolsó oszlopa és sora nélkül, a falai nélkül ennyi mezője van (default: 23 × 5 = 115)
#define MAX_ROOM_INNER_TILES  ((SECTOR_WIDTH - 3) * (SECTOR_HEIGHT - 3))

// nagy szektornál (pl. 1×1-es rácsnál) a jelöltek száma 255-nél több is lehet
#if MAX_ROOM_INNER_TILES > 255
    typedef uint16_t tile_count_t;
#else
    typedef uint8_t tile_count_t;
#endif

static bool is_tile_suitable(const struct Room *room, uint8_t x, uint8_t y, uint8_t purpose)
{
    uint8_t character = get_map_character(x, y);

    if (purpose == FIND_FOR_ITEM) {
        return character == (room->flags & ROOM_MAZE ? CHAR_PASSAGE : CHAR_FLOOR);
    }

    if (get_map_flags(x, y) & TILE_MONSTER) return false;
    if (purpose == FIND_FOR_MONSTER && x == game.player.x && y == game.player.y) return false;
    return is_walkable_character(character);
}

// a szoba falain belüli alkalmas mezők száma
static tile_count_t count_suitable_tiles(const struct Room *room, uint8_t purpose)
{
    tile_count_t count = 0;
    uint8_t right_x = room->x + room->width - 1;
    uint8_t bottom_y = room->y + room->height - 1;

    for (uint8_t y = room->y + 1; y < bottom_y; y++) {
        for (uint8_t x = room->x + 1; x < right_x; x++) {
            if (is_tile_suitable(room, x, y, purpose)) {
                count++;
            }
        }
    }
    return count;
}

// a chosen. alkalmas mező (0-tól, soronként balról jobbra); chosen kisebb, mint az alkalmas mezők száma
static void get_suitable_tile(const struct Room *room, uint8_t purpose, tile_count_t chosen, uint8_t *x, uint8_t *y)
{
    uint8_t right_x = room->x + room->width - 1;
    uint8_t bottom_y = room->y + room->height - 1;

    for (uint8_t tile_y = room->y + 1; tile_y < bottom_y; tile_y++) {
        for (uint8_t tile_x = room->x + 1; tile_x < right_x; tile_x++) {
            if (!is_tile_suitable(room, tile_x, tile_y, purpose)) continue;

            if (chosen == 0) {
                *x = tile_x;
                *y = tile_y;
                return;
            }
            chosen--;
        }
    }
}

// egy alkalmas mező a szobában, egyetlen dobással; egy jelöltnél nincs mit dobni
static bool find_tile_in_room(uint8_t room_index, uint8_t purpose, uint8_t *x, uint8_t *y)
{
    const struct Room *room = &game.rooms[room_index];
    tile_count_t count = count_suitable_tiles(room, purpose);

    if (count == 0) return false;

    tile_count_t chosen = 0;
    if (count > 1) {
        chosen = rnd(count);
    }
    get_suitable_tile(room, purpose, chosen, x, y);
    return true;
}

// ANY_ROOM-nál a még nem próbált nem fantom szobák közül egy dobással választ, amíg talál, vagy el nem fogynak
static bool find_tile(uint8_t room_index, uint8_t purpose, uint8_t *x, uint8_t *y)
{
    if (room_index != ANY_ROOM) {
        return find_tile_in_room(room_index, purpose, x, y);
    }

    bool tried[ROOM_COUNT];
    uint8_t untried_count = 0;
    for (uint8_t i = 0; i < ROOM_COUNT; i++) {
        tried[i] = (game.rooms[i].flags & ROOM_PHANTOM) != 0;
        if (!tried[i]) {
            untried_count++;
        }
    }

    while (untried_count > 0) {
        uint8_t chosen = 0;
        if (untried_count > 1) {
            chosen = rnd(untried_count);
        }

        // a chosen. még nem próbált szoba
        uint8_t chosen_room = 0;
        while (tried[chosen_room] || chosen > 0) {
            if (!tried[chosen_room]) {
                chosen--;
            }
            chosen_room++;
        }

        if (find_tile_in_room(chosen_room, purpose, x, y)) return true;

        tried[chosen_room] = true;
        untried_count--;
    }
    return false;
}

bool find_empty_floor(uint8_t room_index, uint8_t *x, uint8_t *y)
{
    return find_tile(room_index, FIND_FOR_ITEM, x, y);
}

bool find_walkable_tile(uint8_t room_index, uint8_t *x, uint8_t *y)
{
    return find_tile(room_index, FIND_FOR_PLAYER, x, y);
}

// járható mező szörny nélkül, nem a játékos helye: szörnynek
static bool find_monster_tile(uint8_t room_index, uint8_t *x, uint8_t *y)
{
    return find_tile(room_index, FIND_FOR_MONSTER, x, y);
}

#define GOLD_CHANCE_DIVISOR           2         // a szobában 1 / GOLD_CHANCE_DIVISOR eséllyel van arany
#define MONSTER_CHANCE_WITH_GOLD      80        // százalék: az aranyat gyakran szörny őrzi
#define MONSTER_CHANCE_WITHOUT_GOLD   25

// szobánként: arany 1/2 eséllyel, ha van neki üres padló; utána egy alvó kezdő szörny, aranyas szobában nagyobb eséllyel
static void place_gold_and_monsters(void)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        if (game.rooms[room_index].flags & ROOM_PHANTOM) continue;

        uint8_t x;
        uint8_t y;
        bool has_gold = false;

        // ide jön: amulettel csak a legnagyobb elért mélységen (max_level) lehet arany; a dobás ettől függetlenül lefut
        if (rnd(GOLD_CHANCE_DIVISOR) == 0 && find_empty_floor(room_index, &x, &y)) {
            // ide jön: az arany mennyisége és lerakása (x, y)
            has_gold = true;
        }

        uint8_t monster_chance = has_gold ? MONSTER_CHANCE_WITH_GOLD : MONSTER_CHANCE_WITHOUT_GOLD;
        if (rnd(100) < monster_chance && find_monster_tile(room_index, &x, &y)) {
            generate_monster(false, x, y);
            // ide jön: a szörny kezdő tárgya (give_item)
        }
    }
}

// a lépcső egy véletlen nem fantom szoba üres padlójára (labirintusban járatra)
static void place_stairs(void)
{
    uint8_t x;
    uint8_t y;

    // a még üres szobákban mindig van hely, ezért a visszatérési érték nem kell;
    // ha a szobák felépítése változik, erre figyelni kell: hely nélkül az x és az y érvénytelen, és egy véletlen mező íródna felül
    find_empty_floor(ANY_ROOM, &x, &y);

    set_map_character(x, y, CHAR_STAIRS);
}

// a játékos helye egy véletlen nem fantom szoba járható mezőjén
static void choose_player_position(void)
{
    // a szobákban még csak a lépcső van, ezért mindig van hely, és a visszatérési érték nem kell;
    // ha a szobák felépítése változik, erre figyelni kell: hely nélkül a játékos helye érvénytelen maradna
    find_walkable_tile(ANY_ROOM, &game.player.x, &game.player.y);
}

void generate_level(void)
{
    // ide jön: ha a mélység nagyobb az eddigi legnagyobbnál (max_level), az lesz az új legnagyobb
    clear_map();
    clear_monsters();
    // ide jön: a szörnyek csomagjainak és a padlón lévő tárgyaknak a törlése, a tárgytáblában a helyük felszabadul

    generate_rooms();
    connect_rooms();
    assign_passage_numbers();

    // a lépcső és a játékos még az üres szobákba kerül, minden járat látható: mindkettő mindig talál helyet
    place_stairs();
    choose_player_position();
    hide_passages(game.player.x, game.player.y);

    place_gold_and_monsters();
    // egy újabb szint a legutóbbi étel óta: a szobák tárgyai előtt nő, így közöttük egy étel nullázza
    if (game.levels_without_food < UINT8_MAX) {
        game.levels_without_food++;
    }
    // ide jön: tárgyak (kincses szoba, 9 próba, amulett)
    // ide jön: csapdák
}

void new_level(void)
{
    // a játékost nem tartja fogva semmi
    game.flytrap_grip = 0;
    clear_seen_map();
    generate_level();
    // ide jön: a lépcsőt a játékos még nem látta (seenstairs)

    // előbb a nézőablak: az újrarajzolás az üres látott térképből letörli a régi szintet, és az enter_room már a jó oszlopokba rajzol
    center_view_window(game.player.x);
    enter_room(game.player.x, game.player.y);
    forget_previous_position();

    // ide jön: szörnyészlelés alatt a szörnyek azonnal kirajzolódnak (turn_see)
    // ide jön: hallucináció alatt a jelek azonnal hallucináltak (visuals)
}

static bool is_player_on_stairs(void)
{
    return get_map_character(game.player.x, game.player.y) == CHAR_STAIRS;
}

void go_down_stairs(void)
{
    // ide jön: lebegve nem lehet
    if (!is_player_on_stairs()) {
        message(text_no_way_down);
        return;
    }

    if (game.dungeon_level < DUNGEON_LEVEL_MAX) {
        game.dungeon_level++;
    }
    new_level();
}

void go_up_stairs(void)
{
    // ide jön: lebegve nem lehet
    if (!is_player_on_stairs()) {
        message(text_no_way_up);
        return;
    }

    // ide jön: amulettel egy szinttel feljebb, az 1. szintről győzelem
    message(text_way_magically_blocked);
}
