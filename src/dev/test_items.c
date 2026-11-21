#include "tests.h"

#include <stdbool.h>

#include "../items.h"
#include "../state.h"

// a 0..CHANCE_TOTAL-1 dobások közül minden elemet pontosan annyi választ, amennyi az esélye; így az esélyek összege is CHANCE_TOTAL
static bool is_chance_table_exact(const struct ChanceTable *table)
{
    if (table->count == 0) return false;

    for (uint8_t index = 0; index < table->count; index++) {
        uint8_t hits = 0;
        for (uint8_t roll = 0; roll < CHANCE_TOTAL; roll++) {
            if (choose_by_chance(table, roll) == index) {
                hits++;
            }
        }
        if (hits != table->chances[index]) return false;
    }
    return true;
}

int test_item_chances(uint16_t i)
{
    if (!is_chance_table_exact(&item_type_table)) return false;

    for (uint8_t type = 0; type < RANDOM_ITEM_TYPE_COUNT; type++) {
        if (type == ITEM_TYPE_FOOD) continue;
        if (!is_chance_table_exact(&item_subtype_tables[type])) return false;
    }
    return true;
}

#define FOOD_TEST_TRIALS  100                   // ennyi tárgy számlálónként, rögzített maggal

bool is_item_type_and_subtype_valid(uint8_t type, uint8_t which)
{
    if (type == ITEM_TYPE_FOOD) return which == FOOD_RATION || which == FOOD_FRUIT;
    return type < RANDOM_ITEM_TYPE_COUNT && which < item_subtype_tables[type].count;
}

int test_levels_without_food(uint16_t i)
{
    game.seed = 1;

    // a határ fölött mindig étel, és utána a számláló 0
    for (uint8_t trial = 0; trial < FOOD_TEST_TRIALS; trial++) {
        uint8_t type;
        uint8_t which;
        game.levels_without_food = LEVELS_WITHOUT_FOOD_MAX + 1;
        choose_item_type_and_subtype(&type, &which);
        if (type != ITEM_TYPE_FOOD || !is_item_type_and_subtype_valid(type, which)) return false;
        if (game.levels_without_food != 0) return false;
    }

    // a határon a táblák szerint: nem csak étel jön; étel után a számláló 0, más tárgy után nem változik
    bool other_seen = false;
    for (uint8_t trial = 0; trial < FOOD_TEST_TRIALS; trial++) {
        uint8_t type;
        uint8_t which;
        game.levels_without_food = LEVELS_WITHOUT_FOOD_MAX;
        choose_item_type_and_subtype(&type, &which);
        if (!is_item_type_and_subtype_valid(type, which)) return false;

        uint8_t expected_count = type == ITEM_TYPE_FOOD ? 0 : LEVELS_WITHOUT_FOOD_MAX;
        if (game.levels_without_food != expected_count) return false;
        if (type != ITEM_TYPE_FOOD) {
            other_seen = true;
        }
    }
    return other_seen;
}

#define FREED_ITEM_INDEX  (MAX_ITEMS / 2 + 1)   // a betelt táblából ez a hely szabadul fel

int test_item_table(uint16_t i)
{
    clear_item_table();

    // az üres táblában MAX_ITEMS hely foglalható, mindegyik érvényes indexű és szabad volt, utána nincs hely; a foglaltság közben számol
    if (game.item_count != 0) return false;
    for (uint16_t count = 0; count < MAX_ITEMS; count++) {
        uint8_t index = allocate_item();
        if (index == NO_ITEM || index > MAX_ITEMS) return false;
        if (get_item(index)->type != ITEM_TYPE_FREE) return false;
        get_item(index)->type = ITEM_TYPE_POTION;
        if (game.item_count != count + 1) return false;
    }
    if (allocate_item() != NO_ITEM || game.item_count != MAX_ITEMS) return false;

    // betelt táblánál a véletlen tárgy sem jön létre, és nem dob: a mag és az étel nélküli szintek számlálója nem változik
    game.seed = 1;
    game.levels_without_food = LEVELS_WITHOUT_FOOD_MAX + 1;
    if (create_random_item() != NO_ITEM) return false;
    if (game.seed != 1 || game.levels_without_food != LEVELS_WITHOUT_FOOD_MAX + 1) return false;

    // a felszabadított hely újra foglalható, utána ismét nincs hely
    free_item(FREED_ITEM_INDEX);
    if (game.item_count != MAX_ITEMS - 1) return false;
    if (allocate_item() != FREED_ITEM_INDEX) return false;
    get_item(FREED_ITEM_INDEX)->type = ITEM_TYPE_POTION;
    return allocate_item() == NO_ITEM && game.item_count == MAX_ITEMS;
}

