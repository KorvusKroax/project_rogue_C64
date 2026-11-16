#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../monsters.h"
#include "../texts.h"

static bool segment_count_is(uint8_t letter, uint8_t count)
{
    return monster_types[letter - 'A'].damage_segment_count == count;
}

static bool segment_is(uint8_t letter, uint8_t segment, uint8_t dice, uint8_t sides)
{
    const struct DamageSegment *damage = &monster_types[letter - 'A'].damage[segment];
    return damage->dice == dice && damage->sides == sides;
}

static bool name_is(uint8_t letter, const char *name)
{
    return strcmp(text_monster_names[letter - 'A'], name) == 0;
}

static bool segment_counts_valid(void)
{
    for (uint8_t i = 0; i < MONSTER_TYPE_COUNT; i++) {
        uint8_t count = monster_types[i].damage_segment_count;
        if (count < 1 || count > DAMAGE_SEGMENTS_MAX) return false;
    }
    return true;
}

// a tábla betelése után a foglalás NO_MONSTER-t ad, törlés után újra az első helyet
int test_monster_table(uint16_t i)
{
    bool ok = true;

    clear_monsters();
    for (uint8_t expected = 1; expected <= MAX_MONSTERS; expected++) {
        if (allocate_monster('A') != expected) {
            ok = false;
        }
    }
    if (allocate_monster('A') != NO_MONSTER || game.monster_count != MAX_MONSTERS) {
        ok = false;
    }

    clear_monsters();
    if (game.monster_count != 0) {
        ok = false;
    }
    if (allocate_monster('B') != 1 || get_monster(1)->type != 'B') {
        ok = false;
    }

    clear_monsters();
    return ok;
}

#define ORDER_TEST_MONSTERS  3

// a cselekvés sorrendjében bejárva a szörnyek a létrehozás fordított sorrendjében jönnek
int test_monster_order(uint16_t i)
{
    bool ok = true;

    clear_monsters();
    for (uint8_t count = 0; count < ORDER_TEST_MONSTERS; count++) {
        allocate_monster('A' + count);
    }

    uint8_t expected_type = 'A' + ORDER_TEST_MONSTERS - 1;
    uint8_t visited = 0;
    for (uint8_t index = game.monster_count; index != NO_MONSTER; index--) {
        if (get_monster(index)->type != expected_type) {
            ok = false;
        }
        expected_type--;
        visited++;
    }
    if (visited != ORDER_TEST_MONSTERS) {
        ok = false;
    }

    clear_monsters();
    return ok;
}

int test_monster_types(uint16_t i)
{
    return
        segment_counts_valid() &&
        segment_count_is('D', 3) &&
        segment_is('D', 0, 1, 8) &&
        segment_is('D', 1, 1, 8) &&
        segment_is('D', 2, 3, 10) &&
        segment_count_is('A', 2) &&
        segment_is('A', 0, 0, 0) &&
        segment_is('A', 1, 0, 0) &&
        segment_is('F', 0, DAMAGE_DICE_FLYTRAP, 1) &&
        name_is('D', "dragon") &&
        name_is('Z', "zombie");
}
