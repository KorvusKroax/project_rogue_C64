#include "level.h"

#include "corridors.h"
#include "explore.h"
#include "items.h"
#include "map.h"
#include "message.h"
#include "monsters.h"
#include "pack.h"
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

// a szinten lehet arany és tárgy: amulett nélkül mindig, amulettel csak a legmélyebb elért szinten (max_level)
static bool is_loot_allowed(void)
{
    return !is_carrying_amulet() || game.dungeon_level >= game.max_level;
}

// szobánként: arany 1/2 eséllyel, ha a szinten lehet (is_loot_allowed), és van neki üres padló és helye a tárgytáblában;
// utána egy alvó kezdő szörny, aranyas szobában nagyobb eséllyel
static void place_gold_and_monsters(void)
{
    bool is_gold_allowed = is_loot_allowed();

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        if (game.rooms[room_index].flags & ROOM_PHANTOM) continue;

        uint8_t x;
        uint8_t y;
        bool has_gold = false;

        // az arany dobása akkor is lefut, ha a szinten nem lehet arany
        if (rnd(GOLD_CHANCE_DIVISOR) == 0 && is_gold_allowed && find_empty_floor(room_index, &x, &y)) {
            uint8_t gold_index = create_item(ITEM_TYPE_GOLD, 0);
            if (gold_index != NO_ITEM) {
                set_random_gold_amount(gold_index);
                // ide jön: a szoba aranya (ROOMGOLD), ezt őrzi a kapzsi szörny
                put_item_on_floor(gold_index, x, y);
                has_gold = true;
            }
        }

        uint8_t monster_chance = has_gold ? MONSTER_CHANCE_WITH_GOLD : MONSTER_CHANCE_WITHOUT_GOLD;
        if (rnd(100) < monster_chance && find_monster_tile(room_index, &x, &y)) {
            generate_monster(false, x, y);
            // ide jön: a szörny kezdő tárgya (give_item)
        }
    }
}

#define TREASURE_ROOM_CHANCE_DIVISOR  20        // a szinten 1 / TREASURE_ROOM_CHANCE_DIVISOR eséllyel van kincses szoba (default: 20)
#define TREASURE_MIN                  2         // a kincses szoba tervezett tárgyszáma legalább ennyi (default: 2)
#define TREASURE_MAX                  10        // és legfeljebb ennyi (default: 10)
#define TREASURE_EXTRA_MONSTERS       2         // a kincses szobában legalább ennyivel több szörny van, mint a tervezett tárgyszám (default: 2)

#if TREASURE_MIN > TREASURE_MAX
    #error "a TREASURE_MIN nem lehet nagyobb a TREASURE_MAX-nál"
#endif

#if TREASURE_MAX + TREASURE_EXTRA_MONSTERS > 255
    #error "a kincses szoba szörnyeinek száma 8 bites"
#endif

#define ITEM_ATTEMPTS  9                        // ennyi próba a szint tárgyaira (default: 9)
#define ITEM_CHANCE    36                       // százalék: egy próba ennyi eséllyel hoz tárgyat (default: 36)

// egy véletlen tárgy a szoba (ANY_ROOM: bármely nem fantom szoba) üres padlójára; kimarad, ha nincs hely, vagy betelt a tárgytábla
static void place_random_item(uint8_t room_index)
{
    uint8_t x;
    uint8_t y;

    if (!find_empty_floor(room_index, &x, &y)) return;

    uint8_t index = create_random_item();
    if (index != NO_ITEM) {
        put_item_on_floor(index, x, y);
    }
}

