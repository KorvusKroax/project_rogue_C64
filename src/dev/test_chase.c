#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../chase.h"
#include "../explore.h"
#include "../items.h"
#include "../map.h"
#include "../monsters.h"
#include "../pack.h"
#include "../player.h"
#include "../rooms.h"
#include "../state.h"
#include "../view.h"

// a tesztpálya a CHASE_BASE_X, CHASE_BASE_Y mezőtől jobbra lefelé épül
#define CHASE_BASE_X  30
#define CHASE_BASE_Y  (MAP_TOP_ROW + 2)

// a tesztpálya két szobát, két folyosórendszert és három kijáratot használ
#if ROOM_COUNT < 2 || MAX_PASSAGES < 2 || MAX_EXITS < 3
    #error "a tesztpályához legalább 2 szoba, 2 folyosórendszer és 3 kijárat kell"
#endif

// normál szoba falakkal, padlóval; a szobatáblába is bekerül
static void put_chase_room(uint8_t room_index, uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height)
{
    struct Room *room = &game.rooms[room_index];
    room->x = CHASE_BASE_X + offset_x;
    room->y = CHASE_BASE_Y + offset_y;
    room->width = width;
    room->height = height;
    room->flags = 0;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint8_t character = CHAR_FLOOR;
            if (y == 0 || y == height - 1) {
                character = CHAR_WALL_HORIZONTAL;
            } else if (x == 0 || x == width - 1) {
                character = CHAR_WALL_VERTICAL;
            }
            set_map_character(room->x + x, room->y + y, character);
        }
    }
}

// ajtó a szoba falán, a folyosórendszer sorszámával, és a kijárata az ajtótáblában
static void put_chase_door(uint8_t offset_x, uint8_t offset_y, uint8_t room_index, uint8_t passage_number)
{
    uint8_t x = CHASE_BASE_X + offset_x;
    uint8_t y = CHASE_BASE_Y + offset_y;
    set_map_character(x, y, CHAR_DOOR);
    add_map_flags(x, y, passage_number);

    struct Exit *exit = &game.exits[game.exit_count];
    exit->x = x;
    exit->y = y;
    exit->room = room_index;
    exit->passage_number = passage_number;
    game.exit_count++;
}

static void put_chase_passage(uint8_t offset_x, uint8_t offset_y, uint8_t passage_number)
{
    uint8_t x = CHASE_BASE_X + offset_x;
    uint8_t y = CHASE_BASE_Y + offset_y;
    set_map_character(x, y, CHAR_PASSAGE);
    add_map_flags(x, y, TILE_PASSAGE | passage_number);
}

// a tesztpálya (eltolásokkal):
//  - az 1. szoba: (0, 0)..(6, 4), az alsó falán ajtó a (3, 4) mezőn;
//  - a 0. szoba: (10, 6)..(21, 12), ajtó a bal falán a (10, 9), a jobb falán a (21, 9) mezőn;
//  - az 1. folyosórendszer az 1. szoba ajtajától lefelé a (3, 9) mezőig, onnan jobbra a 0. szoba bal ajtajáig;
//  - a 2. folyosórendszer a 0. szoba jobb ajtajától jobbra, a (24, 9) mezőig;
// a többi szoba fantom, hogy a get_room_at csak ezt a kettőt találja; a padló üres
static void put_chase_level(void)
{
    clear_map();
    clear_monsters();
    clear_item_table();
    clear_seen_map();
    game.dungeon_level = 1;
    game.exit_count = 0;

    for (uint8_t room_index = 0; room_index < ROOM_TABLE_SIZE; room_index++) {
        game.rooms[room_index].flags = room_index < ROOM_COUNT ? ROOM_PHANTOM : ROOM_PHANTOM | ROOM_DARK;
    }
    put_chase_room(0, 10, 6, 12, 7);
    put_chase_room(1, 0, 0, 7, 5);

    put_chase_door(10, 9, 0, 1);
    put_chase_door(21, 9, 0, 2);
    put_chase_door(3, 4, 1, 1);

    for (uint8_t y = 5; y <= 9; y++) {
        put_chase_passage(3, y, 1);
    }
    for (uint8_t x = 4; x <= 9; x++) {
        put_chase_passage(x, 9, 1);
    }
    for (uint8_t x = 22; x <= 24; x++) {
        put_chase_passage(x, 9, 2);
    }
}

