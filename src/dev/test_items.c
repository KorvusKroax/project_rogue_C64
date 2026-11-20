#include "tests.h"

#include <stdbool.h>

#include "../items.h"
#include "../state.h"

// a 0..CHANCE_TOTAL-1 dobások közül minden elemet pontosan annyi választ, amennyi az esélye; így az esélyek összege is CHANCE_TOTAL
static bool is_chance_table_exact(const struct ChanceTable *table)
{
    if (table->count == 0) return false;

    for (uint8_t index = 0; index < table->count; index++) {
        uint8_t hits = 0;
        for (uint8_t roll = 0; roll < CHANCE_TOTAL; roll++) {
            if (choose_by_chance(table, roll) == index) {
                hits++;
            }
        }
        if (hits != table->chances[index]) return false;
    }
    return true;
}

int test_item_chances(uint16_t i)
{
    if (!is_chance_table_exact(&item_type_table)) return false;

    for (uint8_t type = 0; type < RANDOM_ITEM_TYPE_COUNT; type++) {
        if (type == ITEM_TYPE_FOOD) continue;
        if (!is_chance_table_exact(&item_subtype_tables[type])) return false;
    }
    return true;
}

#define FOOD_TEST_TRIALS  100                   // ennyi tárgy számlálónként, rögzített maggal

// a típus és az altípus érvényes
static bool is_item_type_and_subtype_valid(uint8_t type, uint8_t which)
{
    if (type == ITEM_TYPE_FOOD) return which == FOOD_RATION || which == FOOD_FRUIT;
    return type < RANDOM_ITEM_TYPE_COUNT && which < item_subtype_tables[type].count;
}

int test_levels_without_food(uint16_t i)
{
    game.seed = 1;

    // a határ fölött mindig étel, és utána a számláló 0
    for (uint8_t trial = 0; trial < FOOD_TEST_TRIALS; trial++) {
        uint8_t type;
        uint8_t which;
        game.levels_without_food = LEVELS_WITHOUT_FOOD_MAX + 1;
        choose_item_type_and_subtype(&type, &which);
        if (type != ITEM_TYPE_FOOD || !is_item_type_and_subtype_valid(type, which)) return false;
        if (game.levels_without_food != 0) return false;
    }

    // a határon a táblák szerint: nem csak étel jön; étel után a számláló 0, más tárgy után nem változik
    bool other_seen = false;
    for (uint8_t trial = 0; trial < FOOD_TEST_TRIALS; trial++) {
        uint8_t type;
        uint8_t which;
        game.levels_without_food = LEVELS_WITHOUT_FOOD_MAX;
        choose_item_type_and_subtype(&type, &which);
        if (!is_item_type_and_subtype_valid(type, which)) return false;

        uint8_t expected_count = type == ITEM_TYPE_FOOD ? 0 : LEVELS_WITHOUT_FOOD_MAX;
        if (game.levels_without_food != expected_count) return false;
        if (type != ITEM_TYPE_FOOD) {
            other_seen = true;
        }
    }
    return other_seen;
}
