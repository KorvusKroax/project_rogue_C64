#include "combat.h"

#include "game.h"
#include "monsters.h"
#include "player.h"
#include "rng.h"
#include "state.h"

#if MONSTER_STRENGTH > STRENGTH_MAX
    #error "a szörnyek ereje nagyobb a legnagyobb erőnél"
#endif

const int8_t strength_hit_bonus[STRENGTH_MAX + 1] = {
    -7, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

const int8_t strength_damage_bonus[STRENGTH_MAX + 1] = {
    -7, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6
};

static const struct DamageSegment unarmed_damage = { UNARMED_DAMAGE_DICE, UNARMED_DAMAGE_SIDES };

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
            int16_t damage = (int16_t)ROLL(segment->dice, segment->sides) + damage_bonus;
            if (damage > 0) {
                *defender_hit_points -= damage;
            }
            did_hit = true;
        }
    }
    return did_hit;
}

bool attack_monster(struct Monster *monster)
{
    stop_running_and_repeating();
    // ide jön: a regen-számláló nullázása
    start_monster_running(monster);
    // ide jön: az álcázott xeroc leleplezése („wait! That's a xeroc!”), a támadás ilyenkor elmarad

    // fegyver nélkül; ide jön: a dexterity és az increase damage gyűrű bónusza
    int16_t hit_bonus = strength_hit_bonus[game.player.strength];
    int16_t damage_bonus = strength_damage_bonus[game.player.strength];

    // a szörny már ébren van, így a támadó nem kap +4-et
    bool did_hit = roll_attack(game.player.experience_level, hit_bonus, damage_bonus, monster->armor,
                               &unarmed_damage, 1, &monster->hit_points);

    // ide jön: a hit/miss üzenet, CANHUH, a szörny halála
    return did_hit;
}

bool attack_player(struct Monster *monster)
{
    stop_running_and_repeating();
    // ide jön: a regen-számláló nullázása; az f-harc vége, ha nem ez a szörny a célpontja; az álcázott xeroc leleplezése

    const struct MonsterType *type_data = &monster_types[monster->type - 'A'];
    int16_t hit_bonus = strength_hit_bonus[MONSTER_STRENGTH];
    int16_t damage_bonus = strength_damage_bonus[MONSTER_STRENGTH];
    // ide jön: ha a játékos nincs ébren (alszik, ájult, fagyott), a szörny +4-et kap

    bool did_hit = roll_attack(monster->experience_level, hit_bonus, damage_bonus, calculate_player_armor(),
                               type_data->damage, type_data->damage_segment_count, &game.player.hit_points);

    // ide jön: a hit/miss üzenet, a játékos halála, a speciális támadás
    return did_hit;
}