#define CHASE_TEST_HIT_POINTS  1000             // a mellé érő szörnyek támadásai alatt se haljon meg: halála után a szörnyek nem lépnek

static void put_chase_player(uint8_t offset_x, uint8_t offset_y)
{
    init_player();
    game.player.hit_points = CHASE_TEST_HIT_POINTS;
    game.player.x = CHASE_BASE_X + offset_x;
    game.player.y = CHASE_BASE_Y + offset_y;
    game.player.room = get_room_at(game.player.x, game.player.y);
}

// ébren lévő szörny, a célpontja a choose_monster_destination szerint; a rekord címe a táblából jön, nem egy hívás eredményéből
// (README, Oscar64-hibák, 8.)
static struct Monster *put_chase_monster(uint8_t type, uint8_t offset_x, uint8_t offset_y)
{
    create_monster(type, CHASE_BASE_X + offset_x, CHASE_BASE_Y + offset_y);
    struct Monster *monster = &game.monsters[game.monster_count - 1];
    start_monster_running(monster);
    return monster;
}

static void run_monster_turns(uint8_t turn_count)
{
    for (uint8_t turn = 0; turn < turn_count; turn++) {
        move_monsters();
    }
}

static bool is_monster_at(const struct Monster *monster, uint8_t offset_x, uint8_t offset_y)
{
    return monster->x == CHASE_BASE_X + offset_x && monster->y == CHASE_BASE_Y + offset_y;
}

// étel a padlón, a tesztpálya megadott mezőjén
static uint8_t put_chase_item(uint8_t offset_x, uint8_t offset_y)
{
    uint8_t index = create_item(ITEM_TYPE_FOOD, FOOD_RATION);
    put_item_on_floor(index, CHASE_BASE_X + offset_x, CHASE_BASE_Y + offset_y);
    return index;
}

static uint8_t get_chase_seen_character(uint8_t offset_x, uint8_t offset_y)
{
    return get_seen_character(CHASE_BASE_X + offset_x, CHASE_BASE_Y + offset_y);
}

// ugyanabban a szobában a zombi egyenesen a játékos felé lép, mellé érve megáll, és nem lép rá
static bool is_same_room_chase_valid(void)
{
    put_chase_level();
    put_chase_player(11, 9);
    struct Monster *zombie = put_chase_monster('Z', 20, 9);

    run_monster_turns(1);
    if (!is_monster_at(zombie, 19, 9)) return false;
    run_monster_turns(7);
    if (!is_monster_at(zombie, 12, 9)) return false;
    run_monster_turns(3);
    return is_monster_at(zombie, 12, 9);
}

// a 0. szobában a (20, 9) mezőről két kör alatt megtett lépések, ha a játékos a (11, 9) mezőn áll
static uint8_t count_steps_in_two_turns(uint8_t type, uint16_t added_flags)
{
    put_chase_level();
    put_chase_player(11, 9);
    struct Monster *monster = put_chase_monster(type, 20, 9);
    monster->flags |= added_flags;

    run_monster_turns(2);
    return CHASE_BASE_X + 20 - monster->x;
}

// körönként egy lépés; a lassított minden második körben, a gyorsított és a távoli repülő (kestrel) kétszer; a Venus flytrap soha
static bool are_monster_speeds_valid(void)
{
    return count_steps_in_two_turns('Z', 0) == 2
        && count_steps_in_two_turns('Z', MONSTER_SLOW) == 1
        && count_steps_in_two_turns('Z', MONSTER_HASTED) == 4
        && count_steps_in_two_turns('K', 0) == 4
        && count_steps_in_two_turns('F', 0) == 0;
}

