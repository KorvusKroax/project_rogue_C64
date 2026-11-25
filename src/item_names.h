#ifndef ITEM_NAMES_H
#define ITEM_NAMES_H

#include <stdbool.h>
#include <stdint.h>

#include "state.h"

// a típusok ismerete (game.potion_knowledge stb.): altípusonként egy bájt; a köztes 1..MAX_NAMES érték az ismeretlen típus neve
// a névkészletben, amelyet a játékos adott neki (`c`)
#define SUBTYPE_UNKNOWN     0                   // a játékos nem ismeri: az álnevével látszik
#define SUBTYPE_IDENTIFIED  0xff                // azonosítva: a valódi nevével látszik

#define NO_NAME  0                              // névindex helyett: nincs név (a címke helyén, a típusismeretben a SUBTYPE_UNKNOWN)

// a típus ismeretének tömbje altípusonként (game.potion_knowledge stb.) bájitalnál, tekercsnél, gyűrűnél és pálcánál; NULL a többinél
uint8_t *get_type_knowledge(uint8_t type);

// a típusismeret bájtja a név indexét is tárolja: a név nélküli ismeretlen típusé a „nincs név”, és a névindex nem éri el az azonosítottét
#if SUBTYPE_UNKNOWN != NO_NAME || MAX_NAMES >= SUBTYPE_IDENTIFIED
    #error "a névindex nem fér el a típusismeret mellett"
#endif

// a tekercsfelirat SCROLL_TITLE_WORDS_MIN + rnd(SCROLL_TITLE_WORDS_RANGE) szó, mindegyik SCROLL_WORD_SYLLABLES_MIN + rnd(SCROLL_WORD_SYLLABLES_RANGE)
// szótagból, legfeljebb SCROLL_TITLE_LENGTH_MAX karakter; az a szótag, amelyik már nem fér el, és minden utána következő, kimarad
#define SCROLL_TITLE_WORDS_MIN       2          // default: 2
#define SCROLL_TITLE_WORDS_RANGE     3          // default: 3
#define SCROLL_WORD_SYLLABLES_MIN    1          // default: 1
#define SCROLL_WORD_SYLLABLES_RANGE  3          // default: 3
#define SCROLL_TITLE_LENGTH_MAX      30         // default: 30
#define SCROLL_TITLE_SIZE            (SCROLL_TITLE_LENGTH_MAX + 1)

// a leghosszabb név az elnevezett típussal és a használatban lévő tárgy utótagjával együtt, a kézben tartott boté:
// "a staff called " (15) + a 20 karakteres név + " [255 charges]" (14) + "(persimmon wood)" (16) + " (weapon in hand)" (17)
#define ITEM_NAME_LENGTH_MAX  82
#define ITEM_NAME_SIZE        (ITEM_NAME_LENGTH_MAX + 1)

// új játékhoz: minden típus ismeretlen, és ismétlés nélkül új álnevet kap: a bájital színt, a gyűrű követ, a pálca anyagot;
// a tekercsfeliratok magja olyan, hogy a SCROLL_SUBTYPE_COUNT felirat mind különböző; a névkészlet üres
void init_item_names(void);

// a névkészlet (game.name_pool): a játékos adta nevek, a típusnév és a tárgy címkéje közös táblája; a nevekre 1..MAX_NAMES index hivatkozik
bool is_name_pool_full(void);
uint8_t store_name(const char *text);           // a nem üres, legfeljebb NAME_LENGTH_MAX karakteres szöveg egy szabad helyre; az indexe, NO_NAME, ha betelt
void set_name(uint8_t name_index, const char *text);   // a foglalt hely szövege lecserélődik: átnevezés, új helyet nem foglal
const char *get_name(uint8_t name_index);       // a foglalt hely szövege
void free_name(uint8_t name_index);             // a hely újra szabad; NO_NAME-re nem történik semmi

bool can_be_named(uint8_t type);                // a `c` mindent elnevez, kivéve az ételt és az amulettet (J26)

// `c` (4.2.6): a csomag tárgyának elnevezése; bájitalnál, tekercsnél, gyűrűnél és pálcánál a típusa kap nevet (azonosított típusnál
// „that has already been identified”), fegyvernél és páncélnál a tárgy saját címkét; ételre és az amulettre „you can't call that anything”;
// ha már van neve, „was called "<név>"”; a beviteli mező üres, üres beírásra a név törlődik („you remove the name”), ESC-re a régi
// marad; ha a névkészlet betelt, és még nincs neve,
// a kérdés helyett „you have no room for more names. Remove a name from something first”
void name_item(uint8_t index);

// a tekercstípus felirata a game.scroll_title_seed-ből, nullával lezárva; out legalább SCROLL_TITLE_SIZE bájt; az RN-t nem hívja
void write_scroll_title(char *out, uint8_t which);

// a tárgy neve (8.8), kisbetűvel kezdve; a kézben tartott tárgyé és a viselt páncélé után az utótag: " (weapon in hand)",
// " (being worn)", ezért ezek nevét a tárgytábla rekordjával kell kérni; az eredmény a következő hívásig él
const char *format_item_name(const struct Item *item);

#endif
