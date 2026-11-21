#include "item_names.h"

#include <stdbool.h>
#include <string.h>

#include "items.h"
#include "num.h"
#include "player.h"
#include "rng.h"
#include "texts.h"

// minden típusnak jut álnév, így a választó ciklusok véget érnek
#if POTION_COLOR_COUNT < POTION_SUBTYPE_COUNT || RING_STONE_COUNT < RING_SUBTYPE_COUNT
    #error "kevesebb az álnév, mint a típus"
#endif

// a pálca anyaga fele eséllyel fém, fele eséllyel fa: mindkét részből kell lennie, és együtt minden pálcatípusnak jusson
#if STICK_METAL_COUNT < 1 || STICK_WOOD_COUNT < 1 || STICK_MATERIAL_COUNT < STICK_SUBTYPE_COUNT
    #error "kevés a pálcaanyag"
#endif

// az álnév indexe 8 bites
#if POTION_COLOR_COUNT > 255 || RING_STONE_COUNT > 255 || STICK_MATERIAL_COUNT > 255
    #error "túl hosszú álnévlista: az index 8 bites"
#endif

static bool is_index_among(const uint8_t *indexes, uint8_t count, uint8_t index)
{
    for (uint8_t position = 0; position < count; position++) {
        if (indexes[position] == index) return true;
    }
    return false;
}

// altípusonként egy index a 0..list_length-1 közül, ismétlés nélkül
static void choose_pseudonyms(uint8_t *pseudonyms, uint8_t subtype_count, uint8_t list_length)
{
    for (uint8_t which = 0; which < subtype_count; which++) {
        uint8_t index;
        do {
            index = rnd(list_length);
        } while (is_index_among(pseudonyms, which, index));
        pseudonyms[which] = index;
    }
}

// pálcatípusonként egy anyag a közös listából, ismétlés nélkül: fele eséllyel fém (pálca), fele eséllyel fa (bot)
static void choose_stick_materials(void)
{
    for (uint8_t which = 0; which < STICK_SUBTYPE_COUNT; which++) {
        uint8_t index;
        do {
            if (rnd(2) == 0) {
                index = rnd(STICK_METAL_COUNT);
            } else {
                index = STICK_METAL_COUNT + rnd(STICK_WOOD_COUNT);
            }
        } while (is_index_among(game.stick_materials, which, index));
        game.stick_materials[which] = index;
    }
}

// a feliratgenerátor 16 bites xorshift, saját állapottal, így a felirat előállítása nem hívja az RN-t;
// a kezdőállapot a mag és az altípus szorzatának kizáró vagya: a szorzó páratlan, így altípusonként más
#define TITLE_STATE_MULTIPLIER  0x9e37

static uint16_t title_random_state;

static uint16_t roll_title_random(uint16_t n)
{
    uint16_t state = title_random_state;
    state ^= state << 7;
    state ^= state >> 9;
    state ^= state << 8;
    title_random_state = state;
    return state % n;
}

// a 0 kezdőállapot 0 marad: akkor minden dobás 0, és a felirat "a a", ez is érvényes
void write_scroll_title(char *out, uint8_t which)
{
    title_random_state = game.scroll_title_seed ^ (uint16_t)(which * TITLE_STATE_MULTIPLIER);

    uint8_t length = 0;
    bool is_full = false;
    uint8_t word_count = roll_title_random(SCROLL_TITLE_WORDS_RANGE) + SCROLL_TITLE_WORDS_MIN;

    for (uint8_t word = 0; word < word_count && !is_full; word++) {
        uint8_t syllable_count = roll_title_random(SCROLL_WORD_SYLLABLES_RANGE) + SCROLL_WORD_SYLLABLES_MIN;

        for (uint8_t syllable = 0; syllable < syllable_count && !is_full; syllable++) {
            const char *text = text_scroll_syllables[roll_title_random(SCROLL_SYLLABLE_COUNT)];
            // a szó első szótagja előtt szóköz áll, kivéve az első szót; a szótagok rövidek, így az első mindig elfér, és a felirat nem üres
            bool needs_space = word > 0 && syllable == 0;

            if (length + needs_space + strlen(text) > SCROLL_TITLE_LENGTH_MAX) {
                is_full = true;
            } else {
                if (needs_space) {
                    out[length++] = ' ';
                }
                while (*text) {
                    out[length++] = *text++;
                }
            }
        }
    }
    out[length] = 0;
}

#define FINGERPRINT_START  5381                 // a djb2 kezdőértéke

// a szöveg 16 bites ujjlenyomata (djb2): egyforma szövegeknek egyforma
static uint16_t calculate_text_fingerprint(const char *text)
{
    uint16_t fingerprint = FINGERPRINT_START;
    while (*text) {
        fingerprint = (fingerprint << 5) + fingerprint + (uint8_t)*text++;
    }
    return fingerprint;
}

