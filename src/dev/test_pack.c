#include "tests.h"

#include <stdbool.h>

#include "../items.h"
#include "../map.h"
#include "../monsters.h"
#include "../move.h"
#include "../pack.h"
#include "../player.h"
#include "../rooms.h"
#include "../state.h"

#define PACK_TEST_POTION_COUNT  3               // ennyi egyforma bájital egy tárgy a csomagban, és ennyi helyet foglal
#define PACK_TEST_ARROW_COUNT   25              // egy ekkora nyílköteg egy helyet foglal

// a bájitalok és a nyílköteg után marad még hely a tekercseknek
#if MAX_PACK < PACK_TEST_POTION_COUNT + 2
    #error "a csomagteszthez kevés a csomag helye"
#endif

int test_pack_slots(uint16_t i)
{
    clear_item_table();
    game.seed = 1;

    // egyforma bájitalok: egy tárgy, darabonként egy hely; a csatolt rekordok felszabadulnak
    uint8_t potion = NO_ITEM;
    for (uint8_t count = 0; count < PACK_TEST_POTION_COUNT; count++) {
        uint8_t index = create_item(ITEM_TYPE_POTION, 0);
        if (!has_pack_room_for(index)) return false;
        potion = add_item_to_pack(index);
    }
    if (get_item(potion)->count != PACK_TEST_POTION_COUNT) return false;
    if (calculate_pack_slots_used() != PACK_TEST_POTION_COUNT || game.item_count != 1) return false;

    // a nyílköteg egy hely
    uint8_t arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    get_item(arrows)->count = PACK_TEST_ARROW_COUNT;
    if (add_item_to_pack(arrows) != arrows) return false;
    if (calculate_pack_slots_used() != PACK_TEST_POTION_COUNT + 1) return false;

    // a többi hely egyforma tekercsekkel telik meg, utána új tárgy nem fér el
    while (calculate_pack_slots_used() < MAX_PACK) {
        uint8_t index = create_item(ITEM_TYPE_SCROLL, 0);
        if (!has_pack_room_for(index)) return false;
        add_item_to_pack(index);
    }
    uint8_t extra_potion = create_item(ITEM_TYPE_POTION, 0);
    if (has_pack_room_for(extra_potion)) return false;

    // a meglévő nyílköteghez csatolt nyilak még beférnek, és a köteg nő
    uint8_t more_arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    get_item(more_arrows)->group = get_item(arrows)->group;
    uint8_t more_count = get_item(more_arrows)->count;
    if (!has_pack_room_for(more_arrows)) return false;
    if (add_item_to_pack(more_arrows) != arrows) return false;
    if (get_item(arrows)->count != PACK_TEST_ARROW_COUNT + more_count) return false;
    if (calculate_pack_slots_used() != MAX_PACK) return false;

    // egy másik nyílköteg már nem
    uint8_t other_arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    bool other_fits = has_pack_room_for(other_arrows);

    clear_item_table();
    return !other_fits;
}

#define PACK_TEST_LETTER_ITEM_COUNT  4          // ennyi különböző tárgy kerül a csomagba a betűk tesztjében
#define PACK_TEST_REMOVED_POSITION   1          // közülük ez (0-tól számolva) kerül ki

int test_pack_letters(uint16_t i)
{
    clear_item_table();

    // különböző altípusú bájitalok: a betűik sorban az elsőtől
    uint8_t items[PACK_TEST_LETTER_ITEM_COUNT];
    for (uint8_t position = 0; position < PACK_TEST_LETTER_ITEM_COUNT; position++) {
        items[position] = add_item_to_pack(create_item(ITEM_TYPE_POTION, position));
        if (get_item(items[position])->location.pack_letter != FIRST_PACK_LETTER + position) return false;
    }

    // egy tárgy kikerül: a többi betűje nem változik
    remove_item_from_list(&game.first_pack_item, items[PACK_TEST_REMOVED_POSITION]);
    free_item(items[PACK_TEST_REMOVED_POSITION]);
    for (uint8_t position = 0; position < PACK_TEST_LETTER_ITEM_COUNT; position++) {
        if (position == PACK_TEST_REMOVED_POSITION) continue;
        if (get_item(items[position])->location.pack_letter != FIRST_PACK_LETTER + position) return false;
    }

    // az új tárgy a legkisebb szabad betűt kapja: előbb a felszabadultat, utána az eddig nem használt következőt
    uint8_t refill = add_item_to_pack(create_item(ITEM_TYPE_POTION, PACK_TEST_LETTER_ITEM_COUNT));
    if (get_item(refill)->location.pack_letter != FIRST_PACK_LETTER + PACK_TEST_REMOVED_POSITION) return false;
    uint8_t next = add_item_to_pack(create_item(ITEM_TYPE_POTION, PACK_TEST_LETTER_ITEM_COUNT + 1));
    bool is_next_letter_right = get_item(next)->location.pack_letter == FIRST_PACK_LETTER + PACK_TEST_LETTER_ITEM_COUNT;

    clear_item_table();
    return is_next_letter_right;
}

