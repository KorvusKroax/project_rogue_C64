#ifndef ITEMS_H
#define ITEMS_H

#include <stdbool.h>
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

// tárgyflagek (1.2.3)
#define ITEM_CURSED     0x01                    // átkozott: használatban nem vehető le, nem cserélhető (ISCURSED)
#define ITEM_KNOWN      0x02                    // azonosítva: a nevében látszik a bónusza, a töltése, a gyűrűmódja (ISKNOW)
#define ITEM_ROOM_GOLD  0x04                    // a szoba aranya: a szint berendezésekor lerakott arany, ezt őrzi a kapzsi szörny (ROOMGOLD)
#define ITEM_PROTECTED  0x08                    // a páncél véd a rozsdától (ISPROT)

// az étel altípusai
#define FOOD_RATION  0
#define FOOD_FRUIT   1

#define FOOD_SUBTYPE_COUNT  2                   // az étel altípusainak száma

// a fegyver- és a páncéltípusok száma: ennyi elemű az esélytáblájuk és a nevük listája (texts.c);
// a bájital, a tekercs, a gyűrű és a pálca altípusainak száma a state.h-ban van, mert a GameState tömbjei is ezzel méreteződnek
#define WEAPON_SUBTYPE_COUNT  9
#define ARMOR_SUBTYPE_COUNT   8

// a bájitalok altípusai: a bájitalok esélytáblájának (potion_chances) indexei
#define POTION_CONFUSION          0
#define POTION_HALLUCINATION      1
#define POTION_POISON             2
#define POTION_GAIN_STRENGTH      3
#define POTION_SEE_INVISIBLE      4
#define POTION_HEALING            5
#define POTION_MONSTER_DETECTION  6
#define POTION_MAGIC_DETECTION    7
#define POTION_RAISE_LEVEL        8
#define POTION_EXTRA_HEALING      9
#define POTION_HASTE_SELF         10
#define POTION_RESTORE_STRENGTH   11
#define POTION_BLINDNESS          12
#define POTION_LEVITATION         13

// a külön kezelt fegyvertípusok: a fegyverek esélytáblájának (weapon_chances) indexei; a többi neve az első használatánál jön
#define WEAPON_MACE       0
#define WEAPON_SHORT_BOW  2
#define WEAPON_ARROW      3
#define WEAPON_DAGGER     4
#define WEAPON_DART       6
#define WEAPON_SHURIKEN   7

// a külön kezelt páncéltípusok: a páncélok esélytáblájának (armor_chances) indexei
#define ARMOR_LEATHER    0
#define ARMOR_RING_MAIL  1

// a külön kezelt gyűrűtípusok: a gyűrűk esélytáblájának (ring_chances) indexei
#define RING_PROTECTION         0
#define RING_ADD_STRENGTH       1
#define RING_AGGRAVATE_MONSTER  6
#define RING_DEXTERITY          7
#define RING_INCREASE_DAMAGE    8
#define RING_TELEPORTATION      11

// a külön kezelt pálcatípus: a pálcák esélytáblájának (stick_chances) indexe
#define STICK_LIGHT  0

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

// a kezdőértékek (8.1–8.7); a darabszám 1, kivéve a kötegelt fegyvereket:
// a kötegelt lövedéké (nyíl, dart, shuriken) rnd(MISSILE_COUNT_RANGE) + MISSILE_COUNT_MIN, a tőré rnd(DAGGER_COUNT_RANGE) + DAGGER_COUNT_MIN
#define MISSILE_COUNT_MIN    8                  // default: 8
#define MISSILE_COUNT_RANGE  8                  // default: 8
#define DAGGER_COUNT_MIN     2                  // default: 2
#define DAGGER_COUNT_RANGE   4                  // default: 4

// a darabszám 8 bites, és legalább 1
#if MISSILE_COUNT_MIN < 1 || DAGGER_COUNT_MIN < 1 || MISSILE_COUNT_MIN + MISSILE_COUNT_RANGE - 1 > 0xff || DAGGER_COUNT_MIN + DAGGER_COUNT_RANGE - 1 > 0xff
    #error "a darabszám 1..255"
#endif

// a modos gyűrűk (protection, add strength, dexterity, increase damage) modja 1..RING_MODIFIER_MAX, vagy -1, és akkor átkozott
#define RING_MODIFIER_MAX  2                    // default: 2

// a pálca töltése: a light pálcáé rnd(LIGHT_CHARGES_RANGE) + LIGHT_CHARGES_MIN, a többié rnd(STICK_CHARGES_RANGE) + STICK_CHARGES_MIN
#define LIGHT_CHARGES_MIN    10                 // default: 10
#define LIGHT_CHARGES_RANGE  10                 // default: 10
#define STICK_CHARGES_MIN    3                  // default: 3
#define STICK_CHARGES_RANGE  5                  // default: 5

// a töltés 8 bites
#if LIGHT_CHARGES_MIN + LIGHT_CHARGES_RANGE - 1 > 0xff || STICK_CHARGES_MIN + STICK_CHARGES_RANGE - 1 > 0xff
    #error "a töltés 8 bites"
#endif

