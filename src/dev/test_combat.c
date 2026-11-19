#include "tests.h"
#include "checks.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../chase.h"
#include "../combat.h"
#include "../explore.h"
#include "../map.h"
#include "../monsters.h"
#include "../move.h"
#include "../player.h"
#include "../rooms.h"
#include "../state.h"
#include "../texts.h"
#include "../view.h"

#define COMBAT_TEST_SEED   1                   // rögzített mag: a dobássorozat minden futásban ugyanaz
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

#define MONSTER_NAME_TEST_INVISIBLE  'P'        // phantom: láthatatlan

// a látott szörny neve "the " + a típus neve, a nem látotté "something"; minden típus neve elfér a pufferben
int test_monster_names(uint16_t i)
{
    // a rekord címe a táblából, nem a létrehozó függvény visszatérési értékéből: ez utóbbi Oscar64-hibát vált ki a move_player-ben
    // (README, Oscar64-hibák, 8.)
    put_combat_scene();
    bool ok = strcmp(prepare_monster_name(&game.monsters[0]), "the kestrel") == 0;

    put_combat_monster(MONSTER_NAME_TEST_INVISIBLE, COMBAT_PLAYER_X - 1, COMBAT_PLAYER_Y);
    ok = ok && strcmp(prepare_monster_name(&game.monsters[1]), text_something) == 0;

    for (uint8_t type_index = 0; type_index < MONSTER_TYPE_COUNT; type_index++) {
        if (strlen(text_monster_article) + strlen(text_monster_names[type_index]) >= MONSTER_NAME_TEXT_SIZE) {
            ok = false;
        }
    }
    if (strlen(text_something) >= MONSTER_NAME_TEXT_SIZE) {
        ok = false;
    }

    clear_combat_scene();
    return ok;
}

