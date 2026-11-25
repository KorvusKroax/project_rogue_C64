#include "equipment.h"

#include <stddef.h>

#include "game.h"
#include "item_names.h"
#include "items.h"
#include "message.h"
#include "pack.h"
#include "player.h"
#include "rng.h"
#include "state.h"
#include "texts.h"

// a kezdőfelszerelés fegyvere: a bónuszai, és azonosítva
static void set_starting_weapon(uint8_t index, int8_t hit_bonus, int8_t damage_bonus)
{
    struct Item *item = get_item(index);
    item->properties.weapon.hit_bonus = hit_bonus;
    item->properties.weapon.damage_bonus = damage_bonus;
    item->flags |= ITEM_KNOWN;
}

void create_starting_equipment(void)
{
    // a tárgytábla üres, így minden tárgynak van helye; a csomag a típusok sorrendjében épül, így a betűk is ebben a sorrendben jönnek
    add_item_to_pack(create_item(ITEM_TYPE_FOOD, FOOD_RATION));

    uint8_t armor = create_item(ITEM_TYPE_ARMOR, ARMOR_RING_MAIL);
    struct Item *armor_item = get_item(armor);
    armor_item->properties.armor_class -= STARTING_ARMOR_ENCHANTMENT;
    armor_item->flags |= ITEM_KNOWN;
    add_item_to_pack(armor);
    game.worn_armor = armor;

    uint8_t mace = create_item(ITEM_TYPE_WEAPON, WEAPON_MACE);
    set_starting_weapon(mace, STARTING_MACE_HIT_BONUS, STARTING_MACE_DAMAGE_BONUS);
    add_item_to_pack(mace);
    game.wielded_item = mace;

    uint8_t bow = create_item(ITEM_TYPE_WEAPON, WEAPON_SHORT_BOW);
    set_starting_weapon(bow, STARTING_BOW_HIT_BONUS, 0);
    add_item_to_pack(bow);

    // a nyílköteg a létrehozáskor kap egy véletlen darabszámot, mint a szinten talált; a kezdő köteg ezt felülírja
    uint8_t arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    set_starting_weapon(arrows, 0, 0);
    get_item(arrows)->count = rnd(STARTING_ARROW_COUNT_RANGE) + STARTING_ARROW_COUNT_MIN;
    add_item_to_pack(arrows);
}

// a tárgy a kézben van, vagy ez a viselt páncél; ide jön: a gyűrűk
static bool is_item_in_use(uint8_t index)
{
    return index == game.wielded_item || index == game.worn_armor;
}

// ha a tárgy átkozott, „you can't. It appears to be cursed”; igaz, ha átkozott
static bool report_cursed_item(uint8_t index)
{
    if (!(get_item(index)->flags & ITEM_CURSED)) return false;
    message(text_appears_cursed);
    return true;
}

bool report_cursed_wielded_item(void)
{
    if (game.wielded_item == NO_ITEM) return false;
    return report_cursed_item(game.wielded_item);
}

bool wield_item(uint8_t index)
{
    const struct Item *item = get_item(index);
    if (item->type == ITEM_TYPE_ARMOR) {
        message(text_cant_wield_armor);
        return false;
    }
    if (is_item_in_use(index)) {
        message(text_already_in_use);
        return false;
    }

    // a név a kézbe vétel előtt készül, így nincs utótagja (8.8)
    char letter_text[2] = { item->location.pack_letter, 0 };
    message_format(text_now_wielding, format_item_name(item), letter_text, 0);
    game.wielded_item = index;
    return true;
}

bool wear_armor(uint8_t index)
{
    if (game.worn_armor != NO_ITEM) {
        message(text_already_wearing);
        return false;
    }
    struct Item *item = get_item(index);
    if (item->type != ITEM_TYPE_ARMOR) {
        message(text_cant_wear_that);
        return true;
    }

    // az üres körben még a páncél nélküli állapot számít
    waste_time();
    if (is_player_dead()) return true;

    item->flags |= ITEM_KNOWN;
    // a név a felvétel előtt készül, így nincs utótagja (8.8)
    message_format(text_now_wearing, format_item_name(item), NULL, 0);
    game.worn_armor = index;
    return true;
}

bool release_item(uint8_t index)
{
    if (!is_item_in_use(index)) return true;
    if (report_cursed_item(index)) return false;

    if (index == game.wielded_item) {
        game.wielded_item = NO_ITEM;
        return true;
    }
    // ide jön: a gyűrű, a levétel hatásával (mint az `R`-nél)

    // a viselt páncél: az üres körben még véd
    waste_time();
    if (is_player_dead()) return false;
    game.worn_armor = NO_ITEM;
    return true;
}

bool take_off_armor(void)
{
    uint8_t index = game.worn_armor;
    if (index == NO_ITEM) {
        message(text_not_wearing_armor);
        return false;
    }
    if (!release_item(index)) return true;

    // a név a levétel után készül, így nincs utótagja (8.8)
    const struct Item *item = get_item(index);
    char letter_text[2] = { item->location.pack_letter, 0 };
    message_format(text_used_to_be_wearing, letter_text, format_item_name(item), 0);
    return true;
}

// a használatban lévő tárgy betűje és neve, utótag nélkül (item_text), vagy ha nincs ilyen, a nothing_text (current)
static void report_item_in_use(uint8_t index, const char *item_text, const char *nothing_text)
{
    if (index == NO_ITEM) {
        message(nothing_text);
        return;
    }
    const struct Item *item = get_item(index);
    char letter_text[2] = { item->location.pack_letter, 0 };
    message_format(item_text, letter_text, format_item_name_without_suffix(item), 0);
}

void report_wielded_item(void)
{
    report_item_in_use(game.wielded_item, text_wielding_item, text_wielding_nothing);
}

void report_worn_armor(void)
{
    report_item_in_use(game.worn_armor, text_wearing_item, text_wearing_nothing);
}
