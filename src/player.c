#include "player.h"

#include "state.h"

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

// amíg nincs páncél és gyűrű, az alap
int8_t calculate_player_armor(void)
{
    return BASE_ARMOR;
}