// a darabszám és a kötegazonosító a fajta szerint (a kötegelt lövedéknek és a tőrnek van köteg); a találati bónusz átkozottnál
// -RANDOM_MODIFIER_MAX..-1, különben 0..RANDOM_MODIFIER_MAX; a sebzésbónusz 0
static bool are_weapon_initial_values_valid(const struct Item *item, bool cursed)
{
    uint8_t count_min = 1;
    uint8_t count_range = 1;
    if (is_many_weapon(item->which)) {
        count_min = MISSILE_COUNT_MIN;
        count_range = MISSILE_COUNT_RANGE;
    } else if (item->which == WEAPON_DAGGER) {
        count_min = DAGGER_COUNT_MIN;
        count_range = DAGGER_COUNT_RANGE;
    }
    if (item->count < count_min || item->count >= count_min + count_range) return false;

    bool grouped = is_many_weapon(item->which) || item->which == WEAPON_DAGGER;
    if ((item->group != 0) != grouped) return false;

    int8_t hit_bonus = item->properties.weapon.hit_bonus;
    if (item->properties.weapon.damage_bonus != 0) return false;
    if (cursed) return hit_bonus >= -RANDOM_MODIFIER_MAX && hit_bonus <= -1;
    return hit_bonus >= 0 && hit_bonus <= RANDOM_MODIFIER_MAX;
}

// átkozottnál az alapnál 1..RANDOM_MODIFIER_MAX-szal rosszabb (nagyobb), különben az alap, vagy legfeljebb ennyivel jobb
static bool is_armor_class_valid(const struct Item *item, bool cursed)
{
    int8_t base = get_armor_base_class(item->which);
    int8_t armor_class = item->properties.armor_class;
    if (cursed) return armor_class > base && armor_class <= base + RANDOM_MODIFIER_MAX;
    return armor_class >= base - RANDOM_MODIFIER_MAX && armor_class <= base;
}

// a modos gyűrű modja 1..RING_MODIFIER_MAX, vagy -1 és átkozott; a többi gyűrű modja 0, és csak az aggravate monster és a teleportation átkozott
static bool is_ring_modifier_valid(const struct Item *item, bool cursed)
{
    int8_t modifier = item->properties.ring_modifier;
    if (has_ring_modifier(item->which)) {
        if (cursed) return modifier == -1;
        return modifier >= 1 && modifier <= RING_MODIFIER_MAX;
    }
    bool always_cursed = item->which == RING_AGGRAVATE_MONSTER || item->which == RING_TELEPORTATION;
    return modifier == 0 && cursed == always_cursed;
}

static bool are_charges_valid(const struct Item *item)
{
    uint8_t charges = item->properties.charges;
    if (item->which == STICK_LIGHT) return charges >= LIGHT_CHARGES_MIN && charges < LIGHT_CHARGES_MIN + LIGHT_CHARGES_RANGE;
    return charges >= STICK_CHARGES_MIN && charges < STICK_CHARGES_MIN + STICK_CHARGES_RANGE;
}

bool are_item_initial_values_valid(const struct Item *item)
{
    if (item->flags & ~ITEM_CURSED) return false;
    bool cursed = (item->flags & ITEM_CURSED) != 0;

    if (item->type == ITEM_TYPE_WEAPON) return are_weapon_initial_values_valid(item, cursed);
    if (item->count != 1 || item->group != 0) return false;
    if (item->type == ITEM_TYPE_ARMOR) return is_armor_class_valid(item, cursed);
    if (item->type == ITEM_TYPE_RING) return is_ring_modifier_valid(item, cursed);
    if (item->type == ITEM_TYPE_STICK) return !cursed && are_charges_valid(item);
    return !cursed;
}

// a tárgytábla minden foglalt tárgyának kötegazonosítója 0, vagy egyetlen másikéval sem egyezik
static bool are_groups_unique(void)
{
    for (uint8_t slot = 0; slot < MAX_ITEMS; slot++) {
        const struct Item *item = &game.items[slot];
        if (item->type == ITEM_TYPE_FREE || item->group == 0) continue;

        for (uint8_t other = slot + 1; other < MAX_ITEMS; other++) {
            if (game.items[other].type != ITEM_TYPE_FREE && game.items[other].group == item->group) return false;
        }
    }
    return true;
}

