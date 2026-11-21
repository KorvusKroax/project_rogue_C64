#include "tests.h"

#include <stdbool.h>

#include "../items.h"
#include "../pack.h"
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
