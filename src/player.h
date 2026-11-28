#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#define BASE_ARMOR  10                  // páncél nélkül ennyi a védelem (default: 10)

#define STRENGTH_MIN  3                 // az erő legalább ennyi (default: 3)
#define STRENGTH_MAX  31                // az erő legfeljebb ennyi; a harci táblák ennyivel indexelnek (default: 31)

#define INITIAL_STRENGTH    16          // a kezdő erő és legnagyobb erő (default: 16)
#define INITIAL_HIT_POINTS  12          // a kezdő hp és max. hp (default: 12)

#if INITIAL_STRENGTH < STRENGTH_MIN || INITIAL_STRENGTH > STRENGTH_MAX
    #error "a kezdő erő az erő tartományán kívül esik"
#endif

// a játékos sebzése üres kézzel (dmg)
#define EMPTY_HAND_DAMAGE_DICE   1      // (default: 1)
#define EMPTY_HAND_DAMAGE_SIDES  4      // (default: 4)

// a szint 1 + az XP-vel elért küszöbök száma (xp_next)
#define EXPERIENCE_THRESHOLD_COUNT  20                                  // (default: 20)
#define EXPERIENCE_LEVEL_MAX        (EXPERIENCE_THRESHOLD_COUNT + 1)    // default: 21
#define LEVEL_UP_HIT_DIE_SIDES      10  // szintenként egy ilyen kocka a max. HP-hoz és a HP-hoz (default: 10)

extern const uint32_t experience_thresholds[EXPERIENCE_THRESHOLD_COUNT];

void init_player(void);                 // új játékhoz: kezdőértékek, szint 1, tapasztalat és arany 0, él, nincs időzített hatás

// a halál oka: a szörny típusa ('A'..'Z'), vagy ha nem szörny okozta, egy DEATH_CAUSE_ kód (a text_death_causes indexe)
#define DEATH_CAUSE_HYPOTHERMIA  0              // fagyás (az Ice monster túl sokszor fagyasztott)
#define DEATH_CAUSE_COUNT        1

#if DEATH_CAUSE_COUNT > 'A'
    #error "a halálok-kódok ütköznek a szörnyek betűivel"
#endif

// a játékos halála; a játék a kör végén ér véget (run_game)
void kill_player(uint8_t cause);
bool is_player_dead(void);
uint8_t get_death_cause(void);          // a kill_player oka; csak halál után értelmes

// lebegve „you can't. You're floating off the ground!” (a lépcső és a `,` előtt); igaz, ha a játékos lebeg (levit_check)
bool report_floating_off_ground(void);

int8_t calculate_player_armor(void);    // a viselt páncél védelme (vagy a BASE_ARMOR) a védelem-gyűrűkkel együtt; kisebb = jobb

// az erő változása (chg_str): STRENGTH_MIN és STRENGTH_MAX között marad, és ha a gyűrűk nélküli erő a legnagyobb
// eddigi fölé nő, az is nő
void change_strength(int8_t amount);

// a szint az XP-hez igazodik: ha magasabb jár, a szintkülönbségnyi kocka a max. HP-hoz és a HP-hoz adódik,
// és „welcome to level <n>”; lejjebb nem visz (check_level)
void update_experience_level(void);

#endif
