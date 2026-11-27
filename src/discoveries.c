#include "discoveries.h"

#include <stddef.h>
#include <string.h>

#include "platform/key.h"
#include "item_names.h"
#include "items.h"
#include "map.h"
#include "message.h"
#include "rng.h"
#include "state.h"
#include "text_list.h"
#include "texts.h"

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

// a text_discovery_type_error ezeket a jeleket sorolja fel
#if CHAR_POTION != '!' || CHAR_SCROLL != '?' || CHAR_RING != '=' || CHAR_STICK != '/'
    #error "a text_discovery_type_error a !?=/ jeleket sorolja fel"
#endif

#define DISCOVERY_TYPE_COUNT  4

// a felfedezhető típusok a `*` lista sorrendjében, a „semmi” sor szavával
struct DiscoveryType {
    uint8_t item_type;
    const char *kind;
};

static const struct DiscoveryType discovery_types[DISCOVERY_TYPE_COUNT] = {
    { ITEM_TYPE_POTION, text_potion_kind },
    { ITEM_TYPE_SCROLL, text_scroll_kind },
    { ITEM_TYPE_RING, text_ring_kind },
    { ITEM_TYPE_STICK, text_stick_kind },
};

// egy tétel egy bájt: a felső bitekben a típus sorszáma (discovery_types), az alsó ENTRY_WHICH_BITS bitben az altípus, vagy
// ENTRY_NOTHING, ha a típusról nincs mit mutatni; ENTRY_SEPARATOR: üres sor a listák között
#define ENTRY_WHICH_BITS  5
#define ENTRY_NOTHING     ((1 << ENTRY_WHICH_BITS) - 1)
#define ENTRY_SEPARATOR   0xff
#define ENTRY_COUNT_MAX   (POTION_SUBTYPE_COUNT + SCROLL_SUBTYPE_COUNT + RING_SUBTYPE_COUNT + STICK_SUBTYPE_COUNT + DISCOVERY_TYPE_COUNT - 1)

#if POTION_SUBTYPE_COUNT >= ENTRY_NOTHING || SCROLL_SUBTYPE_COUNT >= ENTRY_NOTHING || RING_SUBTYPE_COUNT >= ENTRY_NOTHING \
    || STICK_SUBTYPE_COUNT >= ENTRY_NOTHING || ((DISCOVERY_TYPE_COUNT - 1) << ENTRY_WHICH_BITS | ENTRY_NOTHING) >= ENTRY_SEPARATOR
    #error "a tétel nem fér el egy bájtban"
#endif

// a buffers szakaszban ($C400-$C7FF, platform.c)
#pragma bss(buffers)
static uint8_t entries[ENTRY_COUNT_MAX];
#pragma bss(bss)
static uint8_t entry_count;

// a sorrend keverőjének állapota (roll_xorshift): hívásról hívásra megy tovább, és nem kerül a mentésbe; az első híváskor
// az RN állapotából indul (az RN-t nem hívja), így játékonként más; 0 nem lehet, mert az 0 maradna
static uint16_t order_random_state;

static void add_entry(uint8_t entry)
{
    entries[entry_count++] = entry;
}

// a típus ismert és elnevezett altípusai véletlen sorrendben; ha nincs ilyen, a „semmi” tétel
static void add_type_entries(uint8_t type_index)
{
    uint8_t item_type = discovery_types[type_index].item_type;
    const uint8_t *knowledge = get_type_knowledge(item_type);
    uint8_t subtype_count = item_subtype_tables[item_type].count;

    // keverés hátulról: minden helyre a még nem rögzítettek közül egy véletlen
    uint8_t order[ENTRY_NOTHING];
    for (uint8_t which = 0; which < subtype_count; which++) {
        order[which] = which;
    }
    for (uint8_t remaining = subtype_count; remaining > 0; remaining--) {
        uint8_t chosen = roll_xorshift(&order_random_state, remaining);
        uint8_t last = order[remaining - 1];
        order[remaining - 1] = order[chosen];
        order[chosen] = last;
    }

    uint8_t first_entry = entry_count;
    for (uint8_t position = 0; position < subtype_count; position++) {
        uint8_t which = order[position];
        if (knowledge[which] != SUBTYPE_UNKNOWN) {
            add_entry(type_index << ENTRY_WHICH_BITS | which);
        }
    }
    if (entry_count == first_entry) {
        add_entry(type_index << ENTRY_WHICH_BITS | ENTRY_NOTHING);
    }
}

bool prepare_discovery_list(uint8_t type_character)
{
    if (order_random_state == 0) {
        order_random_state = (uint16_t)(game.seed >> 16) | 1;
    }

    entry_count = 0;
    for (uint8_t type_index = 0; type_index < DISCOVERY_TYPE_COUNT; type_index++) {
        if (type_character == '*') {
            if (type_index > 0) {
                add_entry(ENTRY_SEPARATOR);
            }
            add_type_entries(type_index);
        } else if (get_item_character(discovery_types[type_index].item_type) == type_character) {
            add_type_entries(type_index);
        }
    }
    return entry_count > 0;
}

const char *get_discovery_entry(uint8_t index)
{
    if (index >= entry_count) return NULL;

    uint8_t entry = entries[index];
    if (entry == ENTRY_SEPARATOR) return "";

    const struct DiscoveryType *type = &discovery_types[entry >> ENTRY_WHICH_BITS];
    uint8_t which = entry & ENTRY_NOTHING;
    if (which == ENTRY_NOTHING) {
        return prepare_message_text(text_nothing_discovered, type->kind, NULL, 0, false);
    }

    // egy darab, azonosítatlanul: az ismert vagy az elnevezett alak, mod és töltés nélkül
    struct Item item;
    memset(&item, 0, sizeof(item));
    item.type = type->item_type;
    item.which = which;
    item.count = 1;
    return format_item_name(&item);
}

void show_discoveries(void)
{
    while (true) {
        message_prompt(text_discovery_question);
        uint8_t code = wait_for_key();
        message_clear();

        if (code == KEY_ESCAPE) return;
        if (prepare_discovery_list(code)) break;
        message(text_discovery_type_error);
    }
    show_text_list(get_discovery_entry);
}

#pragma optimize(pop)
