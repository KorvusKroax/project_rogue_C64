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
