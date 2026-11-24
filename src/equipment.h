#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <stdbool.h>
#include <stdint.h>

// a kézben tartott tárgy (game.wielded_item) és a viselt páncél (game.worn_armor): mindkettő a csomag tárgya

// `w` előtt: ha a kézben tartott tárgy átkozott, „you can't. It appears to be cursed”, és nem cserélhető; igaz, ha átkozott
bool report_cursed_wielded_item(void);

// `w`: a csomag tárgya a kézbe kerül, bármi, ami nem páncél („you are now wielding <név> (<betű>)”); páncélra „you can't wield armor”,
// használatban lévő tárgyra „That's already in use”; igaz, ha időt vitt
bool wield_item(uint8_t index);

// `W`: a csomag páncélja egy üres kör (waste_time) után felkerül, és azonosítódik („you are now wearing <név>”); ha már van rajta
// páncél, „you are already wearing some. You'll have to take it off first”, nem páncélra „you can't wear that”; ha a játékos
// az üres körben meghal, a parancs ott véget ér; igaz, ha időt vitt
bool wear_armor(uint8_t index);

// a használatban lévő tárgy levétele, mielőtt a tárgy elhagyja a csomagot (`d`): az átkozott marad („you can't. It appears
// to be cursed”), a kézben tartott tárgynál a kéz kiürül, a viselt páncél egy üres kör (waste_time) után kerül le; a használaton kívüli
// tárgyhoz nem nyúl; igaz, ha a tárgy már nincs használatban, hamis, ha átkozott, vagy ha a játékos az üres körben meghalt (a parancs
// ilyenkor véget ér, de időt vitt)
bool release_item(uint8_t index);

// `T`: a viselt páncél egy üres kör után lekerül („you used to be wearing <betű>) <név>”); ha nincs rajta páncél, „you aren't wearing
// any armor”, átkozottra „you can't. It appears to be cursed”; ha a játékos az üres körben meghal, a parancs ott véget ér;
// igaz, ha időt vitt
bool take_off_armor(void);

#endif