// kincses szoba egy véletlen nem fantom szobában, labirintusban is: TREASURE_MIN..TREASURE_MAX tárgy a belső terület erejéig,
// és legalább TREASURE_EXTRA_MONSTERS-szel több aljas szörny a következő mélység szerint; ami nem fér el, kimarad
static void place_treasure_room(void)
{
    uint8_t room_index = choose_random_room();
    const struct Room *room = &game.rooms[room_index];
    uint16_t inner_area = (uint16_t)(room->width - 2) * (room->height - 2);

    // a TREASURE_MIN feletti véletlen többlet felső határa; kis szobában 0, a terv ekkor is TREASURE_MIN
    uint8_t extra = 0;
    if (inner_area > TREASURE_MAX) {
        extra = TREASURE_MAX - TREASURE_MIN;
    } else if (inner_area > TREASURE_MIN) {
        extra = inner_area - TREASURE_MIN;
    }

    uint8_t item_count = rnd(extra) + TREASURE_MIN;
    for (uint8_t i = 0; i < item_count; i++) {
        place_random_item(room_index);
    }

    uint8_t monster_count = rnd(extra) + TREASURE_MIN;
    if (monster_count < item_count + TREASURE_EXTRA_MONSTERS) {
        monster_count = item_count + TREASURE_EXTRA_MONSTERS;
    }
    if (monster_count > inner_area) {
        monster_count = inner_area;
    }

    // legfeljebb DUNGEON_LEVEL_MAX + 1, ez 8 biten elfér
    game.dungeon_level++;
    for (uint8_t i = 0; i < monster_count; i++) {
        uint8_t x;
        uint8_t y;

        // ha nincs több alkalmas mező, vagy betelt a szörnytábla, a többi szörny sem jön létre
        if (!find_monster_tile(room_index, &x, &y)) break;
        uint8_t index = generate_monster(false, x, y);
        if (index == NO_MONSTER) break;

        get_monster(index)->flags |= MONSTER_MEAN;
        // ide jön: a szörny kezdő tárgya (give_item), szintén a következő mélység szerint
    }
    game.dungeon_level--;
}

// az amulett egy véletlen nem fantom szoba üres padlójára; kimarad, ha nincs hely, vagy betelt a tárgytábla
static void place_amulet(void)
{
    uint8_t x;
    uint8_t y;

    if (!find_empty_floor(ANY_ROOM, &x, &y)) return;

    uint8_t index = create_item(ITEM_TYPE_AMULET, 0);
    if (index != NO_ITEM) {
        put_item_on_floor(index, x, y);
    }
}

// a szint tárgyai, ha a szinten lehet (is_loot_allowed; ha nem, dobás sincs): 1 / TREASURE_ROOM_CHANCE_DIVISOR eséllyel kincses szoba,
// aztán ITEM_ATTEMPTS próba bármely nem fantom szobában, végül az AMULET_LEVEL mélységtől az amulett, ha nincs a játékosnál
static void place_items(void)
{
    if (!is_loot_allowed()) return;

    if (rnd(TREASURE_ROOM_CHANCE_DIVISOR) == 0) {
        place_treasure_room();
    }

    for (uint8_t attempt = 0; attempt < ITEM_ATTEMPTS; attempt++) {
        if (rnd(100) < ITEM_CHANCE) {
            place_random_item(ANY_ROOM);
        }
    }

    if (game.dungeon_level >= AMULET_LEVEL && !is_carrying_amulet()) {
        place_amulet();
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
    if (game.dungeon_level > game.max_level) {
        game.max_level = game.dungeon_level;
    }
    clear_map();
    clear_monsters();
    // ide jön: a szörnyek csomagjainak törlése, a tárgytáblában a helyük felszabadul
    free_floor_items();

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
    place_items();
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
    if (!is_player_on_stairs() || game.dungeon_level == DUNGEON_LEVEL_MAX) {
        message(text_no_way_down);
        return;
    }

    game.dungeon_level++;
    new_level();
}

void go_up_stairs(void)
{
    // ide jön: lebegve nem lehet
    if (!is_player_on_stairs()) {
        message(text_no_way_up);
        return;
    }

    if (!is_carrying_amulet()) {
        message(text_way_magically_blocked);
        return;
    }
    // ide jön: az 1. szintről győzelem (M10-01); addig ott nem történik semmi
    if (game.dungeon_level == 1) return;

    game.dungeon_level--;
    new_level();
    message(text_wrenching_sensation);
}
