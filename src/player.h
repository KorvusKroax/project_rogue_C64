#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

#define BASE_ARMOR  10                  // páncél nélkül ennyi a védelem (default: 10)

int8_t calculate_player_armor(void);    // a viselt páncél (vagy a BASE_ARMOR) a védelem-gyűrűkkel együtt; kisebb = jobb

#endif
