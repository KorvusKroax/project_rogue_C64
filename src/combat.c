#include "combat.h"

#include <stddef.h>
#include <string.h>

#include "explore.h"
#include "game.h"
#include "map.h"
#include "message.h"
#include "monsters.h"
#include "move.h"
#include "num.h"
#include "player.h"
#include "rng.h"
#include "state.h"
#include "texts.h"

#if MONSTER_STRENGTH > STRENGTH_MAX
    #error "a szörnyek ereje nagyobb a legnagyobb erőnél"
#endif

const int8_t strength_hit_bonus[STRENGTH_MAX + 1] = {
    -7, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

const int8_t strength_damage_bonus[STRENGTH_MAX + 1] = {
    -7, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6
};

// ideiglenes, a felszerelés issue-ig (M06-04): a kézben mindig a kezdő buzogány
static const struct DamageSegment starting_weapon_damage = { STARTING_WEAPON_DAMAGE_DICE, STARTING_WEAPON_DAMAGE_SIDES };

// az f-harc állapota: tart-e, merre, kamikaze-e (HP-határ nélkül), és a harc legnagyobb egyszeri sebzése a játékoson;
// a harc alatt nincs parancsolvasás, így mentés sem, ezért nem a játékállapotban van
static bool is_fight_active;
static int8_t fight_step_x;
static int8_t fight_step_y;
static bool is_fight_kamikaze;
static int16_t max_hit_this_fight;

// a támadás szegmensenként dob a találatra, és a találó szegmensek sebzése (a kockák és a sebzésbónusz, legalább 0)
// levonódik a védő HP-jából; igaz, ha legalább egy szegmens talált (roll_em)
static bool roll_attack(uint8_t attacker_level, int16_t hit_bonus, int16_t damage_bonus, int8_t defender_armor,
                        const struct DamageSegment *segments, uint8_t segment_count, int16_t *defender_hit_points)
{
    // a szint legfeljebb 255, a páncél legalább −128, így 8 biten nem fér el
    int16_t required = HIT_TARGET_BASE - (int16_t)attacker_level - defender_armor;
    bool did_hit = false;

    for (uint8_t segment_index = 0; segment_index < segment_count; segment_index++) {
        if ((int16_t)rnd(HIT_ROLL_SIDES) + hit_bonus >= required) {
            const struct DamageSegment *segment = &segments[segment_index];
            uint8_t dice = segment->dice;
            // ideiglenes: a Venus flytrap 0 kockával sebez; ide jön: a kockák száma a vf_hit (M05-04)
            if (dice == DAMAGE_DICE_FLYTRAP) {
                dice = 0;
            }
            int16_t damage = (int16_t)ROLL(dice, segment->sides) + damage_bonus;
            if (damage > 0) {
                *defender_hit_points -= damage;
            }
            did_hit = true;
        }
    }
    return did_hit;
}

static char monster_name_text[MONSTER_NAME_TEXT_SIZE];

const char *prepare_monster_name(const struct Monster *monster)
{
    // ide jön: szörnyérzékeléssel (SEEMONST) a nem látott szörny is a nevén
    if (!is_monster_visible(monster)) return text_something;

    // ide jön: hallucinálva a képernyőn látott betűhöz tartozó név
    strcpy(monster_name_text, text_monster_article);
    strcat(monster_name_text, text_monster_names[monster->type - 'A']);
    return monster_name_text;
}

// a hit/miss üzenet a támadás után, a változatok közül véletlenül; f-harc közben nincs üzenet
static void report_attack(const char *const *hit_texts, const char *const *miss_texts, bool did_hit,
                          const char *attacker_name, const char *defender_name)
{
    if (is_fight_active) return;

    const char *const *texts = did_hit ? hit_texts : miss_texts;
    message_format(texts[rnd(ATTACK_TEXT_VARIANT_COUNT)], attacker_name, defender_name, 0);
}

bool roll_player_attack(struct Monster *monster)
{
    // a fegyver és az erő bónusza; ide jön: a dexterity és az increase damage gyűrű bónusza
    int16_t hit_bonus = STARTING_WEAPON_HIT_BONUS + strength_hit_bonus[game.player.strength];
    int16_t damage_bonus = STARTING_WEAPON_DAMAGE_BONUS + strength_damage_bonus[game.player.strength];

    return roll_attack(game.player.experience_level, hit_bonus, damage_bonus, monster->armor,
                       &starting_weapon_damage, 1, &monster->hit_points);
}

bool roll_monster_attack(const struct Monster *monster)
{
    const struct MonsterType *type_data = &monster_types[monster->type - 'A'];
    int16_t hit_bonus = strength_hit_bonus[MONSTER_STRENGTH];
    int16_t damage_bonus = strength_damage_bonus[MONSTER_STRENGTH];
    // ugyanaz, mint a játékos ISRUN-jának vizsgálata: az ISRUN pontosan akkor hiányzik, amikor a játékos cselekvésképtelen (J1)
    if (game.turns_unable_to_act > 0) {
        hit_bonus += INACTIVE_DEFENDER_HIT_BONUS;
    }

    return roll_attack(monster->experience_level, hit_bonus, damage_bonus, calculate_player_armor(),
                       type_data->damage, type_data->damage_segment_count, &game.player.hit_points);
}

// a szörny halála (killed): a játékos megkapja az XP-jét, a szörny kikerül a táblából, „you have defeated <név>”, és jöhet a szintlépés
static void kill_monster(struct Monster *monster)
{
    game.player.experience = capped_add(game.player.experience, monster->experience);
    // ide jön: a flytrap halálával a játékos kiszabadul, és a vf_hit nullázódik
    // ide jön: a leprechaun aranya a csomagjába kerül; a csomagja a környező mezőkre esik

    // a név a kivétel előtt, amíg a szörny még a helyén van
    const char *monster_name = prepare_monster_name(monster);
    remove_monster(monster);
    message_format(text_defeated, monster_name, NULL, 0);
    update_experience_level();
}

bool attack_monster(struct Monster *monster)
{
    stop_running_and_repeating();
    // ide jön: a regen-számláló nullázása
    start_monster_running(monster);
    // ide jön: az álcázott xeroc leleplezése („wait! That's a xeroc!”), a támadás ilyenkor elmarad

    const char *monster_name = prepare_monster_name(monster);
    bool did_hit = roll_player_attack(monster);
    report_attack(text_player_hits, text_player_misses, did_hit, text_you, monster_name);

    // ide jön: CANHUH
    if (did_hit && monster->hit_points <= 0) {
        kill_monster(monster);
    }
    return did_hit;
}

// az Ice monster fagyasztása: a fagyott körök összeadódnak, és ha túl sok lesz, a játékos megfagy;
// üzenet csak akkor, ha a játékos eddig cselekvőképes volt
static void freeze_player(const char *monster_name)
{
    if (game.turns_unable_to_act == 0) {
        message_format(text_frozen_by, monster_name, NULL, 0);
    }
    game.turns_unable_to_act += FREEZE_TURNS_MIN + rnd(FREEZE_TURNS_RANDOM);
    if (game.turns_unable_to_act > HYPOTHERMIA_TURNS) {
        kill_player(DEATH_CAUSE_HYPOTHERMIA);
    }
}

// a Vampire és a Wraith szívása: a HP és a max. HP ugyanannyival csökken, de a HP legalább 1 marad;
// ha a max. HP elfogy, a játékos meghal
static void drain_hit_points(uint8_t amount, uint8_t cause)
{
    game.player.hit_points -= amount;
    game.player.max_hit_points -= amount;
    if (game.player.hit_points <= 0) {
        game.player.hit_points = 1;
    }
    if (game.player.max_hit_points <= 0) {
        kill_player(cause);
        return;
    }
    message(text_suddenly_weaker);
}

// a Wraith szintszívása: egy szinttel lejjebb, és az XP az új szint alsó küszöbére áll, így a következő XP-szerzés
// nem adja vissza a szintet (J2); XP nélkül a játékos meghal
static void drain_experience_level(void)
{
    if (game.player.experience == 0) {
        kill_player('W');
        return;
    }
    game.player.experience_level--;
    if (game.player.experience_level <= 1) {
        game.player.experience_level = 1;
        game.player.experience = 0;
    } else {
        game.player.experience = experience_thresholds[game.player.experience_level - 2];
    }
    drain_hit_points(ROLL(1, WRAITH_DRAIN_SIDES), 'W');
}

// a szörny különleges hatása a sikeres találata után
static void apply_special_attack(const struct Monster *monster, const char *monster_name)
{
    switch (monster->type) {
        case 'I':
            freeze_player(monster_name);
            break;
        case 'V':
            if (rnd(100) < VAMPIRE_DRAIN_CHANCE) {
                drain_hit_points(ROLL(1, VAMPIRE_DRAIN_SIDES), 'V');
            }
            break;
        case 'W':
            if (rnd(100) < WRAITH_DRAIN_CHANCE) {
                drain_experience_level();
            }
            break;
        // ide jön: az Aquator, a Rattlesnake, a Venus flytrap, a Leprechaun és a Nymph
    }
}

bool attack_player(struct Monster *monster)
{
    stop_running_and_repeating();
    // ide jön: a regen-számláló nullázása
    if (is_fight_active && !(monster->flags & MONSTER_TARGET)) {
        stop_fight();
    }
    // ide jön: az álcázott xeroc leleplezése

    const char *monster_name = prepare_monster_name(monster);
    int16_t hit_points_before = game.player.hit_points;
    bool did_hit = roll_monster_attack(monster);
    // az Ice monster támadása nem ír hit/miss üzenetet: a találatáról a fagyasztás szól
    if (monster->type != 'I') {
        report_attack(text_monster_hits, text_monster_misses, did_hit, monster_name, text_you);
    }

    // a halállal minden véget ér: az f-harc sem folytatódik, a speciális támadás elmarad
    if (game.player.hit_points <= 0) {
        kill_player(monster->type);
        stop_fight();
        return did_hit;
    }
    if (did_hit && is_fight_active && !is_fight_kamikaze) {
        int16_t damage = hit_points_before - game.player.hit_points;
        if (damage > max_hit_this_fight) {
            max_hit_this_fight = damage;
        }
        if (game.player.hit_points <= max_hit_this_fight) {
            stop_fight();
        }
    }
    if (did_hit && !(monster->flags & MONSTER_CANCELLED)) {
        apply_special_attack(monster, monster_name);
        // a különleges hatás is ölhet: ilyenkor az f-harc sem folytatódik
        if (is_player_dead()) {
            stop_fight();
        }
    }
    return did_hit;
}

bool start_fight(int8_t step_x, int8_t step_y, bool is_kamikaze)
{
    // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
    uint8_t target_x = game.player.x + step_x;
    uint8_t target_y = game.player.y + step_y;
    uint8_t monster_index = is_on_map(target_x, target_y) ? find_monster_at(target_x, target_y) : NO_MONSTER;

    // ide jön: szörnyérzékeléssel (SEEMONST) a nem látott szörny is lehet célpont
    if (monster_index == NO_MONSTER || !is_monster_visible(get_monster(monster_index))) {
        message(text_no_monster_there);
        return false;
    }
    // szabálytalan átlónál semmi nem történik (J15)
    if (!is_diagonal_step_allowed(game.player.x, game.player.y, target_x, target_y)) return false;

    get_monster(monster_index)->flags |= MONSTER_TARGET;
    is_fight_active = true;
    fight_step_x = step_x;
    fight_step_y = step_y;
    is_fight_kamikaze = is_kamikaze;
    max_hit_this_fight = 0;
    return take_fight_step();
}

void stop_fight(void)
{
    is_fight_active = false;
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        get_monster(index)->flags &= ~MONSTER_TARGET;
    }
}

bool is_fighting(void)
{
    return is_fight_active;
}

bool take_fight_step(void)
{
    if (!move_player(fight_step_x, fight_step_y)) {
        stop_fight();
        return false;
    }
    return true;
}