// a játékos az 1. szobában: a zombi nem egyenesen felé indul (a bal falnál elakadna), hanem a 0. szoba hozzá legközelebbi
// kijáratára, a bal ajtóra; az ajtón a folyosórendszer kijáratai közül az 1. szoba ajtaját választja, és a folyosón át
// a játékos mellé ér
static bool is_exit_chase_valid(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    struct Monster *zombie = put_chase_monster('Z', 19, 7);

    run_monster_turns(9);
    if (!is_monster_at(zombie, 10, 9) || zombie->room != 0) return false;
    run_monster_turns(1);
    if (!is_monster_at(zombie, 9, 9) || zombie->room != PASSAGE_ROOM_INDEX(1)) return false;
    run_monster_turns(11);
    if (!is_monster_at(zombie, 3, 4) || zombie->room != 1) return false;
    run_monster_turns(1);
    if (!is_monster_at(zombie, 3, 3)) return false;
    run_monster_turns(2);
    return is_monster_at(zombie, 3, 3);
}

#define BAT_TEST_SEED       2468
#define BAT_TEST_TRIALS     100
// a (20, 9) mezőről a (11, 9) mezőn álló játékos felé az egyenes lépés a (19, 9): a lépések fele ez, a véletlen fél 1/9-e is,
// így 5/9 eséllyel; a véletlen fél 5/9-e a többi járható szomszéd (két-két padló fölötte és alatta, és a jobb ajtó), így 5/18;
// a többi esetben fal vagy a saját mező, ott helyben marad
#define BAT_TEST_STRAIGHT_EXPECTED  (BAT_TEST_TRIALS * 5 / 9)
#define BAT_TEST_SIDEWAYS_EXPECTED  (BAT_TEST_TRIALS * 5 / 18)
#define BAT_TEST_TOLERANCE          (BAT_TEST_TRIALS * 15 / 100)

#if BAT_TEST_TRIALS > 255
    #error "a BAT_TEST_TRIALS számlálója 8 bites"
#endif

// a szörny vissza a megadott mezőre, a TILE_MONSTER bittel együtt
static void put_monster_back(struct Monster *monster, uint8_t offset_x, uint8_t offset_y)
{
    remove_map_flags(monster->x, monster->y, TILE_MONSTER);
    monster->x = CHASE_BASE_X + offset_x;
    monster->y = CHASE_BASE_Y + offset_y;
    monster->room = get_room_at(monster->x, monster->y);
    add_map_flags(monster->x, monster->y, TILE_MONSTER);
}

// a denevér kaotikusan mozog: a lépései fele véletlen irányba megy; rögzített maggal, mindig ugyanarról a mezőről, repülés nélkül
static bool is_bat_movement_chaotic(void)
{
    put_chase_level();
    put_chase_player(11, 9);
    struct Monster *bat = put_chase_monster('B', 20, 9);
    bat->flags &= ~MONSTER_FLYING;
    game.seed = BAT_TEST_SEED;

    uint8_t straight_count = 0;
    uint8_t sideways_count = 0;
    for (uint8_t trial = 0; trial < BAT_TEST_TRIALS; trial++) {
        run_monster_turns(1);
        if (is_monster_at(bat, 19, 9)) {
            straight_count++;
        } else if (!is_monster_at(bat, 20, 9)) {
            sideways_count++;
        }
        put_monster_back(bat, 20, 9);
    }

    bool is_straight_count_valid = straight_count >= BAT_TEST_STRAIGHT_EXPECTED - BAT_TEST_TOLERANCE
                                && straight_count <= BAT_TEST_STRAIGHT_EXPECTED + BAT_TEST_TOLERANCE;
    bool is_sideways_count_valid = sideways_count >= BAT_TEST_SIDEWAYS_EXPECTED - BAT_TEST_TOLERANCE
                                && sideways_count <= BAT_TEST_SIDEWAYS_EXPECTED + BAT_TEST_TOLERANCE;
    return is_straight_count_valid && is_sideways_count_valid;
}

// a tesztpálya után: a szobatábla vissza, a többi tábla és a térkép üres
static void clear_chase_level(const struct Room *saved_rooms)
{
    memcpy(game.rooms, saved_rooms, sizeof(game.rooms));
    game.exit_count = 0;
    clear_monsters();
    clear_item_table();
    clear_map();
    clear_seen_map();
}

