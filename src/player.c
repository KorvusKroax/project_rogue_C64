#include "player.h"

#include <stddef.h>
#include <string.h>

#include "items.h"
#include "message.h"
#include "rng.h"
#include "state.h"
#include "texts.h"

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

const uint32_t experience_thresholds[EXPERIENCE_THRESHOLD_COUNT] = {
    10, 20, 40, 80, 160, 320, 640, 1300, 2600, 5200, 13000, 26000,
    50000, 100000, 200000, 400000, 800000, 2000000, 4000000, 8000000
};

#define NO_DEATH_CAUSE  0xff                    // a játékos él; se nem szörnybetű, se nem DEATH_CAUSE_ kód

// a halál oka; halott játékossal nincs parancsolvasás, így mentés sem, ezért nem a játékállapotban van
static uint8_t death_cause;

void init_player(void)
{
    death_cause = NO_DEATH_CAUSE;
    game.player.strength = INITIAL_STRENGTH;
    game.max_strength = INITIAL_STRENGTH;
    game.player.hit_points = INITIAL_HIT_POINTS;
    game.player.max_hit_points = INITIAL_HIT_POINTS;
    game.player.experience_level = 1;
    game.player.experience = 0;
    game.turns_unable_to_act = 0;
    game.flytrap_grip = 0;
    memset(game.timers, 0, sizeof(game.timers));
    game.gold = 0;
}

void kill_player(uint8_t cause)
{
    death_cause = cause;
}

bool is_player_dead(void)
{
    return death_cause != NO_DEATH_CAUSE;
}

uint8_t get_death_cause(void)
{
    return death_cause;
}

// ide jön: a védelem-gyűrűk
int8_t calculate_player_armor(void)
{
    if (game.worn_armor == NO_ITEM) return BASE_ARMOR;
    return get_item(game.worn_armor)->properties.armor_class;
}

// az erő növelése vagy csökkentése STRENGTH_MIN és STRENGTH_MAX között (add_str)
static uint8_t add_strength(uint8_t strength, int8_t amount)
{
    int16_t result = (int16_t)strength + amount;
    if (result < STRENGTH_MIN) return STRENGTH_MIN;
    if (result > STRENGTH_MAX) return STRENGTH_MAX;
    return result;
}

void change_strength(int8_t amount)
{
    if (amount == 0) return;

    game.player.strength = add_strength(game.player.strength, amount);
    // ide jön: a gyűrűk nélküli erő a viselt add strength gyűrűk módosítója nélkül (add_strength-tel levonva)
    uint8_t strength_without_rings = game.player.strength;
    if (strength_without_rings > game.max_strength) {
        game.max_strength = strength_without_rings;
    }
}

void update_experience_level(void)
{
    // a küszöbök növekvők, így az első el nem értnél meg lehet állni
    uint8_t level = 1;
    while (level < EXPERIENCE_LEVEL_MAX && game.player.experience >= experience_thresholds[level - 1]) {
        level++;
    }
    if (level <= game.player.experience_level) return;

    int16_t hit_point_gain = ROLL(level - game.player.experience_level, LEVEL_UP_HIT_DIE_SIDES);
    game.player.max_hit_points += hit_point_gain;
    game.player.hit_points += hit_point_gain;
    game.player.experience_level = level;
    message_format(text_welcome_to_level, NULL, NULL, level);
}

#pragma optimize(pop)
