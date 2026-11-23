#ifndef DISCOVERIES_H
#define DISCOVERIES_H

#include <stdbool.h>
#include <stdint.h>

// `D` (4.2.6): „for what type of object do you want a list? (* for all)”, utána a típus vagy a négy típus (`*`) listája (text_list.h);
// más billentyűre „please type one of !?=/ (ESCAPE to quit)” és újra a kérdés, ESC-re semmi; nem visz időt
void show_discoveries(void);

// a lista tételei, mint a show_discoveries-nél: a type_character a típus jele (! ? = /), vagy '*' a négy típushoz, a listák között
// üres tétellel; egy típus ismert és elnevezett altípusai a tárgynév alakjában (8.8), minden hívásnál véletlen sorrendben, ha nincs
// ilyen, „Haven't discovered anything about any potions”; a sorrendet saját generátor keveri, az RN-t nem hívja; hamis, ha
// a type_character egyik sem
bool prepare_discovery_list(uint8_t type_character);
const char *get_discovery_entry(uint8_t index);     // a prepare_discovery_list index. tétele, NULL a végén (TextListEntryFunction)

#endif