int test_monster_chase(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_same_room_chase_valid() &&
        are_monster_speeds_valid() &&
        is_exit_chase_valid() &&
        is_bat_movement_chaotic();

    clear_chase_level(saved_rooms);
    return ok;
}

// a játékos az 1. szobában, a 0. szobában egy tárgy: a nimfa (carry 100) a tárgyat választja, a zombi (carry 0) a játékost;
// egy második nimfa sem a foglalt tárgyat; ha a játékos a 0. szobába lép, a nimfa célpontja is a játékos
static bool is_room_item_destination_valid(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    uint8_t food = put_chase_item(15, 8);

    struct Monster *nymph = put_chase_monster('N', 19, 7);
    struct Monster *zombie = put_chase_monster('Z', 19, 8);
    struct Monster *other_nymph = put_chase_monster('N', 19, 11);
    if (nymph->destination_item != food) return false;
    if (zombie->destination_item != DESTINATION_PLAYER) return false;
    if (other_nymph->destination_item != DESTINATION_PLAYER) return false;

    put_chase_player(11, 9);
    return choose_monster_destination(nymph) == DESTINATION_PLAYER;
}

// a nimfa a folyosón, a tárgy is: ha a játékos az 1. szoba ajtaján áll, a szomszédos nimfát látja, ezért ő a célpont;
// a szobában beljebb állva már nem látja, és a nimfa a tárgyat választja; a más szobában fekvő tárgyat nem
static bool is_visible_monster_destination_valid(void)
{
    put_chase_level();
    put_chase_player(3, 4);
    uint8_t food = put_chase_item(3, 8);
    struct Monster *nymph = put_chase_monster('N', 3, 5);
    if (nymph->destination_item != DESTINATION_PLAYER) return false;

    put_chase_player(3, 2);
    if (choose_monster_destination(nymph) != food) return false;

    // a 0. szobában álló nimfa szobájában nincs tárgy; a játékos a 2. folyosórendszerben, távol
    put_chase_player(24, 9);
    struct Monster *room_nymph = put_chase_monster('N', 19, 7);
    return room_nymph->destination_item == DESTINATION_PLAYER;
}

// J3: a 0. szobában két tárgy; az elsőre (a padló listájában) a folyosón álló nimfa tart; a szoba nimfája a második tárgyat
// választja, mert a többi tárgyat a saját szobájához hasonlítja, nem a foglaló szörnyéhez
static bool is_occupied_item_skipped(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    uint8_t second = put_chase_item(15, 10);
    uint8_t first = put_chase_item(15, 8);

    struct Monster *passage_nymph = put_chase_monster('N', 6, 9);
    passage_nymph->destination_item = first;
    struct Monster *room_nymph = put_chase_monster('N', 19, 7);
    return room_nymph->destination_item == second;
}

int test_monster_destination(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_room_item_destination_valid() &&
        is_visible_monster_destination_valid() &&
        is_occupied_item_skipped();

    clear_chase_level(saved_rooms);
    return ok;
}

// a világos 0. szobában a játékos látja a tárgyat: a nimfa odaér, felveszi, a mező padló, a nimfa alatt is; a szobában nincs
// több tárgy, és a játékos is itt van, így a célpont a játékos; továbblépve a tárgy jele nem tér vissza (J32)
static bool is_visible_pickup_valid(void)
{
    put_chase_level();
    put_chase_player(11, 9);
    uint8_t food = put_chase_item(17, 7);
    struct Monster *nymph = put_chase_monster('N', 20, 7);
    nymph->destination_item = food;
    enter_room(game.player.x, game.player.y);
    if (get_chase_seen_character(17, 7) != CHAR_FOOD) return false;

    run_monster_turns(3);
    if (!is_monster_at(nymph, 17, 7)) return false;
    if (nymph->first_pack_item != food || get_item(food)->next != NO_ITEM) return false;
    if (game.first_floor_item != NO_ITEM) return false;
    if (get_map_character(CHASE_BASE_X + 17, CHASE_BASE_Y + 7) != CHAR_FLOOR) return false;
    if (nymph->character_below != CHAR_FLOOR) return false;
    if (nymph->destination_item != DESTINATION_PLAYER) return false;

    run_monster_turns(1);
    if (is_monster_at(nymph, 17, 7)) return false;
    return get_chase_seen_character(17, 7) == CHAR_FLOOR;
}

