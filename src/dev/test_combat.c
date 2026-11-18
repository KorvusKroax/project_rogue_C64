#include "tests.h"

#include <stdbool.h>
#include <stdint.h>

#include "../chase.h"
#include "../combat.h"
#include "../explore.h"
#include "../map.h"
#include "../monsters.h"
#include "../move.h"
#include "../player.h"
#include "../rooms.h"
#include "../state.h"

#define COMBAT_TEST_SEED    1                   // rögzített mag: a dobássorozat minden futásban ugyanaz
#define COMBAT_TEST_SWINGS  20000               // ennyi csapás oldalanként a találati arányhoz

// a tesztmezők: a játékos és jobbra mellette a szörny, ugyanannak a folyosórendszernek a mezőin
#define COMBAT_PLAYER_X        10
#define COMBAT_PLAYER_Y        (MAP_TOP_ROW + 2)
#define COMBAT_MONSTER_X       (COMBAT_PLAYER_X + 1)
#define COMBAT_MONSTER_Y       COMBAT_PLAYER_Y
#define COMBAT_PASSAGE_NUMBER  1

#define COMBAT_TEST_MONSTER  'K'                // kestrel: az 1. mélységen 1. szintű, egy szegmensű támadás

static void put_combat_tile(uint8_t x, uint8_t y)
{
    set_map_character(x, y, CHAR_PASSAGE);
    add_map_flags(x, y, TILE_PASSAGE | COMBAT_PASSAGE_NUMBER);
}

// az 1. mélységen a kezdőértékű játékos a tesztmezőjén, szörnyek nélkül
static void put_combat_player(void)
{
    clear_map();
    clear_monsters();
    game.dungeon_level = 1;
    init_player();

    put_combat_tile(COMBAT_PLAYER_X, COMBAT_PLAYER_Y);
    game.player.x = COMBAT_PLAYER_X;
    game.player.y = COMBAT_PLAYER_Y;
    game.player.room = get_room_at(COMBAT_PLAYER_X, COMBAT_PLAYER_Y);
}

// alvó szörny az x, y mezőn; a rekordja
static struct Monster *put_combat_monster(uint8_t type, uint8_t x, uint8_t y)
{
    put_combat_tile(x, y);
    return get_monster(create_monster(type, x, y));
}

// a kezdőértékű játékos és mellette egy alvó kestrel; a kestrel rekordja
static struct Monster *put_combat_scene(void)
{
    put_combat_player();
    return put_combat_monster(COMBAT_TEST_MONSTER, COMBAT_MONSTER_X, COMBAT_MONSTER_Y);
}

static void clear_combat_scene(void)
{
    clear_monsters();
    clear_map();
}

// a sebzés nem lehet negatív
static int16_t calculate_damage_limit(int16_t damage)
{
    return damage < 0 ? 0 : damage;
}

// count csapásból ennyi találat várható: a HIT_ROLL_SIDES egyformán valószínű dobásból ennyi éri el a szükséges értéket
static uint16_t calculate_expected_hits(uint16_t count, int16_t hit_bonus, uint8_t attacker_level, int8_t defender_armor)
{
    int16_t required = HIT_TARGET_BASE - (int16_t)attacker_level - defender_armor;
    uint16_t hitting_rolls = 0;
    for (int16_t roll = 0; roll < HIT_ROLL_SIDES; roll++) {
        if (roll + hit_bonus >= required) {
            hitting_rolls++;
        }
    }
    return (uint32_t)count * hitting_rolls / HIT_ROLL_SIDES;
}

// a kapott találatszám legfeljebb a csapások 1%-ával tér el a várttól
static bool is_hit_count_close(uint16_t hits, uint16_t expected, uint16_t count)
{
    uint16_t tolerance = count / 100;
    return hits + tolerance >= expected && hits <= expected + tolerance;
}

