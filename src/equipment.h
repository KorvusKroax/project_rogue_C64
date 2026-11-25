#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <stdbool.h>
#include <stdint.h>

// a kézben tartott tárgy (game.wielded_item) és a viselt páncél (game.worn_armor): mindkettő a csomag tárgya

// a kezdőfelszerelés (5.1)
#define STARTING_ARMOR_ENCHANTMENT  1           // a gyűrűs ing védelme ennyivel jobb az alapértékénél
#define STARTING_MACE_HIT_BONUS     1
#define STARTING_MACE_DAMAGE_BONUS  1
#define STARTING_BOW_HIT_BONUS      1
#define STARTING_ARROW_COUNT_MIN    25
#define STARTING_ARROW_COUNT_RANGE  15          // 25..39

// új játéknál, az üres tárgytáblába: a kezdő csomag (5.1) a betűk sorrendjében: étel, gyűrűs ing (viselve), buzogány (kézben),
// rövid íj, nyílköteg; a páncél és a fegyverek azonosítva
void create_starting_equipment(void);

// a tárgy használatban van: a kézben tartott tárgy vagy a viselt páncél
bool is_item_in_use(uint8_t index);

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

// `)`: „you are wielding (<betű>) <név>”, a név utótag nélkül; üres kéznél „you are wielding nothing”
void report_wielded_item(void);

// `]`: „you are wearing (<betű>) <név>”, a név utótag nélkül; páncél nélkül „you are wearing nothing”
void report_worn_armor(void);

#endif
