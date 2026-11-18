#ifndef COMBAT_H
#define COMBAT_H

#include <stdint.h>
#include <stdbool.h>

#include "monsters.h"
#include "player.h"

// a találat: egy HIT_ROLL_SIDES oldalú kocka (0-tól) + a találati bónusz legalább HIT_TARGET_BASE − a támadó szintje − a védő páncélja
#define HIT_ROLL_SIDES   20                     // (default: 20)
#define HIT_TARGET_BASE  20                     // (default: 20)

// az erő találati és sebzésbónusza (str_plus, add_dam), az erővel indexelve
extern const int8_t strength_hit_bonus[STRENGTH_MAX + 1];
extern const int8_t strength_damage_bonus[STRENGTH_MAX + 1];

// a játékos megtámadja a szörnyet (fight): a szörny felébred és üldöz, utána a támadás dob; igaz, ha talált
bool attack_monster(struct Monster *monster);

// a szörny megtámadja a játékost (attack); igaz, ha talált
bool attack_player(struct Monster *monster);

#endif
