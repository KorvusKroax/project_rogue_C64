#ifndef ITEMS_H
#define ITEMS_H

#include <stdint.h>

#include "state.h"

// a véletlenül generált tárgyak típusai; a sorrend a típusok esélytábláé (item_type_table)
#define ITEM_TYPE_POTION  0
#define ITEM_TYPE_SCROLL  1
#define ITEM_TYPE_FOOD    2
#define ITEM_TYPE_WEAPON  3
#define ITEM_TYPE_ARMOR   4
#define ITEM_TYPE_RING    5
#define ITEM_TYPE_STICK   6

#define RANDOM_ITEM_TYPE_COUNT  7               // a véletlenül generált típusok száma

// a szint berendezésekor külön lerakott típusok, altípus nélkül (which = 0)
#define ITEM_TYPE_GOLD    7
#define ITEM_TYPE_AMULET  8

#define ITEM_TYPE_COUNT  9                      // az összes típus száma

#define ITEM_TYPE_FREE  0xff                    // a tárgytábla szabad helye

#define NO_ITEM  0                              // tárgyindex helyett: nincs tárgy, a lista vége

// az étel altípusai
#define FOOD_RATION  0
#define FOOD_FRUIT   1

#define FRUIT_CHANCE_DIVISOR  10                // az étel 1 / FRUIT_CHANCE_DIVISOR eséllyel gyümölcs, különben adag (default: 10)

#define LEVELS_WITHOUT_FOOD_MAX  3              // ha ennyinél több szint óta nem jött étel, a következő tárgy étel (default: 3)

// az arany mennyisége: rnd(GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL × mélység) + GOLD_AMOUNT_MIN
#define GOLD_AMOUNT_BASE       50               // default: 50
#define GOLD_AMOUNT_PER_LEVEL  10               // default: 10
#define GOLD_AMOUNT_MIN        2                // default: 2

// a mennyiség 16 bites, a dobás felső határa is
#if GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL * DUNGEON_LEVEL_MAX + GOLD_AMOUNT_MIN - 1 > 0xffff
    #error "az arany mennyisége 16 bites"
#endif

#define CHANCE_TOTAL  100                       // az esélyek összege minden esélytáblában; a dobás 0..CHANCE_TOTAL-1

// esélytábla: az index. elemet a dobások közül chances[index] választja
struct ChanceTable {
    const uint8_t *chances;
    uint8_t count;
};

extern const struct ChanceTable item_type_table;                                // a típusok (ITEM_TYPE_)
extern const struct ChanceTable item_subtype_tables[RANDOM_ITEM_TYPE_COUNT];    // típusonként az altípusok, az ITEM_TYPE_ index szerint; az ételnek nincs (FRUIT_CHANCE_DIVISOR)

// a dobás (0..CHANCE_TOTAL-1) szerinti elem indexe: az esélyeket sorban levonja, amíg a dobás nagyobb vagy egyenlő
uint8_t choose_by_chance(const struct ChanceTable *table, uint8_t roll);

// egy új tárgy típusa és altípusa az esélytáblák szerint; ha LEVELS_WITHOUT_FOOD_MAX-nál több szint óta nem jött étel, étel;
// étel után a számláló 0
void choose_item_type_and_subtype(uint8_t *type, uint8_t *which);

// a tárgytábla: a tárgyakra 1..MAX_ITEMS index hivatkozik (state.h)
struct Item *get_item(uint8_t index);           // index: 1..MAX_ITEMS
void clear_item_table(void);                    // minden hely szabad, a padló üres: a játék elején
uint8_t allocate_item(void);                    // egy szabad hely kitöltetlenül, a type beírásától foglalt; NO_ITEM, ha betelt
void free_item(uint8_t index);                  // a hely újra szabad; a tárgy már egyik listában sincs

uint8_t get_item_character(uint8_t type);       // a tárgy jele a térképen

// új tárgy a megadott típussal és altípussal; NO_ITEM, ha betelt a tábla
uint8_t create_item(uint8_t type, uint8_t which);

// új véletlen tárgy (new_item) a choose_item_type_and_subtype szerint; NO_ITEM, ha betelt a tábla: ekkor nincs dobás, és a számláló sem változik
uint8_t create_random_item(void);

// új arany a mélység szerinti véletlen mennyiséggel; NO_ITEM, ha betelt a tábla: ekkor nincs dobás
uint8_t create_gold(void);

// a padlón lévő tárgyak listája, a legutóbb lerakottal kezdve;
// bejárás: for (uint8_t index = game.first_floor_item; index != NO_ITEM; index = get_item(index)->next)
void put_item_on_floor(uint8_t index, uint8_t x, uint8_t y);  // a pozíciója (x, y), a mezőn a jele, és a lista elejére kerül
void free_floor_items(void);                    // új szintnél: a padlón lévő tárgyak helye felszabadul, a lista üres

#endif
