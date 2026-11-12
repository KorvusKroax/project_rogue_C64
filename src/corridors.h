#ifndef CORRIDORS_H
#define CORRIDORS_H

#include <stdint.h>

#define MAX_EXTRA_CORRIDORS  4                  // a feszítőfa után legfeljebb ennyi extra folyosó-próbálkozás (default: 4)

void connect_rooms(void);                       // a szomszédos szobák összekötése: véletlen feszítőfa, utána néhány extra él; a szobák után kell hívni (generate_rooms)
void put_passage(uint8_t x, uint8_t y);         // látható folyosó-mező a labirintusnak és a folyosóknak; hogy titkos-e, azt a hide_passages dönti el

// mélyebben a szabad folyosó-mezők egy része titkos lesz (üresnek látszik); a kept_x, kept_y mező (a játékosé) mindig látható marad,
// a térképen kívüli pozícióval egyik sem; a lépcső és a játékos helye után, a többi elhelyezés előtt kell hívni
void hide_passages(uint8_t kept_x, uint8_t kept_y);

#endif