// az 1. szintű, kezdőfelszerelésű játékos (buzogány) a kestrelt a képletből számolt eséllyel találja el (most 9/20);
// találatnál a sebzés a buzogány kockái, a sebzésbónusza és az erő sebzésbónusza, hibánál nincs sebzés
int test_player_hit_rate(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    int16_t hit_bonus = STARTING_WEAPON_HIT_BONUS + strength_hit_bonus[INITIAL_STRENGTH];
    int16_t damage_bonus = STARTING_WEAPON_DAMAGE_BONUS + strength_damage_bonus[INITIAL_STRENGTH];
    int16_t damage_min = calculate_damage_limit(STARTING_WEAPON_DAMAGE_DICE + damage_bonus);
    int16_t damage_max = calculate_damage_limit(STARTING_WEAPON_DAMAGE_DICE * STARTING_WEAPON_DAMAGE_SIDES + damage_bonus);
    bool ok = true;
    uint16_t hits = 0;

    for (uint16_t swing = 0; swing < COMBAT_TEST_SWINGS; swing++) {
        monster->hit_points = monster->max_hit_points;
        bool did_hit = roll_player_attack(monster);
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

    uint16_t expected = calculate_expected_hits(COMBAT_TEST_SWINGS, hit_bonus, game.player.experience_level, monster->armor);
    if (!is_hit_count_close(hits, expected, COMBAT_TEST_SWINGS)) {
        ok = false;
    }

    clear_combat_scene();
    return ok;
}

// a kestrel a gyűrűs inges játékost a képletből számolt eséllyel találja el: a cselekvőképest most 7/20, a cselekvésképtelent
// a +4 miatt 11/20 eséllyel; találatnál a sebzés a típus egyetlen szegmense (a szörny erejének sebzésbónusza 0), hibánál nincs sebzés
static bool monster_hit_rate_valid(bool is_player_unable_to_act)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    int16_t hit_bonus = strength_hit_bonus[MONSTER_STRENGTH];
    if (is_player_unable_to_act) {
        game.turns_unable_to_act = 1;
        hit_bonus += INACTIVE_DEFENDER_HIT_BONUS;
    }
    const struct MonsterType *type_data = &monster_types[COMBAT_TEST_MONSTER - 'A'];
    const struct DamageSegment *segment = &type_data->damage[0];
    int16_t damage_bonus = strength_damage_bonus[MONSTER_STRENGTH];
    int16_t damage_min = calculate_damage_limit(segment->dice + damage_bonus);
    int16_t damage_max = calculate_damage_limit(segment->dice * segment->sides + damage_bonus);
    bool ok = type_data->damage_segment_count == 1;
    uint16_t hits = 0;

    for (uint16_t swing = 0; swing < COMBAT_TEST_SWINGS; swing++) {
        game.player.hit_points = game.player.max_hit_points;
        bool did_hit = roll_monster_attack(monster);
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

    uint16_t expected = calculate_expected_hits(COMBAT_TEST_SWINGS, hit_bonus, monster->experience_level, calculate_player_armor());
    if (!is_hit_count_close(hits, expected, COMBAT_TEST_SWINGS)) {
        ok = false;
    }

    clear_combat_scene();
    return ok;
}

int test_monster_hit_rate(uint16_t i)
{
    return monster_hit_rate_valid(false) && monster_hit_rate_valid(true);
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
// kamikaze harcban a HP-határ nem számít: a harc a határ elérése után is tart (a teszt itt megáll, mielőtt a játékos meghalna)
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
            break;
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
    // a harcot a HP-határ állította le (vagy a kamikaze még tart), nem a játékos halála
    ok = ok && !is_player_dead() && !has_fight_target();

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

#define KILL_TEST_TRIES_MAX      100            // ennyi ütésen belül a játékos biztosan eltalálja a szörnyet (rögzített maggal)
#define EXPERIENCE_CAP           16777215UL     // az XP telítési határa (num.c, capped_add)

// a szörnyre lépő játékos addig üt, amíg a szörny meg nem hal: a szörny kikerül a táblából, a mezőjén nincs TILE_MONSTER bit,
// és az alatta lévő jel látszik; a játékos helyben marad, és megkapja az XP-t, amely a határon telítődik, és a legnagyobb szintet adja
int test_monster_kill(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    monster->hit_points = 1;
    uint8_t character_below = monster->character_below;
    game.player.experience = EXPERIENCE_CAP - 1;

    uint8_t tries = 0;
    while (game.monster_count > 0 && tries < KILL_TEST_TRIES_MAX) {
        move_player(FIGHT_STEP_X, FIGHT_STEP_Y);
        tries++;
    }

    bool ok =
        game.monster_count == 0 &&
        !(get_map_flags(COMBAT_MONSTER_X, COMBAT_MONSTER_Y) & TILE_MONSTER) &&
        get_seen_character(COMBAT_MONSTER_X, COMBAT_MONSTER_Y) == character_below &&
        game.player.x == COMBAT_PLAYER_X &&
        game.player.y == COMBAT_PLAYER_Y &&
        game.player.experience == EXPERIENCE_CAP &&
        game.player.experience_level == EXPERIENCE_LEVEL_MAX;

    clear_combat_scene();
    return ok;
}

// a középső szörny kivétele után a többi sorrendje marad, a kivett mezőjén nincs TILE_MONSTER bit, és a bitek a táblával egyeznek
int test_monster_removal(uint16_t i)
{
    put_combat_player();
    put_combat_monster('A', COMBAT_PLAYER_X + 2, COMBAT_PLAYER_Y);
    put_combat_monster('B', COMBAT_PLAYER_X + 3, COMBAT_PLAYER_Y);
    put_combat_monster('C', COMBAT_PLAYER_X + 4, COMBAT_PLAYER_Y);

    // a rekord címe a táblából (README, Oscar64-hibák, 8.)
    remove_monster(&game.monsters[1]);

    bool ok =
        game.monster_count == 2 &&
        get_monster(1)->type == 'A' &&
        get_monster(2)->type == 'C' &&
        get_monster(2)->x == COMBAT_PLAYER_X + 4 &&
        !(get_map_flags(COMBAT_PLAYER_X + 3, COMBAT_PLAYER_Y) & TILE_MONSTER) &&
        are_monster_tiles_valid();

    clear_combat_scene();
    return ok;
}

// a kamikaze f-harc a célpont halálával véget ér
int test_fight_target_death(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    monster->hit_points = 1;
    game.player.max_hit_points = FIGHT_TEST_HIT_POINTS;
    game.player.hit_points = FIGHT_TEST_HIT_POINTS;

    start_fight(FIGHT_STEP_X, FIGHT_STEP_Y, true);
    uint8_t round = 0;
    while (is_fighting() && round < FIGHT_TEST_ROUNDS_MAX) {
        move_monsters();
        if (is_fighting()) {
            take_fight_step();
        }
        round++;
    }
    bool ok = !is_fighting() && game.monster_count == 0;

    clear_combat_scene();
    return ok;
}

// az XP-hez igazított szint a várt, és a max. HP és a HP ugyanannyival nő: szintenként egy kockányival
static bool experience_level_valid(uint8_t start_level, uint32_t experience, uint8_t expected_level)
{
    init_player();
    game.player.experience_level = start_level;
    game.player.experience = experience;
    int16_t max_hit_points_before = game.player.max_hit_points;
    int16_t hit_points_before = game.player.hit_points;

    update_experience_level();

    uint8_t gained_levels = expected_level - start_level;
    int16_t gain = game.player.max_hit_points - max_hit_points_before;
    return
        game.player.experience_level == expected_level &&
        game.player.hit_points - hit_points_before == gain &&
        gain >= gained_levels &&
        gain <= gained_levels * LEVEL_UP_HIT_DIE_SIDES;
}

// a küszöbök (xp_next) és a szintlépés: a küszöb alatt marad, a küszöbön lép; 45 XP-vel az 1.-ről a 4.-re;
// a legnagyobb szint 21; lejjebb nem visz
int test_experience_levels(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;

    bool ok =
        experience_thresholds[0] == 10 &&
        experience_thresholds[EXPERIENCE_THRESHOLD_COUNT - 1] == 8000000 &&
        experience_level_valid(1, 9, 1) &&
        experience_level_valid(1, 10, 2) &&
        experience_level_valid(1, 45, 4) &&
        experience_level_valid(1, experience_thresholds[EXPERIENCE_THRESHOLD_COUNT - 1] - 1, EXPERIENCE_LEVEL_MAX - 1) &&
        experience_level_valid(1, experience_thresholds[EXPERIENCE_THRESHOLD_COUNT - 1], EXPERIENCE_LEVEL_MAX) &&
        experience_level_valid(1, EXPERIENCE_CAP, EXPERIENCE_LEVEL_MAX) &&
        experience_level_valid(4, 5, 4);

    init_player();
    return ok;
}

#define SPECIAL_TEST_HIT_POINTS     1000        // a játékos HP-ja és max. HP-ja, hogy a sebzés és a szívás ne ölje meg
#define SPECIAL_TEST_ATTACKS_MAX    1000        // ennyi támadáson belül a várt hatás biztosan bekövetkezik (rögzített maggal)
#define CANCELLED_TEST_ATTACKS      200         // ennyi támadásból a kikapcsolt Ice monster biztosan talál is (rögzített maggal)
#define VAMPIRE_TEST_ATTACKS        2000        // ennyi támadásból mérve a szívás arányát
#define WRAITH_TEST_LEVEL           3           // a Wraith ezt a szintet szívja le lépésenként

// a kezdőértékű játékos SPECIAL_TEST_HIT_POINTS HP-val, és mellette egy type típusú szörny;
// a szörny rekordja a tesztekben a táblából jön (README, Oscar64-hibák, 8.)
static void put_special_attack_scene(uint8_t type)
{
    put_combat_player();
    game.player.hit_points = SPECIAL_TEST_HIT_POINTS;
    game.player.max_hit_points = SPECIAL_TEST_HIT_POINTS;
    put_combat_monster(type, COMBAT_MONSTER_X, COMBAT_MONSTER_Y);
}

// az Ice monster találata nem sebez, és FREEZE_TURNS_MIN..FREEZE_TURNS_MIN + FREEZE_TURNS_RANDOM - 1 körre megfagyasztja
// a játékost; a fagyott körök összeadódnak, és HYPOTHERMIA_TURNS fölött a játékos megfagy; a kikapcsolt Ice monster nem fagyaszt
int test_ice_monster(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    put_special_attack_scene('I');
    bool ok = true;

    uint16_t attacks = 0;
    while (!is_player_dead() && attacks < SPECIAL_TEST_ATTACKS_MAX) {
        uint8_t turns_before = game.turns_unable_to_act;
        bool did_hit = attack_player(&game.monsters[0]);
        uint8_t added_turns = game.turns_unable_to_act - turns_before;
        if (did_hit) {
            if (added_turns < FREEZE_TURNS_MIN || added_turns >= FREEZE_TURNS_MIN + FREEZE_TURNS_RANDOM) {
                ok = false;
            }
        } else if (added_turns != 0) {
            ok = false;
        }
        if (game.player.hit_points != SPECIAL_TEST_HIT_POINTS) {
            ok = false;
        }
        attacks++;
    }
    ok = ok &&
        is_player_dead() &&
        get_death_cause() == DEATH_CAUSE_HYPOTHERMIA &&
        game.turns_unable_to_act > HYPOTHERMIA_TURNS &&
        game.turns_unable_to_act < HYPOTHERMIA_TURNS + FREEZE_TURNS_MIN + FREEZE_TURNS_RANDOM;

    put_special_attack_scene('I');
    game.monsters[0].flags |= MONSTER_CANCELLED;
    uint16_t hits = 0;
    for (uint16_t attack = 0; attack < CANCELLED_TEST_ATTACKS; attack++) {
        if (attack_player(&game.monsters[0])) {
            hits++;
        }
    }
    ok = ok && hits > 0 && game.turns_unable_to_act == 0;

    clear_combat_scene();
    return ok;
}

// a Vampire szívása csak találatnál jön, a találatok VAMPIRE_DRAIN_CHANCE százalékában (±5 százalékpont), és a max. HP-t
// 1..VAMPIRE_DRAIN_SIDES-szal csökkenti; ha a max. HP elfogy, a szívás megöli a játékost, akkor is, ha a HP-ja még bőven van
int test_vampire(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    put_special_attack_scene('V');
    bool ok = true;
    uint16_t hits = 0;
    uint16_t drains = 0;

    for (uint16_t attack = 0; attack < VAMPIRE_TEST_ATTACKS; attack++) {
        game.player.hit_points = SPECIAL_TEST_HIT_POINTS;
        game.player.max_hit_points = SPECIAL_TEST_HIT_POINTS;
        bool did_hit = attack_player(&game.monsters[0]);
        int16_t drained = SPECIAL_TEST_HIT_POINTS - game.player.max_hit_points;
        if (did_hit) {
            hits++;
        }
        if (drained != 0) {
            drains++;
            if (!did_hit || drained < 1 || drained > VAMPIRE_DRAIN_SIDES) {
                ok = false;
            }
        }
    }
    uint16_t expected = (uint32_t)hits * VAMPIRE_DRAIN_CHANCE / 100;
    uint16_t tolerance = hits / 20;
    ok = ok && drains + tolerance >= expected && drains <= expected + tolerance;

    game.player.hit_points = SPECIAL_TEST_HIT_POINTS;
    game.player.max_hit_points = 1;
    uint16_t attacks = 0;
    while (!is_player_dead() && attacks < SPECIAL_TEST_ATTACKS_MAX) {
        attack_player(&game.monsters[0]);
        attacks++;
    }
    ok = ok &&
        is_player_dead() &&
        get_death_cause() == 'V' &&
        game.player.max_hit_points <= 0 &&
        game.player.hit_points > 0;

    clear_combat_scene();
    return ok;
}

// a Wraith szintszívása egy szinttel lejjebb visz, az XP az új szint alsó küszöbére áll, és a max. HP 1..WRAITH_DRAIN_SIDES-szal
// csökken; a következő kis XP-szerzés nem adja vissza a szintet (J2); az 1. szinten 0 XP-vel a következő szívás halálos
int test_wraith(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    put_special_attack_scene('W');
    game.player.experience_level = WRAITH_TEST_LEVEL;
    game.player.experience = experience_thresholds[WRAITH_TEST_LEVEL - 2] + 5;
    bool ok = true;
    uint8_t drains = 0;

    uint16_t attacks = 0;
    while (!is_player_dead() && attacks < SPECIAL_TEST_ATTACKS_MAX) {
        uint8_t level_before = game.player.experience_level;
        game.player.hit_points = SPECIAL_TEST_HIT_POINTS;
        game.player.max_hit_points = SPECIAL_TEST_HIT_POINTS;
        attack_player(&game.monsters[0]);
        attacks++;
        if (is_player_dead()) break;

        int16_t drained = SPECIAL_TEST_HIT_POINTS - game.player.max_hit_points;
        if (game.player.experience_level == level_before) {
            if (drained != 0) {
                ok = false;
            }
            continue;
        }
        drains++;
        if (drained < 1 || drained > WRAITH_DRAIN_SIDES || game.player.experience_level != level_before - 1) {
            ok = false;
        }
        if (game.player.experience_level == 1) {
            ok = ok && game.player.experience == 0;
        } else {
            ok = ok && game.player.experience == experience_thresholds[game.player.experience_level - 2];
            // egy XP-vel a szint nem jön vissza (J2)
            game.player.experience++;
            update_experience_level();
            ok = ok && game.player.experience_level == level_before - 1;
        }
    }
    ok = ok &&
        drains == WRAITH_TEST_LEVEL - 1 &&
        is_player_dead() &&
        get_death_cause() == 'W';

    clear_combat_scene();
    return ok;
}

#define FLYTRAP_TEST_ATTACKS  40                // ennyi támadásban a flytrap talál is és hibáz is, és a játékos még nem hal meg (rögzített maggal)

// attacks támadás a tábla első szörnyétől, a flytraptól; igaz, ha volt találat és hiba is, és mind a szabály szerint sebzett:
// találatnál a HP eggyel csökken, és a szorítás eggyel nő (a kikapcsolt flytrapnál egyik sem), hibánál a HP a szorításnyit csökken
static bool flytrap_attacks_valid(uint8_t attacks, bool is_cancelled)
{
    bool ok = true;
    uint8_t hits = 0;

    for (uint8_t attack = 0; attack < attacks; attack++) {
        int16_t hit_points_before = game.player.hit_points;
        uint8_t grip_before = game.flytrap_grip;
        bool did_hit = attack_player(&game.monsters[0]);
        int16_t damage = hit_points_before - game.player.hit_points;
        if (!did_hit) {
            ok = ok && damage == grip_before && game.flytrap_grip == grip_before;
        } else if (is_cancelled) {
            hits++;
            ok = ok && damage == 0 && game.flytrap_grip == grip_before;
        } else {
            hits++;
            ok = ok && damage == 1 && game.flytrap_grip == grip_before + 1;
        }
    }
    return ok && hits > 0 && hits < attacks;
}

// a Venus flytrap találata 0 kocka, de minden találata szorosabban fogja a játékost, és ez 1 HP-t visz; a mellé ütése
// a szorításnyit sebez, a kikapcsolt flytrapé is; a flytrap halála elengedi a játékost; ha a mellé ütés öl, a halál oka
// a flytrap; a szorítás 255-nél megáll
int test_flytrap(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    put_special_attack_scene('F');
    bool ok = flytrap_attacks_valid(FLYTRAP_TEST_ATTACKS, false);

    game.monsters[0].flags |= MONSTER_CANCELLED;
    ok = ok && flytrap_attacks_valid(FLYTRAP_TEST_ATTACKS, true) && !is_player_dead() && game.flytrap_grip > 0;

    game.monsters[0].hit_points = 1;
    uint8_t tries = 0;
    while (game.monster_count > 0 && tries < KILL_TEST_TRIES_MAX) {
        move_player(FIGHT_STEP_X, FIGHT_STEP_Y);
        tries++;
    }
    ok = ok && game.monster_count == 0 && game.flytrap_grip == 0;

    put_special_attack_scene('F');
    game.flytrap_grip = UINT8_MAX;
    uint16_t attacks = 0;
    while (!is_player_dead() && attacks < SPECIAL_TEST_ATTACKS_MAX) {
        attack_player(&game.monsters[0]);
        if (game.flytrap_grip != UINT8_MAX) {
            ok = false;
        }
        attacks++;
    }
    ok = ok && is_player_dead() && get_death_cause() == 'F';

    clear_combat_scene();
    return ok;
}

static bool is_player_at_start(void)
{
    return game.player.x == COMBAT_PLAYER_X && game.player.y == COMBAT_PLAYER_Y;
}

// a fogva tartott játékos (jobbra a flytrap, balra szabad mező, lent egy alvó kestrel, fent kőzet) helyben marad: a szabad mezőre,
// a kőzetbe és a kestrelre lépés is időbe kerül, a szabálytalan átló nem; a futás és az f-harc véget ér, és a kestrel nem kap ütést (J25);
// a flytrapra lépve megtámadja; a flytrap megölése után újra léphet
int test_flytrap_hold(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    put_special_attack_scene('F');
    put_combat_tile(COMBAT_PLAYER_X - 1, COMBAT_PLAYER_Y);
    put_combat_monster(COMBAT_TEST_MONSTER, COMBAT_PLAYER_X, COMBAT_PLAYER_Y + 1);
    game.flytrap_grip = 1;

    bool ok =
        move_player(-1, 0) && is_player_at_start() &&
        move_player(0, -1) && is_player_at_start() &&
        !move_player(-1, -1) && is_player_at_start();

    // a futás lépése a move_player; a take_run_step itt az Oscar64 figyelmeztetését váltaná ki (README, Oscar64-hibák, 9.)
    start_running(-1, 0, false);
    ok = ok && move_player(-1, 0) && !is_running() && is_player_at_start();

    ok = ok &&
        start_fight(0, 1, false) &&
        !is_fighting() &&
        !has_fight_target() &&
        !(game.monsters[1].flags & MONSTER_RUNNING) &&
        is_player_at_start();

    ok = ok && move_player(FIGHT_STEP_X, FIGHT_STEP_Y) && (game.monsters[0].flags & MONSTER_RUNNING) && is_player_at_start();

    game.monsters[0].hit_points = 1;
    uint8_t tries = 0;
    while (find_monster_at(COMBAT_MONSTER_X, COMBAT_MONSTER_Y) != NO_MONSTER && tries < KILL_TEST_TRIES_MAX) {
        move_player(FIGHT_STEP_X, FIGHT_STEP_Y);
        tries++;
    }
    ok = ok && game.flytrap_grip == 0 && move_player(-1, 0) && game.player.x == COMBAT_PLAYER_X - 1;

    clear_combat_scene();
    return ok;
}

#define DEATH_TEST_EXPERIENCE        100        // a halál előtti állapot, amelyet az új játék visszaállít
#define DEATH_TEST_EXPERIENCE_LEVEL  5
#define DEATH_TEST_GOLD              50

// a kamikaze f-harcban 1 HP-val a kestrel megöli a játékost: a halál oka a kestrel, a harc véget ér, és utána a szörny már nem támad;
// új játéknál (init_player) a játékos él, és a kezdőértékekkel indul
int test_player_death(uint16_t i)
{
    game.seed = COMBAT_TEST_SEED;
    struct Monster *monster = put_combat_scene();
    monster->hit_points = FIGHT_TEST_HIT_POINTS;
    game.player.hit_points = 1;

    bool ok = start_fight(FIGHT_STEP_X, FIGHT_STEP_Y, true) && !is_player_dead();
    uint8_t round = 0;
    while (!is_player_dead() && round < FIGHT_TEST_ROUNDS_MAX) {
        move_monsters();
        if (is_fighting()) {
            take_fight_step();
        }
        round++;
    }

    int16_t hit_points_at_death = game.player.hit_points;
    move_monsters();
    ok = ok &&
        is_player_dead() &&
        get_death_cause() == COMBAT_TEST_MONSTER &&
        !is_fighting() &&
        hit_points_at_death <= 0 &&
        game.player.hit_points == hit_points_at_death;

    game.player.experience = DEATH_TEST_EXPERIENCE;
    game.player.experience_level = DEATH_TEST_EXPERIENCE_LEVEL;
    game.gold = DEATH_TEST_GOLD;
    init_player();
    ok = ok &&
        !is_player_dead() &&
        game.player.hit_points == INITIAL_HIT_POINTS &&
        game.player.max_hit_points == INITIAL_HIT_POINTS &&
        game.player.experience_level == 1 &&
        game.player.experience == 0 &&
        game.gold == 0;

    clear_combat_scene();
    return ok;
}
