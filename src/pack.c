#include "pack.h"

#include <stddef.h>
#include <string.h>

#include "equipment.h"
#include "item_names.h"
#include "items.h"
#include "message.h"
#include "state.h"
#include "text_list.h"
#include "texts.h"

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

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

uint8_t find_pack_item_by_letter(uint8_t letter)
{
    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        if (get_item(index)->location.pack_letter == letter) return index;
    }
    return NO_ITEM;
}

bool is_carrying_amulet(void)
{
    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        if (get_item(index)->type == ITEM_TYPE_AMULET) return true;
    }
    return false;
}

bool is_item_in_pack(uint8_t index)
{
    for (uint8_t pack_index = game.first_pack_item; pack_index != NO_ITEM; pack_index = get_item(pack_index)->next) {
        if (pack_index == index) return true;
    }
    return false;
}

// a legkisebb betű, amelyet egyik csomagbeli tárgy sem visel; a ciklus a 'z' előtt megáll, mert a tárgy csak akkor kerül a csomagba,
// ha befér, és a csomag MAX_PACK helyén legfeljebb ennyi tárgy van, ez pedig nem több a betűknél (pack.h)
static uint8_t choose_free_pack_letter(void)
{
    uint8_t letter = FIRST_PACK_LETTER;
    while (find_pack_item_by_letter(letter) != NO_ITEM) {
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
        struct Item *stack_item = get_item(stack);
        stack_item->count += item->count;
        // a címke nélküli köteg átveszi a tárgy címkéjét; ha mindkettőnek van, a tárgyé a rekordjával együtt felszabadul
        if (stack_item->label == NO_NAME) {
            stack_item->label = item->label;
            item->label = NO_NAME;
        }
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
    // ide jön: a tárgy megtaláltnak számít (ISFOUND)

    const struct Item *pack_item = get_item(pack_index);
    char letter_text[2] = { pack_item->location.pack_letter, 0 };
    message_format(text_you_now_have, format_item_name(pack_item), letter_text, 0);
}

bool drop_item(uint8_t index)
{
    struct Item *item = get_item(index);

    // bájital-, tekercs- és ételkötegből egy darab új rekordként kerül le, a köteg a csomagban marad; ehhez szabad hely kell,
    // és ez a levétel előtt dől el, így ha nincs, a tárgy használatban marad
    bool is_piece = item->count > 1 && is_stackable_type(item->type);
    if (is_piece && game.item_count == MAX_ITEMS) {
        message(text_no_room_for_that);
        return false;
    }
    if (!release_item(index)) return true;

    uint8_t dropped_index = index;
    if (is_piece) {
        // a hely megvan: üres kör csak a páncél levételével jár, és a páncél nem köteg; a darabok nem kapnak címkét,
        // így a másolatnak sincs
        dropped_index = allocate_item();
        struct Item *dropped = get_item(dropped_index);
        *dropped = *item;
        dropped->count = 1;
        item->count--;
    } else {
        remove_item_from_list(&game.first_pack_item, index);
    }

    put_item_on_floor(dropped_index, game.player.x, game.player.y);
    message_format(text_dropped, format_item_name(get_item(dropped_index)), NULL, 0);
    return true;
}

void use_up_pack_item(uint8_t index)
{
    if (index == game.wielded_item) {
        game.wielded_item = NO_ITEM;
    }

    struct Item *item = get_item(index);
    if (item->count > 1) {
        item->count--;
        return;
    }
    remove_item_from_list(&game.first_pack_item, index);
    free_item(index);
}

// a listasor a betűvel kezdődik: "a) a mace"
#define PACK_LIST_PREFIX_LENGTH  3
#define PACK_LIST_ENTRY_SIZE     (PACK_LIST_PREFIX_LENGTH + ITEM_NAME_SIZE)

// a buffers szakaszban ($C400-$C7FF, platform.c)
#pragma bss(buffers)
static char pack_list_entry[PACK_LIST_ENTRY_SIZE];
#pragma bss(bss)

static uint8_t pack_list_filter;                // PACK_FILTER_

static bool is_item_in_pack_list(const struct Item *item)
{
    switch (pack_list_filter) {
        case PACK_FILTER_NAMEABLE:
            return can_be_named(item->type);
        case PACK_FILTER_WEAPON:
            return item->type == ITEM_TYPE_WEAPON;
        case PACK_FILTER_ARMOR:
            return item->type == ITEM_TYPE_ARMOR;
        case PACK_FILTER_POTION:
            return item->type == ITEM_TYPE_POTION;
        default:
            return true;
    }
}

// a csomag szűrőn átjutó tárgyai közül az index. listasora; NULL, ha nincs ennyi (TextListEntryFunction)
static const char *get_pack_list_entry(uint8_t index)
{
    for (uint8_t item_index = game.first_pack_item; item_index != NO_ITEM; item_index = get_item(item_index)->next) {
        const struct Item *item = get_item(item_index);
        if (!is_item_in_pack_list(item)) continue;
        if (index > 0) {
            index--;
            continue;
        }

        pack_list_entry[0] = item->location.pack_letter;
        pack_list_entry[1] = ')';
        pack_list_entry[2] = ' ';
        strcpy(pack_list_entry + PACK_LIST_PREFIX_LENGTH, format_item_name(item));
        return pack_list_entry;
    }
    return NULL;
}

// a get_pack_list_entry(0) helyett: ha a get_pack_list_entry-t állandó indexszel közvetlenül is hívnánk, az Oscar64 1.32.273
// az indexet a mutatón át érkező hívásokban is ennek venné (README, Oscar64-hibák, 12.)
static bool has_pack_list_entry(void)
{
    for (uint8_t item_index = game.first_pack_item; item_index != NO_ITEM; item_index = get_item(item_index)->next) {
        if (is_item_in_pack_list(get_item(item_index))) return true;
    }
    return false;
}

bool show_pack_list(uint8_t filter)
{
    pack_list_filter = filter;
    if (!has_pack_list_entry()) {
        message(filter == PACK_FILTER_ALL ? text_empty_handed : text_nothing_appropriate);
        return false;
    }
    show_text_list(get_pack_list_entry);
    return true;
}

#pragma optimize(pop)
