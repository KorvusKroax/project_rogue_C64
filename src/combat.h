#ifndef COMBAT_H
#define COMBAT_H

#include <stdint.h>
#include <stdbool.h>

#include "monsters.h"
#include "player.h"

// a találat: egy HIT_ROLL_SIDES oldalú kocka (0-tól) + a találati bónusz legalább HIT_TARGET_BASE − a támadó szintje − a védő páncélja
#define HIT_ROLL_SIDES   20                     // (default: 20)
#define HIT_TARGET_BASE  20                     // (default: 20)

// a nem cselekvő védőt (alvó szörnyet, cselekvésképtelen játékost) ennyivel könnyebb eltalálni
#define INACTIVE_DEFENDER_HIT_BONUS  4          // (default: 4)

// a különleges támadások a szörny sikeres találata után
#define FREEZE_TURNS_MIN      2                 // az Ice monster fagyasztása: ennyi + rnd(FREEZE_TURNS_RANDOM) kör (default: 2)
#define FREEZE_TURNS_RANDOM   2                 // (default: 2)
#define HYPOTHERMIA_TURNS     50                // ennél több fagyott kör halálos (default: 50)
#define VAMPIRE_DRAIN_CHANCE  30                // a Vampire ennyi százalék eséllyel szív HP-t és max. HP-t (default: 30)
#define VAMPIRE_DRAIN_SIDES   3                 // a szívás egy ekkora kocka (default: 3)
#define WRAITH_DRAIN_CHANCE   15                // a Wraith ennyi százalék eséllyel szív egy szintet (default: 15)
#define WRAITH_DRAIN_SIDES    10                // a szintszívással a HP és a max. HP egy ekkora kockányival csökken (default: 10)

// a fagyás a halálos határ alatt még egyszer hozzáadódhat, és a körök száma 8 bites
#if HYPOTHERMIA_TURNS + FREEZE_TURNS_MIN + FREEZE_TURNS_RANDOM - 1 > 255
    #error "a fagyás körei nem férnek el 8 biten"
#endif

// az erő találati és sebzésbónusza (str_plus, add_dam), az erővel indexelve
extern const int8_t strength_hit_bonus[STRENGTH_MAX + 1];
extern const int8_t strength_damage_bonus[STRENGTH_MAX + 1];

// a szörny neve az üzenetekben: "the " + a típus neve, a nem látott szörnyé "something"; az eredmény a következő hívásig él (set_mname)
#define MONSTER_NAME_TEXT_SIZE  18              // "the " + a leghosszabb név (13) + a lezáró nulla
const char *prepare_monster_name(const struct Monster *monster);

// a játékos megtámadja a szörnyet (fight): a szörny felébred és üldöz, a támadás dob, és f-harcon kívül
// kiíródik a hit/miss üzenet; ha a szörny meghal, kikerül a táblából (a mutató utána a következőre mutat); igaz, ha talált
bool attack_monster(struct Monster *monster);

// a szörny megtámadja a játékost (attack): a támadás dob, és f-harcon kívül kiíródik a hit/miss üzenet (az Ice monsternél nem);
// találat után jön a különleges hatás, ha a szörnynek van és nincs kikapcsolva (MONSTER_CANCELLED); a Venus flytrap mellé ütése
// is sebez; igaz, ha talált
bool attack_player(struct Monster *monster);

// csak a dobás és a sebzés, üzenet és más hatás nélkül; igaz, ha talált
bool roll_player_attack(struct Monster *monster);         // a szörnynek már ébren kell lennie: alvó szörnyre nem jár a +4
bool roll_monster_attack(const struct Monster *monster);  // a cselekvésképtelen játékos ellen a szörny +4-et kap

// az f-harc (f, F) a step_x, step_y irányban álló, látott szörny ellen: az első ütés azonnal, utána minden akcióban újabb,
// amíg a célpont el nem mozdul, más szörny nem támad, vagy (kamikaze nélkül) a játékos HP-ja a harc legnagyobb egyszeri
// sebzésére vagy az alá nem esik; igaz, ha időt vitt (nem visz időt: nincs ott látott szörny, vagy szabálytalan az átló, J15)
bool start_fight(int8_t step_x, int8_t step_y, bool is_kamikaze);
void stop_fight(void);          // az f-harc vége; a célpont jelölése (MONSTER_TARGET) törlődik (J23)
bool is_fighting(void);

// az f-harc következő ütése; ha nem sikerül, a harc véget ér; igaz, ha időt vitt
bool take_fight_step(void);

#endif