// a táblák értékei a szakaszok határain és a két végén
int test_strength_bonuses(uint16_t i)
{
    return
        strength_hit_bonus[0] == -7 &&
        strength_hit_bonus[6] == -1 &&
        strength_hit_bonus[7] == 0 &&
        strength_hit_bonus[16] == 0 &&
        strength_hit_bonus[17] == 1 &&
        strength_hit_bonus[20] == 1 &&
        strength_hit_bonus[21] == 2 &&
        strength_hit_bonus[30] == 2 &&
        strength_hit_bonus[STRENGTH_MAX] == 3 &&
        strength_damage_bonus[0] == -7 &&
        strength_damage_bonus[6] == -1 &&
        strength_damage_bonus[7] == 0 &&
        strength_damage_bonus[15] == 0 &&
        strength_damage_bonus[16] == 1 &&
        strength_damage_bonus[17] == 1 &&
        strength_damage_bonus[18] == 2 &&
        strength_damage_bonus[19] == 3 &&
        strength_damage_bonus[20] == 3 &&
        strength_damage_bonus[21] == 4 &&
        strength_damage_bonus[22] == 5 &&
        strength_damage_bonus[30] == 5 &&
        strength_damage_bonus[STRENGTH_MAX] == 6;
}

// a kezdőértékű, fegyver nélküli játékos a kestrelt a képletből számolt eséllyel találja el (most 8/20);
// találatnál a sebzés a fegyver nélküli kocka és az erő sebzésbónusza, hibánál nincs sebzés
int test_player_hit_rate(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    int16_t damage_bonus = strength_damage_bonus[INITIAL_STRENGTH];
    int16_t damage_min = calculate_damage_limit(UNARMED_DAMAGE_DICE + damage_bonus);
    int16_t damage_max = calculate_damage_limit(UNARMED_DAMAGE_DICE * UNARMED_DAMAGE_SIDES + damage_bonus);
    bool ok = true;
    uint16_t hits = 0;

    for (uint16_t swing = 0; swing < COMBAT_TEST_SWINGS; swing++) {
        monster->hit_points = monster->max_hit_points;
        bool did_hit = attack_monster(monster);
        int16_t damage = monster->max_hit_points - monster->hit_points;
        if (did_hit) {
            hits++;
            if (damage < damage_min || damage > damage_max) {
                ok = false;
            }
        } else if (damage != 0) {
            ok = false;
        }
    }

    uint16_t expected = calculate_expected_hits(COMBAT_TEST_SWINGS, strength_hit_bonus[INITIAL_STRENGTH], game.player.experience_level, monster->armor);
    if (!is_hit_count_close(hits, expected, COMBAT_TEST_SWINGS)) {
        ok = false;
    }

    clear_combat_scene();
    return ok;
}

// a kestrel a páncél nélküli játékost a képletből számolt eséllyel találja el (most 11/20);
// találatnál a sebzés a típus egyetlen szegmense (a szörny erejének sebzésbónusza 0), hibánál nincs sebzés
int test_monster_hit_rate(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    const struct MonsterType *type_data = &monster_types[COMBAT_TEST_MONSTER - 'A'];
    const struct DamageSegment *segment = &type_data->damage[0];
    int16_t damage_bonus = strength_damage_bonus[MONSTER_STRENGTH];
    int16_t damage_min = calculate_damage_limit(segment->dice + damage_bonus);
    int16_t damage_max = calculate_damage_limit(segment->dice * segment->sides + damage_bonus);
    bool ok = type_data->damage_segment_count == 1;
    uint16_t hits = 0;

    for (uint16_t swing = 0; swing < COMBAT_TEST_SWINGS; swing++) {
        game.player.hit_points = game.player.max_hit_points;
        bool did_hit = attack_player(monster);
        int16_t damage = game.player.max_hit_points - game.player.hit_points;
        if (did_hit) {
            hits++;
            if (damage < damage_min || damage > damage_max) {
                ok = false;
            }
        } else if (damage != 0) {
            ok = false;
        }
    }

    uint16_t expected = calculate_expected_hits(COMBAT_TEST_SWINGS, strength_hit_bonus[MONSTER_STRENGTH], monster->experience_level, calculate_player_armor());
    if (!is_hit_count_close(hits, expected, COMBAT_TEST_SWINGS)) {
        ok = false;
    }

    clear_combat_scene();
    return ok;
}