// csak véletlen generáláskor: a fegyver CURSED_WEAPON_CHANCE eséllyel átkozott, ENCHANTED_WEAPON_CHANCE eséllyel jobb (a CHANCE_TOTAL-ból),
// a páncél ugyanígy; az eltérés mértéke 1..RANDOM_MODIFIER_MAX
#define CURSED_WEAPON_CHANCE     10             // default: 10
#define ENCHANTED_WEAPON_CHANCE  5              // default: 5
#define CURSED_ARMOR_CHANCE      20             // default: 20
#define ENCHANTED_ARMOR_CHANCE   8              // default: 8
#define RANDOM_MODIFIER_MAX      3              // default: 3

#if CURSED_WEAPON_CHANCE + ENCHANTED_WEAPON_CHANCE > CHANCE_TOTAL || CURSED_ARMOR_CHANCE + ENCHANTED_ARMOR_CHANCE > CHANCE_TOTAL
    #error "az átok és a bónusz esélye együtt legfeljebb CHANCE_TOTAL"
#endif

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
void clear_item_table(void);                    // minden hely szabad, a padló és a csomag üres, a kéz is, és nincs viselt páncél: a játék elején
uint8_t allocate_item(void);                    // egy szabad hely címke nélkül, egyébként kitöltetlenül, a type beírásától foglalt; a game.item_count eggyel nő; NO_ITEM, ha betelt
void free_item(uint8_t index);                  // a hely és a tárgy címkéje újra szabad, a game.item_count eggyel csökken; a tárgy már egyik listában sincs

uint8_t get_item_character(uint8_t type);       // a tárgy jele a térképen
bool is_item_character(uint8_t character);      // a jel valamelyik tárgytípusé

bool is_many_weapon(uint8_t which);             // kötegelt lövedék (ISMANY): nyíl, dart, shuriken
bool has_ring_modifier(uint8_t which);          // modos gyűrű: protection, add strength, dexterity, increase damage
int8_t get_armor_base_class(uint8_t which);     // a páncéltípus védelme (8.5)

// mágikus tárgy (is_magic, 8.8): a bájital, a tekercs, a gyűrű, a pálca és az amulett mindig; a páncél, ha védett (ITEM_PROTECTED)
// vagy a védelme eltér az alapértékétől; a fegyver, ha van találati vagy sebzésbónusza; az étel és az arany soha
bool is_magic_item(const struct Item *item);

// új tárgy a megadott típussal és altípussal, a minden létrehozáskor járó kezdőértékekkel (8.1): darabszám, kötegazonosító,
// a páncél védelme, a gyűrű modja és átka, a pálca töltése; NO_ITEM, ha betelt a tábla
uint8_t create_item(uint8_t type, uint8_t which);

// csak véletlen generáláskor, a kezdőértékek után: a fegyver és a páncél átka vagy bónusza (8.4–8.5); a többi típusnál nem dob
void roll_curse_or_bonus(uint8_t index);

// új véletlen tárgy (new_item) a choose_item_type_and_subtype szerint, a kezdőértékekkel és az átok vagy bónusz dobásával;
// NO_ITEM, ha betelt a tábla: ekkor nincs dobás, és a számláló sem változik
uint8_t create_random_item(void);

// a mélység szerinti véletlen aranymennyiség (GOLDCALC): rnd(GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL × mélység) + GOLD_AMOUNT_MIN
uint16_t roll_gold_amount(void);

// az arany (create_item(ITEM_TYPE_GOLD, 0)) mélység szerinti véletlen mennyisége (roll_gold_amount);
// nem egy 8 bites visszatérésű create_gold része, mert annak az eredményét az Oscar64 1.32.273 a hívóban rossz helyről olvasta (README, Oscar64-hibák, 10.)
void set_random_gold_amount(uint8_t index);

// egyszeresen láncolt tárgylista (a padlón lévő tárgyak, a csomag, egy szörny csomagja): az eleje egy tárgyindex (NO_ITEM: üres),
// a következő tárgyé a next mező; a list_first a lista elejét tároló változó
void insert_item_after(uint8_t *list_first, uint8_t previous, uint8_t index);  // az index a previous után, NO_ITEM-nél a lista elejére kerül
void remove_item_from_list(uint8_t *list_first, uint8_t index);                // az index kikerül a listából; a listában kell lennie
void free_item_list(uint8_t *list_first);       // a lista minden tárgyának helye felszabadul, és a lista üres: új szintnél a padlóé és a szörnyek csomagjai

// a padlón lévő tárgyak listája, a legutóbb lerakottal kezdve;
// bejárás: for (uint8_t index = game.first_floor_item; index != NO_ITEM; index = get_item(index)->next)
void put_item_on_floor(uint8_t index, uint8_t x, uint8_t y);  // a pozíciója (x, y), a mezőn a jele, és a lista elejére kerül
uint8_t find_floor_item_at(uint8_t x, uint8_t y);  // a padlón az x, y mezőn fekvő tárgy (egy mezőn legfeljebb egy van); NO_ITEM, ha ott nincs
uint8_t find_room_gold(uint8_t room_index);     // a szobában (folyosórendszerben) fekvő, ITEM_ROOM_GOLD jelzőjű arany (szobánként legfeljebb egy); NO_ITEM, ha nincs
void take_item_from_floor(uint8_t index);       // a tárgy kikerül a listából, a mező jele folyosó-mezőn (TILE_PASSAGE) folyosó, különben padló; a felé tartó szörnyek célpontja a játékos lesz

#endif
