#ifndef ITEM_NAMES_H
#define ITEM_NAMES_H

#include <stdint.h>

#include "state.h"

// a típusok ismerete (game.potion_knowledge stb.): altípusonként egy bájt
#define SUBTYPE_UNKNOWN     0                   // a játékos nem ismeri: az álnevével látszik
#define SUBTYPE_IDENTIFIED  0xff                // azonosítva: a valódi nevével látszik

// a tekercsfelirat SCROLL_TITLE_WORDS_MIN + rnd(SCROLL_TITLE_WORDS_RANGE) szó, mindegyik SCROLL_WORD_SYLLABLES_MIN + rnd(SCROLL_WORD_SYLLABLES_RANGE)
// szótagból, legfeljebb SCROLL_TITLE_LENGTH_MAX karakter; az a szótag, amelyik már nem fér el, és minden utána következő, kimarad
#define SCROLL_TITLE_WORDS_MIN       2          // default: 2
#define SCROLL_TITLE_WORDS_RANGE     3          // default: 3
#define SCROLL_WORD_SYLLABLES_MIN    1          // default: 1
#define SCROLL_WORD_SYLLABLES_RANGE  3          // default: 3
#define SCROLL_TITLE_LENGTH_MAX      30         // default: 30
#define SCROLL_TITLE_SIZE            (SCROLL_TITLE_LENGTH_MAX + 1)

// a leghosszabb név a később jövő címkével és a használatban lévő tárgy utótagjával együtt:
// "a staff called " (15) + a 20 karakteres címke + " [255 charges]" (14) + "(persimmon wood)" (16) + " (weapon in hand)" (17)
#define ITEM_NAME_LENGTH_MAX  82
#define ITEM_NAME_SIZE        (ITEM_NAME_LENGTH_MAX + 1)

// új játékhoz: minden típus ismeretlen, és ismétlés nélkül új álnevet kap: a bájital színt, a gyűrű követ, a pálca anyagot;
// a tekercsfeliratok magja olyan, hogy a SCROLL_SUBTYPE_COUNT felirat mind különböző
void init_item_names(void);

// a tekercstípus felirata a game.scroll_title_seed-ből, nullával lezárva; out legalább SCROLL_TITLE_SIZE bájt; az RN-t nem hívja
void write_scroll_title(char *out, uint8_t which);

// a tárgy neve (8.8), kisbetűvel kezdve; az eredmény a következő hívásig él
const char *format_item_name(const struct Item *item);

#endif
