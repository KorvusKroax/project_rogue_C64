#include "pack.h"

#include <stddef.h>

#include "item_names.h"
#include "items.h"
#include "message.h"
#include "state.h"
#include "texts.h"

// az arany mennyisége az üzenetben előjeles 16 bites szám (message_format)
#if GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL * DUNGEON_LEVEL_MAX + GOLD_AMOUNT_MIN - 1 > 0x7fff
    #error "az arany mennyisége nem fér az üzenet számába"
#endif

// a kötegelt fegyver kötege egy hely, minden más tárgy darabonként egy
static uint8_t get_item_slot_count(const struct Item *item)
{
    return item->group != 0 ? 1 : item->count;
}

uint8_t calculate_pack_slots_used(void)
{
    uint8_t slots = 0;
    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        slots += get_item_slot_count(get_item(index));
    }
    return slots;
}

static bool is_stackable_type(uint8_t type)
{
    return type == ITEM_TYPE_POTION || type == ITEM_TYPE_SCROLL || type == ITEM_TYPE_FOOD;
}

// a csomag tárgya, amelynek kötegéhez a tárgy csatolódik: azonos altípusú bájital, tekercs, étel,
// vagy azonos altípusú és kötegazonosítójú fegyver; NO_ITEM, ha nincs ilyen
static uint8_t find_pack_stack_for(const struct Item *item)
{
    bool is_stackable = is_stackable_type(item->type);
    if (!is_stackable && item->group == 0) return NO_ITEM;

    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        const struct Item *pack_item = get_item(index);
        if (pack_item->type != item->type || pack_item->which != item->which) continue;
        if (is_stackable || pack_item->group == item->group) return index;
    }
    return NO_ITEM;
}

bool has_pack_room_for(uint8_t index)
{
    const struct Item *item = get_item(index);

    uint8_t needed_slots = get_item_slot_count(item);
    if (item->group != 0 && find_pack_stack_for(item) != NO_ITEM) {
        needed_slots = 0;
    }
    return calculate_pack_slots_used() + needed_slots <= MAX_PACK;
}

static bool is_pack_letter_used(uint8_t letter)
{
    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        if (get_item(index)->location.pack_letter == letter) return true;
    }
    return false;
}

// a legkisebb betű, amelyet egyik csomagbeli tárgy sem visel; a ciklus a 'z' előtt megáll, mert a tárgy csak akkor kerül a csomagba,
// ha befér, és a csomag MAX_PACK helyén legfeljebb ennyi tárgy van, ez pedig nem több a betűknél (pack.h)
static uint8_t choose_free_pack_letter(void)
{
    uint8_t letter = FIRST_PACK_LETTER;
    while (is_pack_letter_used(letter)) {
        letter++;
    }
    return letter;
}

// a csomag tárgya, amely után az új tárgy kerül: az utolsó azonos altípusú, ha nincs ilyen, az utolsó azonos típusú,
// ha az sincs, a csomag utolsó tárgya; NO_ITEM, ha a csomag üres (ekkor a lista elejére kerül)
static uint8_t find_pack_position_for(const struct Item *item)
{
    uint8_t last_same_which = NO_ITEM;
    uint8_t last_same_type = NO_ITEM;
    uint8_t last = NO_ITEM;

    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        const struct Item *pack_item = get_item(index);
        if (pack_item->type == item->type) {
            last_same_type = index;
            if (pack_item->which == item->which) {
                last_same_which = index;
            }
        }
        last = index;
    }

    if (last_same_which != NO_ITEM) return last_same_which;
    if (last_same_type != NO_ITEM) return last_same_type;
    return last;
}

uint8_t add_item_to_pack(uint8_t index)
{
    struct Item *item = get_item(index);

    // a köteg darabszáma nem csordul túl: a bájital, a tekercs és az étel darabszáma a csomag helyeinek száma alatt marad,
    // az azonos kötegazonosítójú fegyverek pedig egyetlen, legfeljebb 255 darabos kötegből váltak ki
    uint8_t stack = find_pack_stack_for(item);
    if (stack != NO_ITEM) {
        get_item(stack)->count += item->count;
        free_item(index);
        return stack;
    }

    uint8_t previous = find_pack_position_for(item);
    item->location.pack_letter = choose_free_pack_letter();
    insert_item_after(&game.first_pack_item, previous, index);
    return index;
}

void report_moving_onto_item(uint8_t index)
{
    message_format(text_moved_onto, format_item_name(get_item(index)), NULL, 0);
}

void pick_up_item(uint8_t index)
{
    struct Item *item = get_item(index);

    // az arany nem kerül a csomagba
    if (item->type == ITEM_TYPE_GOLD) {
        uint16_t amount = item->properties.gold_amount;
        game.gold += amount;
        take_item_from_floor(index);
        free_item(index);
        message_format(text_found_gold, NULL, NULL, amount);
        return;
    }
    // ide jön: a már látott scare monster tekercs porrá válik

    if (!has_pack_room_for(index)) {
        message(text_no_room_in_pack);
        report_moving_onto_item(index);
        return;
    }

    take_item_from_floor(index);
    uint8_t pack_index = add_item_to_pack(index);
    // ide jön: a tárgy megtaláltnak számít (ISFOUND); az amulettel has_amulet

    const struct Item *pack_item = get_item(pack_index);
    char letter_text[2] = { pack_item->location.pack_letter, 0 };
    message_format(text_you_now_have, format_item_name(pack_item), letter_text, 0);
}
