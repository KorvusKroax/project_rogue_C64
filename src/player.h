#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

#define BASE_ARMOR  10                  // páncél nélkül ennyi a védelem (default: 10)

#define STRENGTH_MAX  31                // az erő legfeljebb ennyi; a harci táblák ennyivel indexelnek (default: 31)

#define INITIAL_STRENGTH    16          // a kezdő erő és legnagyobb erő (default: 16)
#define INITIAL_HIT_POINTS  12          // a kezdő hp és max. hp (default: 12)

#if INITIAL_STRENGTH > STRENGTH_MAX
    #error "a kezdő erő nagyobb a legnagyobb erőnél"
#endif

// fegyver nélkül a játékos ütése egy ilyen sebzés-szegmens
#define UNARMED_DAMAGE_DICE   1         // (default: 1)
#define UNARMED_DAMAGE_SIDES  4         // (default: 4)

void init_player(void);                 // új játékhoz: kezdőértékek, szint 1, tapasztalat és arany 0

int8_t calculate_player_armor(void);    // a viselt páncél (vagy a BASE_ARMOR) a védelem-gyűrűkkel együtt; kisebb = jobb

#endif