// a szörnyre lépő játékos helyben marad és megtámadja: a lépés időt visz, a szörny felébred, a futás megáll
int test_move_attack(uint16_t i)
{
    struct Monster *monster = put_combat_scene();
    bool ok = !(monster->flags & MONSTER_RUNNING);

    start_running(COMBAT_MONSTER_X - COMBAT_PLAYER_X, COMBAT_MONSTER_Y - COMBAT_PLAYER_Y, false);
    bool took_time = move_player(COMBAT_MONSTER_X - COMBAT_PLAYER_X, COMBAT_MONSTER_Y - COMBAT_PLAYER_Y);

    ok = ok &&
        took_time &&
        game.player.x == COMBAT_PLAYER_X &&
        game.player.y == COMBAT_PLAYER_Y &&
        monster->x == COMBAT_MONSTER_X &&
        monster->y == COMBAT_MONSTER_Y &&
        (monster->flags & MONSTER_RUNNING) &&
        !is_running();

    clear_combat_scene();
    return ok;
}

#define MONSTER_ATTACK_TEST_TURNS       20      // ennyi körön át támad a kestrel; a rögzített maggal közben biztosan talál
#define MONSTER_ATTACK_TEST_HIT_POINTS  1000    // a játékos HP-ja, hogy a körök alatt ne fogyjon el

// az ébren lévő, szomszédos szörny a szörnyek körében helyben marad és támad: a játékos futása megáll, a HP-ja csökken
int test_monster_attack(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    game.player.max_hit_points = MONSTER_ATTACK_TEST_HIT_POINTS;
    game.player.hit_points = MONSTER_ATTACK_TEST_HIT_POINTS;
    start_monster_running(monster);

    start_running(COMBAT_PLAYER_X - COMBAT_MONSTER_X, COMBAT_PLAYER_Y - COMBAT_MONSTER_Y, false);
    move_monsters();
    bool ok = !is_running();

    for (uint8_t turn = 1; turn < MONSTER_ATTACK_TEST_TURNS; turn++) {
        move_monsters();
    }

    ok = ok &&
        monster->x == COMBAT_MONSTER_X &&
        monster->y == COMBAT_MONSTER_Y &&
        game.player.hit_points < MONSTER_ATTACK_TEST_HIT_POINTS;

    clear_combat_scene();
    return ok;
}

#define FIGHT_TEST_ROUNDS_MAX  50               // a HP-határ ennyi körön belül leállítja a harcot (rögzített maggal)
#define FIGHT_TEST_HIT_POINTS  1000             // ennyi HP mellett a HP-határ nem állítja le a harcot, és a célpont sem fogy el

#define FIGHT_STEP_X  (COMBAT_MONSTER_X - COMBAT_PLAYER_X)
#define FIGHT_STEP_Y  (COMBAT_MONSTER_Y - COMBAT_PLAYER_Y)

static bool has_fight_target(void)
{
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        if (get_monster(index)->flags & MONSTER_TARGET) return true;
    }
    return false;
}

// szabálytalan átlónál a látott szörny ellen sem indul harc, és nem telik idő (J15)
int test_fight_diagonal(uint16_t i)
{
    put_combat_player();
    // a sarok egyik mezője járható, így a szörny látszik, a másik kő, így az átló szabálytalan
    put_combat_tile(COMBAT_PLAYER_X + 1, COMBAT_PLAYER_Y);
    struct Monster *monster = put_combat_monster(COMBAT_TEST_MONSTER, COMBAT_PLAYER_X + 1, COMBAT_PLAYER_Y + 1);

    bool took_time = start_fight(1, 1, false);
    bool ok =
        is_monster_visible(monster) &&
        !took_time &&
        !is_fighting() &&
        !has_fight_target() &&
        !(monster->flags & MONSTER_RUNNING);

    clear_combat_scene();
    return ok;
}

