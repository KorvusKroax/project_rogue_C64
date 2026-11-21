#include "items.h"

#include "map.h"
#include "rng.h"

#ifdef DEVMODE
    #include "dev/dev.h"
#endif

#define TABLE_LENGTH(table)  (sizeof(table) / sizeof((table)[0]))

static const uint8_t item_type_chances[RANDOM_ITEM_TYPE_COUNT] = {
    26,     // bájital
    36,     // tekercs
    16,     // étel
     7,     // fegyver
     7,     // páncél
     4,     // gyűrű
     4,     // pálca
};

const struct ChanceTable item_type_table = { item_type_chances, RANDOM_ITEM_TYPE_COUNT };

static const uint8_t potion_chances[] = {
     7,     // confusion
     8,     // hallucination
     8,     // poison
    13,     // gain strength
     3,     // see invisible
    13,     // healing
     6,     // monster detection
     6,     // magic detection
     2,     // raise level
     5,     // extra healing
     5,     // haste self
    13,     // restore strength
     5,     // blindness
     6,     // levitation
};

static const uint8_t scroll_chances[] = {
     7,     // monster confusion
     4,     // magic mapping
     2,     // hold monster
     3,     // sleep
     7,     // enchant armor
    10,     // identify potion
    10,     // identify scroll
     6,     // identify weapon
     7,     // identify armor
    10,     // identify ring, wand or staff
     3,     // scare monster
     2,     // food detection
     5,     // teleportation
     8,     // enchant weapon
     4,     // create monster
     7,     // remove curse
     3,     // aggravate monsters
     2,     // protect armor
};

static const uint8_t weapon_chances[] = {
    11,     // mace
    11,     // long sword
    12,     // short bow
    12,     // arrow
     8,     // dagger
    10,     // two handed sword
    12,     // dart
    12,     // shuriken
    12,     // spear
};

static const uint8_t armor_chances[] = {
    20,     // leather armor
    15,     // ring mail
    15,     // studded leather
    13,     // scale mail
    12,     // chain mail
    10,     // splint mail
    10,     // banded mail
     5,     // plate mail
};

// a páncéltípusok védelme (8.5), az esélytábla sorrendjében; minél kisebb, annál jobb
static const int8_t armor_base_classes[TABLE_LENGTH(armor_chances)] = {
     8,     // leather armor
     7,     // ring mail
     7,     // studded leather
     6,     // scale mail
     5,     // chain mail
     4,     // splint mail
     4,     // banded mail
     3,     // plate mail
};

static const uint8_t ring_chances[] = {
     9,     // protection
     9,     // add strength
     5,     // sustain strength
    10,     // searching
    10,     // see invisible
     1,     // adornment
    10,     // aggravate monster
     8,     // dexterity
     8,     // increase damage
     4,     // regeneration
     9,     // slow digestion
     5,     // teleportation
     7,     // stealth
     5,     // maintain armor
};

static const uint8_t stick_chances[] = {
    12,     // light
     6,     // invisibility
     3,     // lightning
     3,     // fire
     3,     // cold
    15,     // polymorph
    10,     // magic missile
    10,     // haste monster
    11,     // slow monster
     9,     // drain life
     1,     // nothing
     6,     // teleport away
     6,     // teleport to
     5,     // cancellation
};

const struct ChanceTable item_subtype_tables[RANDOM_ITEM_TYPE_COUNT] = {
    { potion_chances, TABLE_LENGTH(potion_chances) },       // ITEM_TYPE_POTION
    { scroll_chances, TABLE_LENGTH(scroll_chances) },       // ITEM_TYPE_SCROLL
    { 0, 0 },                                               // ITEM_TYPE_FOOD: nincs táblája, adag vagy gyümölcs (choose_item_type_and_subtype)
    { weapon_chances, TABLE_LENGTH(weapon_chances) },       // ITEM_TYPE_WEAPON
    { armor_chances,  TABLE_LENGTH(armor_chances) },        // ITEM_TYPE_ARMOR
    { ring_chances,   TABLE_LENGTH(ring_chances) },         // ITEM_TYPE_RING
    { stick_chances,  TABLE_LENGTH(stick_chances) },        // ITEM_TYPE_STICK
};

uint8_t choose_by_chance(const struct ChanceTable *table, uint8_t roll)
{
    uint8_t index = 0;

    // az utolsó elemnél megáll, így az index mindig érvényes
    while (index < table->count - 1 && roll >= table->chances[index]) {
        roll -= table->chances[index];
        index++;
    }
    return index;
}

