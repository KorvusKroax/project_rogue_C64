#ifndef CORRIDORS_H
#define CORRIDORS_H

#include <stdint.h>

// a szomszédos szobák összekötése: véletlen feszítőfa, utána néhány extra él; a folyosóvégek az ajtótáblába kerülnek;
// a szobák után kell hívni (generate_rooms)
void connect_rooms(void);
// az összefüggő folyosó- és labirintusmezők, az ajtókkal együtt, egy folyosórendszer: a sorszáma (1-től) a TILE_PASSAGE_NUMBER bitekbe
// és az ajtók bejegyzésébe kerül, a rekordja a szobatábla végén van (state.h); a szobák indexe, azon belül az ajtótábla sorrendjében
// számoz; a connect_rooms után kell hívni; a látott térkép bejárt mezőit munkaterületnek használja, utána ezek üresek
void assign_passage_numbers(void);

void put_passage(uint8_t x, uint8_t y);         // látható folyosó-mező a labirintusnak és a folyosóknak; hogy titkos-e, azt a hide_passages dönti el

// mélyebben a szabad folyosó-mezők egy része titkos lesz (üresnek látszik); a kept_x, kept_y mező (a játékosé) mindig látható marad,
// a térképen kívüli pozícióval egyik sem; a lépcső és a játékos helye után, a többi elhelyezés előtt kell hívni
void hide_passages(uint8_t kept_x, uint8_t kept_y);

#endif
