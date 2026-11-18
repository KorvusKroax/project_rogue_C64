#include "player.h"

#include <stddef.h>

#include "message.h"
#include "rng.h"
#include "state.h"
#include "texts.h"

const uint32_t experience_thresholds[EXPERIENCE_THRESHOLD_COUNT] = {
    10, 20, 40, 80, 160, 320, 640, 1300, 2600, 5200, 13000, 26000,
    50000, 100000, 200000, 400000, 800000, 2000000, 4000000, 8000000
};

void init_player(void)
{
    game.player.strength = INITIAL_STRENGTH;
    game.max_strength = INITIAL_STRENGTH;
    game.player.hit_points = INITIAL_HIT_POINTS;
    game.player.max_hit_points = INITIAL_HIT_POINTS;
    game.player.experience_level = 1;
    game.player.experience = 0;
    game.gold = 0;
}

// ideiglenes, a felszerelés issue-ig (M06-04): mindig a kezdő gyűrűs ing; ide jön: a védelem-gyűrűk
int8_t calculate_player_armor(void)
{
    return STARTING_ARMOR_CLASS;
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
