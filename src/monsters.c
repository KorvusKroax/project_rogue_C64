#include "monsters.h"

#include "level.h"
#include "map.h"
#include "rng.h"
#include "rooms.h"
#include "view.h"

const struct MonsterType monster_types[MONSTER_TYPE_COUNT] = {
    //      carry  flags                           level armor  exp  segments
    /* A */ {   0, MONSTER_MEAN,                       5,   2,   20, 2, { { 0, 0 }, { 0, 0 } } },
    /* B */ {   0, MONSTER_FLYING,                     1,   3,    1, 1, { { 1, 2 } } },
    /* C */ {  15, 0,                                  4,   4,   17, 3, { { 1, 2 }, { 1, 5 }, { 1, 5 } } },
    /* D */ { 100, MONSTER_MEAN,                      10,  -1, 5000, 3, { { 1, 8 }, { 1, 8 }, { 3, 10 } } },
    /* E */ {   0, MONSTER_MEAN,                       1,   7,    2, 1, { { 1, 2 } } },
    /* F */ {   0, MONSTER_MEAN,                       8,   3,   80, 1, { { DAMAGE_DICE_FLYTRAP, 1 } } },
    /* G */ {  20, MONSTER_MEAN | MONSTER_FLYING,     13,   2, 2000, 2, { { 4, 3 }, { 3, 5 } } },
    /* H */ {   0, MONSTER_MEAN,                       1,   5,    3, 1, { { 1, 8 } } },
    /* I */ {   0, 0,                                  1,   9,    5, 1, { { 0, 0 } } },
    /* J */ {  70, 0,                                 15,   6, 3000, 2, { { 2, 12 }, { 2, 4 } } },
    /* K */ {   0, MONSTER_MEAN | MONSTER_FLYING,      1,   7,    1, 1, { { 1, 4 } } },
    /* L */ {   0, 0,                                  3,   8,   10, 1, { { 1, 1 } } },
    /* M */ {  40, MONSTER_MEAN,                       8,   2,  200, 3, { { 3, 4 }, { 3, 4 }, { 2, 5 } } },
    /* N */ { 100, 0,                                  3,   9,   37, 1, { { 0, 0 } } },
    /* O */ {  15, MONSTER_GREEDY,                     1,   6,    5, 1, { { 1, 8 } } },
    /* P */ {   0, MONSTER_INVISIBLE,                  8,   3,  120, 1, { { 4, 4 } } },
    /* Q */ {   0, MONSTER_MEAN,                       3,   3,   15, 2, { { 1, 5 }, { 1, 5 } } },
    /* R */ {   0, MONSTER_MEAN,                       2,   3,    9, 1, { { 1, 6 } } },
    /* S */ {   0, MONSTER_MEAN,                       1,   5,    2, 1, { { 1, 3 } } },
    /* T */ {  50, MONSTER_MEAN,                       6,   4,  120, 3, { { 1, 8 }, { 1, 8 }, { 2, 6 } } },
    /* U */ {   0, MONSTER_MEAN,                       7,  -2,  190, 3, { { 1, 9 }, { 1, 9 }, { 2, 9 } } },
    /* V */ {  20, MONSTER_MEAN,                       8,   1,  350, 1, { { 1, 10 } } },
    /* W */ {   0, 0,                                  5,   4,   55, 1, { { 1, 6 } } },
    /* X */ {  30, 0,                                  7,   7,  100, 1, { { 4, 4 } } },
    /* Y */ {  30, 0,                                  4,   6,   50, 2, { { 1, 6 }, { 1, 6 } } },
    /* Z */ {   0, MONSTER_MEAN,                       2,   8,    6, 1, { { 1, 8 } } },
};

struct Monster *get_monster(uint8_t index)
{
    return &game.monsters[index - 1];
}

uint8_t allocate_monster(void)
{
    if (game.monster_count == MAX_MONSTERS) return NO_MONSTER;

    game.monster_count++;
    return game.monster_count;
}

void clear_monsters(void)
{
    game.monster_count = 0;
}

uint8_t find_monster_at(uint8_t x, uint8_t y)
{
    if (!(get_map_flags(x, y) & TILE_MONSTER)) return NO_MONSTER;

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        if (monster->x == x && monster->y == y) return index;
    }
    return NO_MONSTER;
}

// a típusok a mélységgel nehezedő sorrendben; a mélység körüli véletlen pozíció választ közülük
#define MONSTER_TYPE_ORDER_LENGTH  26
#define NOT_WANDERING              '0'          // a vándorló sorrendben: ezen a helyen nincs vándorló szörny

static const char initial_monster_order[MONSTER_TYPE_ORDER_LENGTH + 1]   = "KEBSHIROZLCQANYFTWPXUMVGJD";
static const char wandering_monster_order[MONSTER_TYPE_ORDER_LENGTH + 1] = "KEBSH0ROZ0CQA0Y0TWP0UMVGJ0";

