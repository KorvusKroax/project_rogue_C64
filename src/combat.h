#ifndef COMBAT_H
#define COMBAT_H

#include <stdint.h>
#include <stdbool.h>

#include "items.h"
#include "monsters.h"
#include "player.h"

// a találat: egy HIT_ROLL_SIDES oldalú kocka (0-tól) + a találati bónusz legalább HIT_TARGET_BASE − a támadó szintje − a védő páncélja
#define HIT_ROLL_SIDES   20                     // (default: 20)
#define HIT_TARGET_BASE  20                     // (default: 20)

// a mentődobás: egy SAVING_THROW_SIDES oldalú kocka (1-től) legalább a cél − a játékos szintje / 2 (save_throw)
#define SAVING_THROW_SIDES  20                  // (default: 20)
#define POISON_SAVE_TARGET  14                  // a mérgezés elleni mentődobás célja (default: 14)
#define MAGIC_SAVE_TARGET   17                  // a mágia elleni mentődobás célja (default: 17)

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

#define RUST_ARMOR_CLASS_LIMIT  9               // az Aquator rozsdája ennél a védelemnél már nem ront tovább (default: 9)
#define LEPRECHAUN_EXTRA_GOLD_ROLLS  4          // a leprechaun lopása sikertelen, a halálakor hagyott arany sikeres mágia elleni mentődobásnál
                                                // ennyi további aranydobással nagyobb (default: 4)

// a leprechaun aranya (egy aranydobás és a további dobások) az arany mennyiségének 16 bitjén és az üzenet előjeles számán elfér
#if (LEPRECHAUN_EXTRA_GOLD_ROLLS + 1) * (GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL * DUNGEON_LEVEL_MAX + GOLD_AMOUNT_MIN - 1) > 0x7fff
    #error "a leprechaun aranya nem fér el 15 biten"
#endif

// a fagyás a halálos határ alatt még egyszer hozzáadódhat, és a körök száma 8 bites
#if HYPOTHERMIA_TURNS + FREEZE_TURNS_MIN + FREEZE_TURNS_RANDOM - 1 > 255
    #error "a fagyás körei nem férnek el 8 biten"
#endif

// az erő találati és sebzésbónusza (str_plus, add_dam), az erővel indexelve
extern const int8_t strength_hit_bonus[STRENGTH_MAX + 1];
extern const int8_t strength_damage_bonus[STRENGTH_MAX + 1];

// a kézben tartott tárgy sebzése közelharcban: a fegyvereké típusonként (8.4, a „kézben” oszlop), a fegyvertípusok sorrendjében;
// a boté és a pálcáé az anyaguk szerint (8.7); minden más tárgy 0x0, így a találat csak a bónuszokat sebzi
extern const struct DamageSegment weapon_melee_damage[WEAPON_SUBTYPE_COUNT];
#define STAFF_MELEE_DICE   2                    // (default: 2)
#define STAFF_MELEE_SIDES  3                    // (default: 3)
#define WAND_MELEE_DICE    1                    // (default: 1)
#define WAND_MELEE_SIDES   1                    // (default: 1)

// a szörny neve az üzenetekben: "the " + a típus neve, a nem látott szörnyé "something"; az eredmény a következő hívásig él (set_mname)
#define MONSTER_NAME_TEXT_SIZE  18              // "the " + a leghosszabb név (13) + a lezáró nulla
const char *prepare_monster_name(const struct Monster *monster);

// a játékos megtámadja a szörnyet (fight): a szörny felébred és üldöz, a támadás dob, és f-harcon kívül
// kiíródik a hit/miss üzenet; ha a szörny meghal, kikerül a táblából (a mutató utána a következőre mutat); igaz, ha talált;
// az álcázott xeroc ellen nincs dobás, csak leleplezi („wait! That's a xeroc!”)
bool attack_monster(struct Monster *monster);

// a szörny megtámadja a játékost (attack): az álcázott xeroc előbb üzenet nélkül leleplezi magát; a támadás dob, és f-harcon
// kívül kiíródik a hit/miss üzenet (az Ice monsternél nem);
// találat után jön a különleges hatás, ha a szörnynek van és nincs kikapcsolva (MONSTER_CANCELLED); a Venus flytrap mellé ütése
// is sebez; a nimfa és a leprechaun a lopással eltűnik: ekkor a szörnytábla eggyel rövidebb, és a mutató a következő szörnyre
// mutat; igaz, ha talált
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