int test_pack_order(uint16_t i)
{
    clear_item_table();

    // két-két altípus bájitalból, tekercsből és páncélból; a páncél nem kötegel, így két egyforma két tárgy
    uint8_t first_potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    uint8_t first_scroll = add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 0));
    uint8_t second_potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 1));
    uint8_t first_armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 0));
    uint8_t second_scroll = add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 1));
    uint8_t second_armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 1));
    uint8_t first_armor_again = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 0));

    // új típus a végére, új altípus az azonos típusúak után, ismétlődő altípus az azonos altípusúak után
    const uint8_t expected[] = { first_potion, second_potion, first_scroll, second_scroll, first_armor, first_armor_again, second_armor };
    if (!is_item_list(game.first_pack_item, expected, sizeof(expected))) return false;

    // a köteghez csatolt bájital nem változtat a sorrenden
    if (add_item_to_pack(create_item(ITEM_TYPE_POTION, 0)) != first_potion) return false;
    bool is_order_kept = is_item_list(game.first_pack_item, expected, sizeof(expected)) && get_item(first_potion)->count == 2;

    clear_item_table();
    return is_order_kept;
}

// a felvétel tesztmezői: a játékos egy folyosó-mezőn, jobbra mellette egy folyosó- és egy padlómező
#define PICKUP_PLAYER_X        10
#define PICKUP_PLAYER_Y        (MAP_TOP_ROW + 2)
#define PICKUP_PASSAGE_NUMBER  1

#define PICKUP_TEST_GOLD_START   100            // ennyi aranya van a játékosnak a felvétel előtt
#define PICKUP_TEST_GOLD_AMOUNT  25             // ennyi arany fekszik a padlón

static void put_pickup_passage_tile(uint8_t x, uint8_t y)
{
    set_map_character(x, y, CHAR_PASSAGE);
    add_map_flags(x, y, TILE_PASSAGE | PICKUP_PASSAGE_NUMBER);
}

// a kezdőértékű játékos a tesztmezőjén, szörnyek nélkül; üres a padló és a csomag
static void put_pickup_scene(void)
{
    clear_map();
    clear_monsters();
    clear_item_table();
    init_player();

    put_pickup_passage_tile(PICKUP_PLAYER_X, PICKUP_PLAYER_Y);
    put_pickup_passage_tile(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    set_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y, CHAR_FLOOR);
    game.player.x = PICKUP_PLAYER_X;
    game.player.y = PICKUP_PLAYER_Y;
    game.player.room = get_room_at(PICKUP_PLAYER_X, PICKUP_PLAYER_Y);
}

static void clear_pickup_scene(void)
{
    clear_item_table();
    clear_map();
}

int test_pickup_by_step(uint16_t i)
{
    put_pickup_scene();
    uint8_t potion = create_item(ITEM_TYPE_POTION, 0);
    put_item_on_floor(potion, PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    uint8_t scroll = create_item(ITEM_TYPE_SCROLL, 0);
    put_item_on_floor(scroll, PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y);

    // futva a tárgyra lépve felveszi, és a futás megáll; a futás lépése a move_player, mint a fogva tartás tesztjében (test_combat.c)
    start_running(1, 0, false);
    bool ok =
        move_player(1, 0) &&
        !is_running() &&
        find_floor_item_at(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == CHAR_PASSAGE &&
        game.first_pack_item == potion &&
        get_item(potion)->location.pack_letter == FIRST_PACK_LETTER;

    // az `m` lépése a tárgyat a padlón hagyja, a jelével együtt
    ok = ok &&
        move_player_without_pickup(1, 0) &&
        game.player.x == PICKUP_PLAYER_X + 2 &&
        find_floor_item_at(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == scroll &&
        get_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == CHAR_SCROLL &&
        get_item(potion)->next == NO_ITEM;

    // a `,` felveszi, és a padlómező jele újra padló
    pick_up_item(scroll);
    const uint8_t expected[] = { potion, scroll };
    ok = ok &&
        game.first_floor_item == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == CHAR_FLOOR &&
        is_item_list(game.first_pack_item, expected, sizeof(expected));

    clear_pickup_scene();
    return ok;
}

int test_pickup_gold(uint16_t i)
{
    put_pickup_scene();
    game.gold = PICKUP_TEST_GOLD_START;
    uint8_t gold = create_item(ITEM_TYPE_GOLD, 0);
    get_item(gold)->properties.gold_amount = PICKUP_TEST_GOLD_AMOUNT;
    put_item_on_floor(gold, PICKUP_PLAYER_X, PICKUP_PLAYER_Y);

    pick_up_item(gold);
    bool ok =
        game.gold == PICKUP_TEST_GOLD_START + PICKUP_TEST_GOLD_AMOUNT &&
        game.item_count == 0 &&
        game.first_floor_item == NO_ITEM &&
        game.first_pack_item == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X, PICKUP_PLAYER_Y) == CHAR_PASSAGE;

    clear_pickup_scene();
    return ok;
}

int test_pickup_full_pack(uint16_t i)
{
    put_pickup_scene();
    while (calculate_pack_slots_used() < MAX_PACK) {
        add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 0));
    }
    uint8_t potion = create_item(ITEM_TYPE_POTION, 0);
    put_item_on_floor(potion, PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    uint8_t item_count = game.item_count;

    bool ok =
        move_player(1, 0) &&
        game.player.x == PICKUP_PLAYER_X + 1 &&
        find_floor_item_at(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == potion &&
        get_map_character(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == CHAR_POTION &&
        calculate_pack_slots_used() == MAX_PACK &&
        game.item_count == item_count;

    clear_pickup_scene();
    return ok;
}
