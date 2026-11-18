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

// az f-harc (f, F) a step_x, step_y irányban álló, látott szörny ellen: az első ütés azonnal, utána minden akcióban újabb,
// amíg a célpont el nem mozdul, más szörny nem támad, vagy (kamikaze nélkül) a játékos HP-ja a harc legnagyobb egyszeri
// sebzésére vagy az alá nem esik; igaz, ha időt vitt (nem visz időt: nincs ott látott szörny, vagy szabálytalan az átló, J15)
bool start_fight(int8_t step_x, int8_t step_y, bool is_kamikaze);
void stop_fight(void);          // az f-harc vége; a célpont jelölése (MONSTER_TARGET) törlődik (J23)
bool is_fighting(void);

// az f-harc következő ütése; ha nem sikerül, a harc véget ér; igaz, ha időt vitt
bool take_fight_step(void);

#endif
