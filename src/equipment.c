#include "equipment.h"

#include <stddef.h>

#include "game.h"
#include "item_names.h"
#include "items.h"
#include "message.h"
#include "player.h"
#include "state.h"
#include "texts.h"

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