// a harc addig tart, amíg a játékos HP-ja a harc legnagyobb egyszeri sebzése fölött van, utána véget ér, és nem marad jelölt célpont (J23);
// kamikaze harcban a HP-határ nem számít
static bool fight_hit_limit_valid(bool is_kamikaze)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    monster->hit_points = FIGHT_TEST_HIT_POINTS;

    bool ok = start_fight(FIGHT_STEP_X, FIGHT_STEP_Y, is_kamikaze) && is_fighting() && (monster->flags & MONSTER_TARGET);
    bool has_reached_limit = false;
    int16_t max_hit = 0;
    uint8_t round = 0;

    while (is_fighting() && round < FIGHT_TEST_ROUNDS_MAX) {
        if (game.player.hit_points <= max_hit) {
            has_reached_limit = true;
        }
        int16_t hit_points_before = game.player.hit_points;
        move_monsters();
        int16_t damage = hit_points_before - game.player.hit_points;
        if (damage > max_hit) {
            max_hit = damage;
        }
        if (is_fighting()) {
            take_fight_step();
        }
        round++;
    }

    if (is_kamikaze) {
        ok = ok && has_reached_limit && is_fighting() && (monster->flags & MONSTER_TARGET);
        stop_fight();
    } else {
        ok = ok && !has_reached_limit && !is_fighting() && game.player.hit_points <= max_hit;
    }
    ok = ok && !has_fight_target();

    clear_combat_scene();
    return ok;
}

int test_fight_hit_limit(uint16_t i)
{
    return fight_hit_limit_valid(false) && fight_hit_limit_valid(true);
}

// más szörny támadása véget vet a harcnak
int test_fight_other_attack(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *target = put_combat_scene();
    target->hit_points = FIGHT_TEST_HIT_POINTS;
    game.player.max_hit_points = FIGHT_TEST_HIT_POINTS;
    game.player.hit_points = FIGHT_TEST_HIT_POINTS;
    // a másik szörny a játékos bal oldalán; később jött létre, így a célpont előtt cselekszik
    struct Monster *other = put_combat_monster(COMBAT_TEST_MONSTER, COMBAT_PLAYER_X - 1, COMBAT_PLAYER_Y);
    start_monster_running(other);

    bool ok = start_fight(FIGHT_STEP_X, FIGHT_STEP_Y, false) && is_fighting();
    move_monsters();
    ok = ok && !is_fighting() && !has_fight_target();

    clear_combat_scene();
    return ok;
}

// az elmozduló célpont véget vet a harcnak: a zavarodott kestrel előbb-utóbb ellép a játékos mellől
int test_fight_target_moves(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    monster->hit_points = FIGHT_TEST_HIT_POINTS;
    monster->flags |= MONSTER_CONFUSED;
    game.player.max_hit_points = FIGHT_TEST_HIT_POINTS;
    game.player.hit_points = FIGHT_TEST_HIT_POINTS;
    // szabad mezők a kestrel körül, hogy elléphessen
    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            put_combat_tile(COMBAT_MONSTER_X + offset_x, COMBAT_MONSTER_Y + offset_y);
        }
    }

    bool ok = start_fight(FIGHT_STEP_X, FIGHT_STEP_Y, false);
    bool has_moved = false;
    uint8_t round = 0;
    while (!has_moved && round < FIGHT_TEST_ROUNDS_MAX) {
        // amíg a célpont a helyén van, a harc tart
        if (!is_fighting()) {
            ok = false;
        }
        move_monsters();
        has_moved = monster->x != COMBAT_MONSTER_X || monster->y != COMBAT_MONSTER_Y;
        if (is_fighting()) {
            take_fight_step();
        }
        round++;
    }
    ok = ok && has_moved && !is_fighting() && !has_fight_target();

    clear_combat_scene();
    return ok;
}