void choose_item_type_and_subtype(uint8_t *type, uint8_t *which)
{
    if (game.levels_without_food > LEVELS_WITHOUT_FOOD_MAX) {
        *type = ITEM_TYPE_FOOD;
    } else {
        uint8_t type_roll = rnd(CHANCE_TOTAL);
        *type = choose_by_chance(&item_type_table, type_roll);
    }

    if (*type == ITEM_TYPE_FOOD) {
        game.levels_without_food = 0;
        *which = rnd(FRUIT_CHANCE_DIVISOR) != 0 ? FOOD_RATION : FOOD_FRUIT;
    } else {
        uint8_t subtype_roll = rnd(CHANCE_TOTAL);
        *which = choose_by_chance(&item_subtype_tables[*type], subtype_roll);
    }
}

struct Item *get_item(uint8_t index)
{
    return &game.items[index - 1];
}

void clear_item_table(void)
{
    for (uint8_t slot = 0; slot < MAX_ITEMS; slot++) {
        game.items[slot].type = ITEM_TYPE_FREE;
    }
    game.item_count = 0;
    game.first_floor_item = NO_ITEM;
}

uint8_t allocate_item(void)
{
    // a tömb helyén számol, mert MAX_ITEMS = 255 mellett az index <= MAX_ITEMS feltétel 8 biten mindig igaz volna
    for (uint8_t slot = 0; slot < MAX_ITEMS; slot++) {
        if (game.items[slot].type == ITEM_TYPE_FREE) {
            game.item_count++;
            #ifdef DEVMODE
                update_item_count_peak();
            #endif
            return slot + 1;
        }
    }
    return NO_ITEM;
}

void free_item(uint8_t index)
{
    get_item(index)->type = ITEM_TYPE_FREE;
    game.item_count--;
}

static const uint8_t item_type_characters[ITEM_TYPE_COUNT] = {
    CHAR_POTION,
    CHAR_SCROLL,
    CHAR_FOOD,
    CHAR_WEAPON,
    CHAR_ARMOR,
    CHAR_RING,
    CHAR_STICK,
    CHAR_GOLD,
    CHAR_AMULET,
};

uint8_t get_item_character(uint8_t type)
{
    return item_type_characters[type];
}

bool is_many_weapon(uint8_t which)
{
    return which == WEAPON_ARROW || which == WEAPON_DART || which == WEAPON_SHURIKEN;
}

bool has_ring_modifier(uint8_t which)
{
    return which == RING_PROTECTION || which == RING_ADD_STRENGTH || which == RING_DEXTERITY || which == RING_INCREASE_DAMAGE;
}

int8_t get_armor_base_class(uint8_t which)
{
    return armor_base_classes[which];
}

// az utoljára kiadott kötegazonosító; nem kell menteni, mert csak az számít, hogy két köteg ne kapja ugyanazt
static uint8_t last_group = 0;

static bool is_group_used(uint8_t group)
{
    for (uint8_t slot = 0; slot < MAX_ITEMS; slot++) {
        if (game.items[slot].type != ITEM_TYPE_FREE && game.items[slot].group == group) return true;
    }
    return false;
}

// új kötegazonosító: az utoljára kiadott utáni első érték (a 0 kimarad), amelyet egyetlen meglévő tárgy sem használ;
// a ciklus véget ér, mert a többi tárgy legfeljebb MAX_ITEMS - 1 <= 254 értéket foglal (state.h), így az 1..255 közül mindig marad szabad
static uint8_t choose_unused_group(void)
{
    do {
        last_group++;
        if (last_group == 0) {
            last_group = 1;
        }
    } while (is_group_used(last_group));
    return last_group;
}

// a bónuszok 0; a kötegelt lövedék és a tőr darabszáma véletlen, és új kötegazonosítót kap (a tőr nem ISMANY, mégis kötegel)
static void set_initial_weapon_values(struct Item *item)
{
    item->properties.weapon.hit_bonus = 0;
    item->properties.weapon.damage_bonus = 0;

    if (is_many_weapon(item->which)) {
        item->count = rnd(MISSILE_COUNT_RANGE) + MISSILE_COUNT_MIN;
        item->group = choose_unused_group();
    } else if (item->which == WEAPON_DAGGER) {
        item->count = rnd(DAGGER_COUNT_RANGE) + DAGGER_COUNT_MIN;
        item->group = choose_unused_group();
    }
}

