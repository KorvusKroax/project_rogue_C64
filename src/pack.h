#ifndef PACK_H
#define PACK_H

#include <stdbool.h>
#include <stdint.h>

// a csomag helyei darabszám szerint: a kötegelt fegyver (nyíl, dart, shuriken, tőr) kötege egy hely, minden más tárgy darabonként egy
#define MAX_PACK  23                            // default: 23

#define FIRST_PACK_LETTER  'a'

// minden csomagbeli tárgy legalább egy helyet foglal és egy betűt kap, így a betű mindig elég
#if MAX_PACK > 'z' - FIRST_PACK_LETTER + 1
    #error "a csomagnak több helye van, mint betűje"
#endif

// a csomag a game.first_pack_item kezdetű tárgylista (items.h), típus szerint rendezve;
// bejárás: for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next)

uint8_t calculate_pack_slots_used(void);        // a csomag foglalt helyeinek száma

// a tárgy befér-e a csomagba: a meglévő kötegéhez csatolt kötegelt fegyver nem foglal új helyet
bool has_pack_room_for(uint8_t index);

// a tárgy a csomagba kerül (8.9), ha befér (has_pack_room_for), és egyik listában sincs:
// - az azonos altípusú bájital, tekercs, étel, illetve az azonos altípusú és kötegazonosítójú fegyver kötegéhez csatolódik,
//   és a rekordja felszabadul;
// - különben a legkisebb szabad betűt kapja, és az azonos altípusúak után kerül, ha nincs ilyen, az azonos típusúak után,
//   ha az sincs, a csomag végére
// az eredmény a csomag tárgya, amelyben most van: a köteg, vagy maga a tárgy
uint8_t add_item_to_pack(uint8_t index);

// a padlón fekvő tárgy felvétele: az arany a pénzhez adódik („you found <n> gold pieces”), más tárgy a csomagba kerül
// („you now have <név> (<betű>)”), és a felvett tárgy lekerül a padlóról; ha nem fér a csomagba, a padlón marad
// („there's no room in your pack”, utána „you moved onto <név>”)
void pick_up_item(uint8_t index);

// az `m` parancs üzenete a padlón fekvő tárgyra lépve: „you moved onto <név>”
void report_moving_onto_item(uint8_t index);

#endif
