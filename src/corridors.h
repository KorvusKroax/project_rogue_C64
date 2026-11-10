#ifndef CORRIDORS_H
#define CORRIDORS_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_EXTRA_CORRIDORS  4                  // a feszítőfa után legfeljebb ennyi extra folyosó-próbálkozás (default: 4)

void connect_rooms(void);                       // a szomszédos szobák összekötése: véletlen feszítőfa, utána néhány extra él; a szobák után kell hívni (generate_rooms)
void put_passage(uint8_t x, uint8_t y);         // folyosó-mező a labirintusnak és a folyosóknak; mélyebben titkos is lehet (üresnek látszik)

// ideiglenes: a folyosók után a szint-önteszt a térképen ellenőrzi, hogy minden szoba elérhető, akkor ez törlődik
#ifdef DEVMODE
    bool are_rooms_connected(uint8_t room_a, uint8_t room_b);  // a szint-önteszthez: az utolsó connect_rooms összekötötte-e a két szobát
#endif

#endif
