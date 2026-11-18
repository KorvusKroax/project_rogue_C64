#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#define BASE_ARMOR  10                  // páncél nélkül ennyi a védelem (default: 10)

#define STRENGTH_MAX  31                // az erő legfeljebb ennyi; a harci táblák ennyivel indexelnek (default: 31)

#define INITIAL_STRENGTH    16          // a kezdő erő és legnagyobb erő (default: 16)
#define INITIAL_HIT_POINTS  12          // a kezdő hp és max. hp (default: 12)

#if INITIAL_STRENGTH > STRENGTH_MAX
    #error "a kezdő erő nagyobb a legnagyobb erőnél"
#endif

// ideiglenes, a felszerelés issue-ig (M06-04): a kezdőfelszerelés rögzített adatként, a kézben a buzogány, viselve a gyűrűs ing
#define STARTING_WEAPON_DAMAGE_DICE   2 // buzogány: 2x4 (default: 2)
#define STARTING_WEAPON_DAMAGE_SIDES  4 // (default: 4)
#define STARTING_WEAPON_HIT_BONUS     1 // +1 (default: 1)
#define STARTING_WEAPON_DAMAGE_BONUS  1 // +1 (default: 1)
#define STARTING_ARMOR_CLASS          6 // gyűrűs ing: 7 − 1 (default: 6)

// a szint 1 + az XP-vel elért küszöbök száma (xp_next)
#define EXPERIENCE_THRESHOLD_COUNT  20                                  // (default: 20)
#define EXPERIENCE_LEVEL_MAX        (EXPERIENCE_THRESHOLD_COUNT + 1)    // default: 21
#define LEVEL_UP_HIT_DIE_SIDES      10  // szintenként egy ilyen kocka a max. HP-hoz és a HP-hoz (default: 10)

extern const uint32_t experience_thresholds[EXPERIENCE_THRESHOLD_COUNT];

void init_player(void);                 // új játékhoz: kezdőértékek, szint 1, tapasztalat és arany 0, él

// a játékos halála; az ok egyelőre a szörny típusa ('A'..'Z'); a játék a kör végén ér véget (run_game)
void kill_player(uint8_t cause);
bool is_player_dead(void);
uint8_t get_death_cause(void);          // a kill_player oka; csak halál után értelmes

int8_t calculate_player_armor(void);    // a viselt páncél (vagy a BASE_ARMOR) a védelem-gyűrűkkel együtt; kisebb = jobb

// a szint az XP-hez igazodik: ha magasabb jár, a szintkülönbségnyi kocka a max. HP-hoz és a HP-hoz adódik,
// és „welcome to level <n>”; lejjebb nem visz (check_level)
void update_experience_level(void);

#endif