// a játékos az 1. szobában nem látja a 0. szobát: a nimfa felveszi a tárgyat, de a játékos emlékében a tárgy a helyén marad
static bool is_unseen_pickup_remembered(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    uint8_t food = put_chase_item(17, 7);
    draw_tile(CHASE_BASE_X + 17, CHASE_BASE_Y + 7, CHAR_FOOD);
    struct Monster *nymph = put_chase_monster('N', 20, 7);
    if (nymph->destination_item != food) return false;

    run_monster_turns(3);
    if (nymph->first_pack_item != food) return false;
    if (nymph->destination_item != DESTINATION_PLAYER) return false;

    run_monster_turns(1);
    if (is_monster_at(nymph, 17, 7)) return false;
    return get_chase_seen_character(17, 7) == CHAR_FOOD;
}

// a tárgy felé a legjobb lépés a játékos mezője: a nimfa megtámadja, nem lép rá, és a tárgy a padlón marad, továbbra is célpontként (J33)
static bool is_player_in_the_way_attacked(void)
{
    put_chase_level();
    put_chase_player(19, 9);
    uint8_t food = put_chase_item(14, 9);
    struct Monster *nymph = put_chase_monster('N', 20, 9);
    nymph->destination_item = food;

    run_monster_turns(1);
    return is_monster_at(nymph, 20, 9) && game.first_floor_item == food && nymph->destination_item == food;
}

// két nimfa tart ugyanarra a tárgyra (a választás ezt nem adja, ezért kézzel): amelyik felveszi, a másik célpontja a játékos lesz;
// a harmadik nimfa egy másik tárgyra tart, az övé marad
static bool is_destination_cleared_by_monster_pickup(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    uint8_t other_food = put_chase_item(12, 11);
    uint8_t food = put_chase_item(17, 7);

    struct Monster *picker = put_chase_monster('N', 18, 7);
    struct Monster *follower = put_chase_monster('N', 20, 9);
    follower->destination_item = food;
    struct Monster *bystander = put_chase_monster('N', 20, 11);
    bystander->destination_item = other_food;
    if (picker->destination_item != food) return false;

    run_monster_turns(1);
    return picker->first_pack_item == food && follower->destination_item == DESTINATION_PLAYER
        && bystander->destination_item == other_food;
}

// a játékos veszi fel a tárgyat, és az aranyat is (az utóbbi rekordja felszabadul): a rájuk tartó szörnyek célpontja a játékos lesz
static bool is_destination_cleared_by_player_pickup(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    uint8_t gold = create_item(ITEM_TYPE_GOLD, 0);
    get_item(gold)->properties.gold_amount = 1;
    put_item_on_floor(gold, CHASE_BASE_X + 15, CHASE_BASE_Y + 10);
    uint8_t food = put_chase_item(17, 7);

    struct Monster *food_nymph = put_chase_monster('N', 19, 7);
    struct Monster *gold_nymph = put_chase_monster('N', 19, 11);
    gold_nymph->destination_item = gold;
    if (food_nymph->destination_item != food) return false;

    pick_up_item(food);
    pick_up_item(gold);
    return food_nymph->destination_item == DESTINATION_PLAYER && gold_nymph->destination_item == DESTINATION_PLAYER;
}

int test_destination_taken(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_destination_cleared_by_monster_pickup() &&
        is_destination_cleared_by_player_pickup();

    clear_chase_level(saved_rooms);
    return ok;
}

int test_monster_pickup(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_visible_pickup_valid() &&
        is_unseen_pickup_remembered() &&
        is_player_in_the_way_attacked();

    clear_chase_level(saved_rooms);
    return ok;
}
