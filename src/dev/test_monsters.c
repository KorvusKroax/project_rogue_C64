#include "tests.h"
#include "checks.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../map.h"
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
        if (allocate_monster() != expected) {
            ok = false;
        }
    }
    if (allocate_monster() != NO_MONSTER || game.monster_count != MAX_MONSTERS) {
        ok = false;
    }

    clear_monsters();
    if (game.monster_count != 0) {
        ok = false;
    }
    if (allocate_monster() != 1) {
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
        get_monster(allocate_monster())->type = 'A' + count;
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

#define GENERATION_TEST_ROLLS  100             // ennyi típusválasztás mélységenként
#define GENERATION_TEST_X      1                // a létrehozott szörnyek mezője; a tesztben a mező tartalma nem számít
#define GENERATION_TEST_Y      MAP_TOP_ROW

static const uint8_t generation_test_depths[] = { 1, 10, 20, 26, 40, 255 };

#define GENERATION_TEST_DEPTH_COUNT  (sizeof(generation_test_depths) / sizeof(generation_test_depths[0]))

// az 1. mélységen a pozíció mindig az elejéről kicsúszik, így csak az első öt típus jöhet
static bool is_first_depth_type(uint8_t type)
{
    return strchr("KEBSH", type) != NULL;
}

// a vándorló sorrendből hiányzó típusok
static bool is_never_wandering(uint8_t type)
{
    return strchr("ILNFXD", type) != NULL;
}

// a típusválasztás: mindig betű, az 1. mélységen csak az első öt, vándorlóként soha a hiányzók
static bool monster_type_choices_valid(void)
{
    for (uint8_t depth_index = 0; depth_index < GENERATION_TEST_DEPTH_COUNT; depth_index++) {
        game.dungeon_level = generation_test_depths[depth_index];

        for (uint8_t roll = 0; roll < GENERATION_TEST_ROLLS; roll++) {
            uint8_t initial = choose_monster_type(false);
            uint8_t wandering = choose_monster_type(true);

            if (initial < 'A' || initial > 'Z' || wandering < 'A' || wandering > 'Z') return false;
            if (is_never_wandering(wandering)) return false;
            if (game.dungeon_level == 1 && (!is_first_depth_type(initial) || !is_first_depth_type(wandering))) return false;
        }
    }
    return true;
}

// az XP a képletből: a típus értéke, mélységenként 10, és a HP-tól függő rész
static uint16_t calculate_expected_experience(uint16_t type_experience, uint8_t depth_bonus, uint8_t level, int16_t max_hit_points)
{
    uint16_t bonus = level == 1 ? max_hit_points / 8 : max_hit_points / 6;
    if (level > 9) {
        bonus *= 20;
    } else if (level > 6) {
        bonus *= 4;
    }
    return type_experience + depth_bonus * 10 + bonus;
}

// egy dragon az adott mélységen: a szint, az armor, a HP tartománya, az XP és a gyorsítás
static bool dragon_valid(uint8_t depth, uint8_t depth_bonus, int8_t armor, bool hasted)
{
    game.dungeon_level = depth;
    clear_monsters();
    struct Monster *dragon = get_monster(create_monster('D', GENERATION_TEST_X, GENERATION_TEST_Y));
    uint8_t level = 10 + depth_bonus;

    return
        dragon->type == 'D' &&
        dragon->disguise == 'D' &&
        dragon->x == GENERATION_TEST_X &&
        dragon->y == GENERATION_TEST_Y &&
        dragon->experience_level == level &&
        dragon->armor == armor &&
        dragon->max_hit_points >= level &&
        dragon->max_hit_points <= level * 8 &&
        dragon->hit_points == dragon->max_hit_points &&
        dragon->experience == calculate_expected_experience(5000, depth_bonus, level, dragon->max_hit_points) &&
        (dragon->flags & MONSTER_MEAN) &&
        (dragon->flags & MONSTER_SLOW_TURN) &&
        ((dragon->flags & MONSTER_HASTED) != 0) == hasted &&
        dragon->destination_item == DESTINATION_PLAYER &&
        (get_map_flags(GENERATION_TEST_X, GENERATION_TEST_Y) & TILE_MONSTER);
}

// a xeroc álcája tárgyjel; az amulett jele csak az amulett szintjétől
static bool xeroc_disguises_valid(uint8_t depth)
{
    game.dungeon_level = depth;

    for (uint8_t roll = 0; roll < GENERATION_TEST_ROLLS; roll++) {
        clear_monsters();
        uint8_t disguise = get_monster(create_monster('X', GENERATION_TEST_X, GENERATION_TEST_Y))->disguise;

        if (strchr("!?=/:)]%*,", disguise) == NULL) return false;
        if (disguise == ',' && depth < 26) return false;
    }
    return true;
}

// betelt táblánál a generálás és a létrehozás NO_MONSTER-t ad: nem dob, a mezőre nem kerül TILE_MONSTER, a szörnyek száma marad
static bool full_table_generation_valid(void)
{
    clear_monsters();
    remove_map_flags(GENERATION_TEST_X, GENERATION_TEST_Y, TILE_MONSTER);
    while (allocate_monster() != NO_MONSTER) {}

    uint32_t seed = game.seed;
    return
        generate_monster(false, GENERATION_TEST_X, GENERATION_TEST_Y) == NO_MONSTER &&
        create_monster('X', GENERATION_TEST_X, GENERATION_TEST_Y) == NO_MONSTER &&
        game.seed == seed &&
        game.monster_count == MAX_MONSTERS &&
        !(get_map_flags(GENERATION_TEST_X, GENERATION_TEST_Y) & TILE_MONSTER);
}

int test_monster_generation(uint16_t i)
{
    uint8_t saved_dungeon_level = game.dungeon_level;

    bool ok =
        monster_type_choices_valid() &&
        dragon_valid(26, 0, -1, false) &&
        dragon_valid(29, 3, -4, false) &&
        dragon_valid(30, 4, -5, true) &&
        dragon_valid(255, 229, -128, true) &&
        xeroc_disguises_valid(25) &&
        xeroc_disguises_valid(26) &&
        full_table_generation_valid();

    clear_monsters();
    remove_map_flags(GENERATION_TEST_X, GENERATION_TEST_Y, TILE_MONSTER);
    game.dungeon_level = saved_dungeon_level;
    return ok;
}

#define TILES_TEST_Y  (MAP_TOP_ROW + 1)

// az ellenőrzés jó táblára igazat ad, kóbor bitre és egy mezőn álló két szörnyre hamisat
int test_monster_tiles(uint16_t i)
{
    bool ok = true;

    clear_map();
    clear_monsters();
    if (!are_monster_tiles_valid()) {
        ok = false;
    }

    create_monster('B', 1, TILES_TEST_Y);
    create_monster('K', 2, TILES_TEST_Y);
    if (!are_monster_tiles_valid()) {
        ok = false;
    }

    add_map_flags(3, TILES_TEST_Y, TILE_MONSTER);
    if (are_monster_tiles_valid()) {
        ok = false;
    }
    remove_map_flags(3, TILES_TEST_Y, TILE_MONSTER);

    create_monster('E', 2, TILES_TEST_Y);
    if (are_monster_tiles_valid()) {
        ok = false;
    }

    clear_monsters();
    clear_map();
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