// a feliratok szövegét kell összevetni, nem a szótagjaikat: két különböző szótagsor is adhat egyforma feliratot (o + od = ood);
// ha két különböző felirat ujjlenyomata egyezik, az újrasorsolás fölösleges, de ártalmatlan
static bool are_scroll_title_fingerprints_distinct(void)
{
    uint16_t fingerprints[SCROLL_SUBTYPE_COUNT];
    char title[SCROLL_TITLE_SIZE];

    for (uint8_t which = 0; which < SCROLL_SUBTYPE_COUNT; which++) {
        write_scroll_title(title, which);
        uint16_t fingerprint = calculate_text_fingerprint(title);
        for (uint8_t other = 0; other < which; other++) {
            if (fingerprints[other] == fingerprint) return false;
        }
        fingerprints[which] = fingerprint;
    }
    return true;
}

void init_item_names(void)
{
    memset(game.potion_knowledge, SUBTYPE_UNKNOWN, sizeof(game.potion_knowledge));
    memset(game.scroll_knowledge, SUBTYPE_UNKNOWN, sizeof(game.scroll_knowledge));
    memset(game.ring_knowledge, SUBTYPE_UNKNOWN, sizeof(game.ring_knowledge));
    memset(game.stick_knowledge, SUBTYPE_UNKNOWN, sizeof(game.stick_knowledge));

    choose_pseudonyms(game.potion_colors, POTION_SUBTYPE_COUNT, POTION_COLOR_COUNT);
    choose_pseudonyms(game.ring_stones, RING_SUBTYPE_COUNT, RING_STONE_COUNT);
    choose_stick_materials();

    // szinte mindig elsőre különbözők
    do {
        game.scroll_title_seed = RN();
    } while (!are_scroll_title_fingerprints_distinct());
}

// a név a formázás alatt; a puffer a leghosszabb nevet is elbírja (ITEM_NAME_LENGTH_MAX), ezért az írás nem ellenőrzi a hosszt
static char item_name[ITEM_NAME_SIZE];
static uint8_t item_name_length;
static bool item_name_has_article;              // a név az "a " névelővel kezdődik, ezt a végén a complete_article igazítja

static void add_character(char character)
{
    item_name[item_name_length++] = character;
}

static void add_text(const char *text)
{
    while (*text) {
        item_name[item_name_length++] = *text++;
    }
}

static void add_number(int32_t number)
{
    item_name_length += format_int(item_name + item_name_length, number, 0);
}

// a nem negatív szám is előjellel: "+0", "+1", "-2"
static void add_signed_number(int16_t number)
{
    if (number >= 0) {
        add_character('+');
    }
    add_number(number);
}

// a szó, több darabnál "s" végződéssel: "arrow", "arrows"
static void add_counted_word(const char *word, uint8_t count)
{
    add_text(word);
    if (count > 1) {
        add_character('s');
    }
}

// egy darabnál a névelő, különben a darabszám, utána szóköz
static void add_count_or_article(uint8_t count)
{
    if (count == 1) {
        add_text("a ");
        item_name_has_article = true;
    } else {
        add_number(count);
        add_character(' ');
    }
}

#define ARTICLE_LENGTH  2                       // "a "

static bool is_vowel(char character)
{
    return character == 'a' || character == 'e' || character == 'i' || character == 'o' || character == 'u';
}

// a név eleji "a " névelő magánhangzó előtt "an "
static void complete_article(void)
{
    if (!is_vowel(item_name[ARTICLE_LENGTH])) return;

    memmove(item_name + ARTICLE_LENGTH, item_name + ARTICLE_LENGTH - 1, item_name_length - ARTICLE_LENGTH + 1);
    item_name[ARTICLE_LENGTH - 1] = 'n';
    item_name_length++;
}

// ismert típus: "a potion of healing", "2 potions of healing"
static void add_known_kind_name(uint8_t count, const char *kind, const char *real_name)
{
    add_count_or_article(count);
    add_counted_word(kind, count);
    add_text(text_of);
    add_text(real_name);
}

// ismeretlen típus: "a blue potion", "2 blue potions"
static void add_unknown_kind_name(uint8_t count, const char *pseudonym, const char *kind)
{
    add_count_or_article(count);
    add_text(pseudonym);
    add_character(' ');
    add_counted_word(kind, count);
}

// az ismert típus neve után: "(blue)"
static void add_pseudonym_in_parentheses(const char *pseudonym)
{
    add_character('(');
    add_text(pseudonym);
    add_character(')');
}

static void add_potion_name(const struct Item *item)
{
    uint8_t which = item->which;
    const char *color = text_potion_colors[game.potion_colors[which]];

    if (game.potion_knowledge[which] == SUBTYPE_IDENTIFIED) {
        add_known_kind_name(item->count, text_potion_kind, text_potion_names[which]);
        add_pseudonym_in_parentheses(color);
    } else {
        add_unknown_kind_name(item->count, color, text_potion_kind);
    }
}