#define ORDER_POSITION_OFFSET  6                // a pozíció: mélység + rnd(10) − 6
#define ORDER_POSITION_SPREAD  10
#define ORDER_EDGE_SPREAD      5                // a sorrend széléről kicsúszó pozíció helyett: rnd(5) az elején, rnd(5) + 21 a végén

#if MONSTER_TYPE_ORDER_LENGTH != MONSTER_TYPE_COUNT
    #error "a típussorrendben minden típusnak szerepelnie kell"
#endif

uint8_t choose_monster_type(bool wandering)
{
    const char *order = wandering ? wandering_monster_order : initial_monster_order;
    int16_t position;

    do {
        position = (int16_t)game.dungeon_level + (int16_t)rnd(ORDER_POSITION_SPREAD) - ORDER_POSITION_OFFSET;
        if (position < 0) {
            position = rnd(ORDER_EDGE_SPREAD);
        }
        if (position > MONSTER_TYPE_ORDER_LENGTH - 1) {
            position = rnd(ORDER_EDGE_SPREAD) + MONSTER_TYPE_ORDER_LENGTH - ORDER_EDGE_SPREAD;
        }
    } while (order[position] == NOT_WANDERING);

    return order[position];
}

#define HASTED_MONSTER_DEPTH     30             // ettől a mélységtől minden új szörny gyorsított
#define MONSTER_HIT_DIE_SIDES    8              // a HP: szintenként egy 8 oldalú kocka
#define EXPERIENCE_PER_DEPTH     10             // az amulett szintje alatt mélységenként ennyi XP-vel több
#define ARMOR_MIN                (-128)         // az armor 8 bites előjeles

// a xeroc álcája: a tárgyjelek egyike; az amulett jele csak az amulett szintjétől
static const uint8_t disguise_characters[] = {
    CHAR_POTION, CHAR_SCROLL, CHAR_RING, CHAR_STICK, CHAR_FOOD, CHAR_WEAPON, CHAR_ARMOR, CHAR_STAIRS, CHAR_GOLD, CHAR_AMULET
};

#define DISGUISE_CHARACTER_COUNT  (sizeof(disguise_characters) / sizeof(disguise_characters[0]))

static uint8_t choose_disguise_character(void)
{
    uint8_t count = DISGUISE_CHARACTER_COUNT;
    if (game.dungeon_level < AMULET_LEVEL) {
        count--;
    }
    return disguise_characters[rnd(count)];
}

// az XP-érték HP-tól függő része (expmod)
static uint16_t calculate_experience_bonus(uint8_t level, int16_t max_hit_points)
{
    uint16_t bonus = level == 1 ? max_hit_points / 8 : max_hit_points / 6;

    if (level > 9) {
        bonus *= 20;
    } else if (level > 6) {
        bonus *= 4;
    }
    return bonus;
}

uint8_t create_monster(uint8_t type, uint8_t x, uint8_t y)
{
    uint8_t index = allocate_monster();
    if (index == NO_MONSTER) return NO_MONSTER;

    const struct MonsterType *type_data = &monster_types[type - 'A'];
    struct Monster *monster = get_monster(index);
    uint8_t depth_bonus = game.dungeon_level > AMULET_LEVEL ? game.dungeon_level - AMULET_LEVEL : 0;
    int16_t armor = type_data->armor - depth_bonus;

    monster->x = x;
    monster->y = y;
    monster->room = get_room_at(x, y);
    monster->type = type;
    monster->experience_level = type_data->experience_level + depth_bonus;
    monster->max_hit_points = ROLL(monster->experience_level, MONSTER_HIT_DIE_SIDES);
    monster->hit_points = monster->max_hit_points;
    monster->armor = armor < ARMOR_MIN ? ARMOR_MIN : armor;
    monster->experience = type_data->experience + depth_bonus * EXPERIENCE_PER_DEPTH
                        + calculate_experience_bonus(monster->experience_level, monster->max_hit_points);
    monster->flags = type_data->flags | MONSTER_SLOW_TURN;
    if (game.dungeon_level >= HASTED_MONSTER_DEPTH) {
        monster->flags |= MONSTER_HASTED;
    }
    monster->character_below = get_seen_character(x, y);
    monster->destination_item = DESTINATION_PLAYER;
    monster->first_pack_item = 0;
    monster->disguise = type == 'X' ? choose_disguise_character() : type;

    add_map_flags(x, y, TILE_MONSTER);
    return index;
}

uint8_t generate_monster(bool wandering, uint8_t x, uint8_t y)
{
    if (game.monster_count == MAX_MONSTERS) return NO_MONSTER;

    return create_monster(choose_monster_type(wandering), x, y);
}