uint8_t get_tested_subtype_count(uint8_t type)
{
    if (type == ITEM_TYPE_FOOD) return FOOD_SUBTYPE_COUNT;
    if (type < RANDOM_ITEM_TYPE_COUNT) return item_subtype_tables[type].count;
    return 1;
}

#define CURSE_TEST_TRIALS           200         // ennyi fegyveren és ennyi páncélon dob a véletlen rész, rögzített maggal
#define GENERATED_ITEM_TEST_ROUNDS  5           // ennyiszer telik meg a tárgytábla véletlen tárgyakkal

int test_item_initial_values(uint16_t i)
{
    clear_item_table();
    game.seed = 1;
    game.levels_without_food = 0;

    // a minden létrehozáskor járó rész: minden típus minden altípusa érvényes, és a fegyver meg a páncél átok és bónusz nélküli
    for (uint8_t type = 0; type < ITEM_TYPE_COUNT; type++) {
        uint8_t subtype_count = get_tested_subtype_count(type);
        for (uint8_t which = 0; which < subtype_count; which++) {
            uint8_t index = create_item(type, which);
            if (index == NO_ITEM) return false;

            const struct Item *item = get_item(index);
            if (!are_item_initial_values_valid(item)) return false;
            if (type == ITEM_TYPE_WEAPON || type == ITEM_TYPE_ARMOR) {
                if (item->flags & ITEM_CURSED) return false;
            }
            if (type == ITEM_TYPE_WEAPON && item->properties.weapon.hit_bonus != 0) return false;
            if (type == ITEM_TYPE_ARMOR && item->properties.armor_class != get_armor_base_class(which)) return false;
            free_item(index);
        }
    }

    // a véletlen rész a fegyveren és a páncélon: érvényes, és mindhárom kimenet előfordul (átkozott, jobb, változatlan)
    static const uint8_t modified_types[] = { ITEM_TYPE_WEAPON, ITEM_TYPE_ARMOR };
    for (uint8_t type_index = 0; type_index < sizeof(modified_types); type_index++) {
        uint8_t type = modified_types[type_index];
        uint8_t subtype_count = item_subtype_tables[type].count;
        bool cursed_seen = false;
        bool enchanted_seen = false;
        bool unchanged_seen = false;

        for (uint8_t trial = 0; trial < CURSE_TEST_TRIALS; trial++) {
            uint8_t index = create_item(type, trial % subtype_count);
            if (index == NO_ITEM) return false;
            roll_curse_or_bonus(index);

            const struct Item *item = get_item(index);
            if (!are_item_initial_values_valid(item)) return false;

            bool enchanted = type == ITEM_TYPE_WEAPON ? item->properties.weapon.hit_bonus > 0 : item->properties.armor_class < get_armor_base_class(item->which);
            if (item->flags & ITEM_CURSED) {
                cursed_seen = true;
            } else if (enchanted) {
                enchanted_seen = true;
            } else {
                unchanged_seen = true;
            }
            free_item(index);
        }
        if (!cursed_seen || !enchanted_seen || !unchanged_seen) return false;
    }

    // a véletlen tárgyak: a tárgytáblát többször megtöltve mind érvényes, és a kötegazonosítók különbözők
    for (uint8_t round = 0; round < GENERATED_ITEM_TEST_ROUNDS; round++) {
        clear_item_table();
        for (uint8_t count = 0; count < MAX_ITEMS; count++) {
            if (create_random_item() == NO_ITEM) return false;
        }
        for (uint8_t slot = 0; slot < MAX_ITEMS; slot++) {
            if (!are_item_initial_values_valid(&game.items[slot])) return false;
        }
        if (!are_groups_unique()) return false;
    }

    clear_item_table();
    return true;
}

int test_item_groups(uint16_t i)
{
    clear_item_table();
    game.seed = 1;

    // a tábla megtelik nyíl- és tőrkötegekkel: mindegyiknek van kötegazonosítója, és mind különböző
    for (uint8_t count = 0; count < MAX_ITEMS; count++) {
        uint8_t which = count % 2 == 0 ? WEAPON_ARROW : WEAPON_DAGGER;
        uint8_t index = create_item(ITEM_TYPE_WEAPON, which);
        if (index == NO_ITEM || get_item(index)->group == 0) return false;
    }
    if (!are_groups_unique()) return false;

    // egy köteg helyére új kerül: az azonosítója sem ütközik a többivel
    free_item(FREED_ITEM_INDEX);
    uint8_t index = create_item(ITEM_TYPE_WEAPON, WEAPON_DART);
    if (index == NO_ITEM || get_item(index)->group == 0) return false;
    bool unique = are_groups_unique();

    clear_item_table();
    return unique;
}