// ismeretlen típus: "a scroll titled 'zok bie'"
static void add_scroll_name(const struct Item *item)
{
    uint8_t which = item->which;

    if (game.scroll_knowledge[which] == SUBTYPE_IDENTIFIED) {
        add_known_kind_name(item->count, text_scroll_kind, text_scroll_names[which]);
    } else {
        add_count_or_article(item->count);
        add_counted_word(text_scroll_kind, item->count);
        add_text(text_titled);
        add_character('\'');
        write_scroll_title(item_name + item_name_length, which);
        item_name_length += strlen(item_name + item_name_length);
        add_character('\'');
    }
}

// adag: "some food", "3 rations of food"; gyümölcs: "a slime-mold", "2 slime-molds"
static void add_food_name(const struct Item *item)
{
    if (item->which == FOOD_FRUIT) {
        add_count_or_article(item->count);
        add_counted_word(text_fruit_name, item->count);
    } else if (item->count == 1) {
        add_text(text_some_food);
    } else {
        add_number(item->count);
        add_text(text_rations_of_food);
    }
}

// "a mace", azonosítva "a +1,+1 mace", kötegben "25 +0,+0 arrows"
static void add_weapon_name(const struct Item *item)
{
    add_count_or_article(item->count);
    if (item->flags & ITEM_KNOWN) {
        add_signed_number(item->properties.weapon.hit_bonus);
        add_character(',');
        add_signed_number(item->properties.weapon.damage_bonus);
        add_character(' ');
    }
    add_counted_word(text_weapon_names[item->which], item->count);
}

// névelő nélkül: "ring mail", azonosítva "+1 ring mail [protection 4]": az eltérés az alapvédelemtől, és a védelem a státuszsor módján
static void add_armor_name(const struct Item *item)
{
    int8_t armor_class = item->properties.armor_class;
    bool is_known = item->flags & ITEM_KNOWN;

    if (is_known) {
        add_signed_number(get_armor_base_class(item->which) - armor_class);
        add_character(' ');
    }
    add_text(text_armor_names[item->which]);
    if (is_known) {
        add_text(" [");
        add_text(text_protection);
        add_number(BASE_ARMOR - armor_class);
        add_character(']');
    }
}

// "a ruby ring", ismert típusnál "a ring of protection(ruby)", azonosítva a modos gyűrű modjával: "a ring of protection [+1](ruby)"
static void add_ring_name(const struct Item *item)
{
    uint8_t which = item->which;
    const char *stone = text_ring_stones[game.ring_stones[which]];

    if (game.ring_knowledge[which] == SUBTYPE_IDENTIFIED) {
        add_known_kind_name(item->count, text_ring_kind, text_ring_names[which]);
        if ((item->flags & ITEM_KNOWN) && has_ring_modifier(which)) {
            add_text(" [");
            add_signed_number(item->properties.ring_modifier);
            add_character(']');
        }
        add_pseudonym_in_parentheses(stone);
    } else {
        add_unknown_kind_name(item->count, stone, text_ring_kind);
    }
}

// fémből pálca, fából bot: "an iron wand", ismert típusnál "a wand of light(iron)", azonosítva a töltéssel: "a wand of light [5 charges](iron)"
static void add_stick_name(const struct Item *item)
{
    uint8_t which = item->which;
    uint8_t material_index = game.stick_materials[which];
    const char *material = text_stick_materials[material_index];
    const char *kind = material_index < STICK_METAL_COUNT ? text_wand_kind : text_staff_kind;

    if (game.stick_knowledge[which] == SUBTYPE_IDENTIFIED) {
        add_known_kind_name(item->count, kind, text_stick_names[which]);
        if (item->flags & ITEM_KNOWN) {
            add_text(" [");
            add_number(item->properties.charges);
            add_text(text_charges);
            add_character(']');
        }
        add_pseudonym_in_parentheses(material);
    } else {
        add_unknown_kind_name(item->count, material, kind);
    }
}

const char *format_item_name(const struct Item *item)
{
    item_name_length = 0;
    item_name_has_article = false;

    switch (item->type) {
        case ITEM_TYPE_POTION:
            add_potion_name(item);
            break;
        case ITEM_TYPE_SCROLL:
            add_scroll_name(item);
            break;
        case ITEM_TYPE_FOOD:
            add_food_name(item);
            break;
        case ITEM_TYPE_WEAPON:
            add_weapon_name(item);
            break;
        case ITEM_TYPE_ARMOR:
            add_armor_name(item);
            break;
        case ITEM_TYPE_RING:
            add_ring_name(item);
            break;
        case ITEM_TYPE_STICK:
            add_stick_name(item);
            break;
        case ITEM_TYPE_GOLD:
            add_number(item->properties.gold_amount);
            add_text(text_gold_pieces);
            break;
        case ITEM_TYPE_AMULET:
            add_text(text_amulet_name);
            break;
    }

    if (item_name_has_article) {
        complete_article();
    }
    item_name[item_name_length] = 0;
    return item_name;
}