// a modos gyűrű modja 1..RING_MODIFIER_MAX, vagy -1, és akkor átkozott; az aggravate monster és a teleportation gyűrű mindig átkozott
static void set_initial_ring_values(struct Item *item)
{
    item->properties.ring_modifier = 0;

    if (has_ring_modifier(item->which)) {
        int8_t modifier = rnd(RING_MODIFIER_MAX + 1);
        if (modifier == 0) {
            modifier = -1;
            item->flags |= ITEM_CURSED;
        }
        item->properties.ring_modifier = modifier;
    } else if (item->which == RING_AGGRAVATE_MONSTER || item->which == RING_TELEPORTATION) {
        item->flags |= ITEM_CURSED;
    }
}

static void set_initial_charges(struct Item *item)
{
    if (item->which == STICK_LIGHT) {
        item->properties.charges = rnd(LIGHT_CHARGES_RANGE) + LIGHT_CHARGES_MIN;
    } else {
        item->properties.charges = rnd(STICK_CHARGES_RANGE) + STICK_CHARGES_MIN;
    }
}

// a minden létrehozáskor járó kezdőértékek (8.1–8.7); a type és a which már be van írva
static void set_initial_values(struct Item *item)
{
    item->count = 1;
    item->flags = 0;
    item->group = 0;

    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            set_initial_weapon_values(item);
            break;
        case ITEM_TYPE_ARMOR:
            item->properties.armor_class = armor_base_classes[item->which];
            break;
        case ITEM_TYPE_RING:
            set_initial_ring_values(item);
            break;
        case ITEM_TYPE_STICK:
            set_initial_charges(item);
            break;
    }
}

uint8_t create_item(uint8_t type, uint8_t which)
{
    uint8_t index = allocate_item();
    if (index == NO_ITEM) return NO_ITEM;

    struct Item *item = get_item(index);
    item->type = type;
    item->which = which;
    set_initial_values(item);
    return index;
}

void roll_curse_or_bonus(uint8_t index)
{
    struct Item *item = get_item(index);

    if (item->type == ITEM_TYPE_WEAPON) {
        uint8_t roll = rnd(CHANCE_TOTAL);
        if (roll < CURSED_WEAPON_CHANCE) {
            uint8_t penalty = rnd(RANDOM_MODIFIER_MAX) + 1;
            item->flags |= ITEM_CURSED;
            item->properties.weapon.hit_bonus -= penalty;
        } else if (roll < CURSED_WEAPON_CHANCE + ENCHANTED_WEAPON_CHANCE) {
            uint8_t bonus = rnd(RANDOM_MODIFIER_MAX) + 1;
            item->properties.weapon.hit_bonus += bonus;
        }
    } else if (item->type == ITEM_TYPE_ARMOR) {
        // a páncélnál a nagyobb védelem a rosszabb
        uint8_t roll = rnd(CHANCE_TOTAL);
        if (roll < CURSED_ARMOR_CHANCE) {
            uint8_t penalty = rnd(RANDOM_MODIFIER_MAX) + 1;
            item->flags |= ITEM_CURSED;
            item->properties.armor_class += penalty;
        } else if (roll < CURSED_ARMOR_CHANCE + ENCHANTED_ARMOR_CHANCE) {
            uint8_t bonus = rnd(RANDOM_MODIFIER_MAX) + 1;
            item->properties.armor_class -= bonus;
        }
    }
}

void set_random_gold_amount(uint8_t index)
{
    uint16_t amount = rnd(GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL * game.dungeon_level) + GOLD_AMOUNT_MIN;
    get_item(index)->properties.gold_amount = amount;
}

uint8_t create_random_item(void)
{
    uint8_t index = allocate_item();
    if (index == NO_ITEM) return NO_ITEM;

    struct Item *item = get_item(index);
    choose_item_type_and_subtype(&item->type, &item->which);
    set_initial_values(item);
    roll_curse_or_bonus(index);
    return index;
}

void put_item_on_floor(uint8_t index, uint8_t x, uint8_t y)
{
    struct Item *item = get_item(index);

    item->x = x;
    item->y = y;
    item->next = game.first_floor_item;
    game.first_floor_item = index;
    set_map_character(x, y, get_item_character(item->type));
}

void free_floor_items(void)
{
    while (game.first_floor_item != NO_ITEM) {
        uint8_t index = game.first_floor_item;
        game.first_floor_item = get_item(index)->next;
        free_item(index);
    }
}
