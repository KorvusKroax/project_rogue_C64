#include "tests.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../platform/platform.h"
#include "../discoveries.h"
#include "../item_names.h"
#include "../items.h"
#include "../map.h"
#include "../num.h"
#include "../state.h"
#include "../message.h"
#include "../texts.h"
#include "dev.h"

// a lista index. eleme a buffer-be, amely ITEM_NAME_SIZE bájtos, mert az elem a tárgynévbe kerül, így nem hosszabb annál; a buffer-t adja
static const char *read_list_text(char *buffer, const char *const *list, uint8_t index)
{
    copy_text_from_table(buffer, list, index, ITEM_NAME_LENGTH_MAX);
    return buffer;
}

// a lista minden eleme kisbetűvel kezdődik, és egy listán belül mind különböző
static bool is_text_list_valid(const char *const *list, uint8_t count)
{
    char text[ITEM_NAME_SIZE];
    char other_text[ITEM_NAME_SIZE];

    for (uint8_t index = 0; index < count; index++) {
        if (get_text_from_table(list, index) == NULL) return false;
        read_list_text(text, list, index);
        if (text[0] < 'a' || text[0] > 'z') return false;

        for (uint8_t other = index + 1; other < count; other++) {
            if (strcmp(text, read_list_text(other_text, list, other)) == 0) return false;
        }
    }
    return true;
}

int test_item_name_lists(uint16_t i)
{
    if (!is_text_list_valid(text_potion_names, POTION_SUBTYPE_COUNT)) return false;
    if (!is_text_list_valid(text_scroll_names, SCROLL_SUBTYPE_COUNT)) return false;
    if (!is_text_list_valid(text_weapon_names, WEAPON_SUBTYPE_COUNT)) return false;
    if (!is_text_list_valid(text_armor_names, ARMOR_SUBTYPE_COUNT)) return false;
    if (!is_text_list_valid(text_ring_names, RING_SUBTYPE_COUNT)) return false;
    if (!is_text_list_valid(text_stick_names, STICK_SUBTYPE_COUNT)) return false;
    if (!is_text_list_valid(text_potion_colors, POTION_COLOR_COUNT)) return false;
    if (!is_text_list_valid(text_ring_stones, RING_STONE_COUNT)) return false;
    if (!is_text_list_valid(text_stick_materials, STICK_MATERIAL_COUNT)) return false;
    return is_text_list_valid(text_scroll_syllables, SCROLL_SYLLABLE_COUNT);
}

// mind a lista tartományában, és nincs két egyforma
static bool are_pseudonyms_valid(const uint8_t *pseudonyms, uint8_t count, uint8_t list_length)
{
    for (uint8_t which = 0; which < count; which++) {
        if (pseudonyms[which] >= list_length) return false;
        for (uint8_t other = which + 1; other < count; other++) {
            if (pseudonyms[other] == pseudonyms[which]) return false;
        }
    }
    return true;
}

static bool are_all_unknown(const uint8_t *knowledge, uint8_t count)
{
    for (uint8_t which = 0; which < count; which++) {
        if (knowledge[which] != SUBTYPE_UNKNOWN) return false;
    }
    return true;
}

#define PSEUDONYM_TEST_SEED_COUNT  20           // ennyi új játék, rögzített magokkal

int test_item_pseudonyms(uint16_t i)
{
    bool wand_seen = false;
    bool staff_seen = false;

    for (uint8_t seed = 1; seed <= PSEUDONYM_TEST_SEED_COUNT; seed++) {
        game.seed = seed;
        // az előző játék ismerete ne maradjon meg
        memset(game.potion_knowledge, SUBTYPE_IDENTIFIED, sizeof(game.potion_knowledge));
        memset(game.scroll_knowledge, SUBTYPE_IDENTIFIED, sizeof(game.scroll_knowledge));
        memset(game.ring_knowledge, SUBTYPE_IDENTIFIED, sizeof(game.ring_knowledge));
        memset(game.stick_knowledge, SUBTYPE_IDENTIFIED, sizeof(game.stick_knowledge));
        init_item_names();

        if (!are_all_unknown(game.potion_knowledge, POTION_SUBTYPE_COUNT)) return false;
        if (!are_all_unknown(game.scroll_knowledge, SCROLL_SUBTYPE_COUNT)) return false;
        if (!are_all_unknown(game.ring_knowledge, RING_SUBTYPE_COUNT)) return false;
        if (!are_all_unknown(game.stick_knowledge, STICK_SUBTYPE_COUNT)) return false;

        if (!are_pseudonyms_valid(game.potion_colors, POTION_SUBTYPE_COUNT, POTION_COLOR_COUNT)) return false;
        if (!are_pseudonyms_valid(game.ring_stones, RING_SUBTYPE_COUNT, RING_STONE_COUNT)) return false;
        if (!are_pseudonyms_valid(game.stick_materials, STICK_SUBTYPE_COUNT, STICK_MATERIAL_COUNT)) return false;

        for (uint8_t which = 0; which < STICK_SUBTYPE_COUNT; which++) {
            if (game.stick_materials[which] < STICK_METAL_COUNT) {
                wand_seen = true;
            } else {
                staff_seen = true;
            }
        }
    }
    return wand_seen && staff_seen;
}

// nem üres, legfeljebb SCROLL_TITLE_LENGTH_MAX karakter, kisbetűs szavak egy-egy szóközzel elválasztva
static bool is_scroll_title_valid(const char *title)
{
    size_t length = strlen(title);
    if (length == 0 || length > SCROLL_TITLE_LENGTH_MAX) return false;
    if (title[0] == ' ' || title[length - 1] == ' ') return false;

    for (uint8_t position = 0; position < length; position++) {
        char character = title[position];
        if (character == ' ') {
            if (title[position + 1] == ' ') return false;
        } else if (character < 'a' || character > 'z') {
            return false;
        }
    }
    return true;
}

static char scroll_titles[SCROLL_SUBTYPE_COUNT][SCROLL_TITLE_SIZE];

int test_scroll_titles(uint16_t i)
{
    game.seed = i + 1;
    init_item_names();

    uint32_t seed_after_init = game.seed;
    for (uint8_t which = 0; which < SCROLL_SUBTYPE_COUNT; which++) {
        write_scroll_title(scroll_titles[which], which);
        if (!is_scroll_title_valid(scroll_titles[which])) return false;
    }

    // a felirat előállítása nem hívja az RN-t, és újra ugyanaz jön ki
    if (game.seed != seed_after_init) return false;
    for (uint8_t which = 0; which < SCROLL_SUBTYPE_COUNT; which++) {
        char title[SCROLL_TITLE_SIZE];
        write_scroll_title(title, which);
        if (strcmp(title, scroll_titles[which]) != 0) return false;
    }

    // egy játékon belül nincs két egyforma
    for (uint8_t which = 0; which < SCROLL_SUBTYPE_COUNT; which++) {
        for (uint8_t other = which + 1; other < SCROLL_SUBTYPE_COUNT; other++) {
            if (strcmp(scroll_titles[which], scroll_titles[other]) == 0) return false;
        }
    }
    return true;
}

// a szöveg indexe a listában; a teszt csak a listában lévő szöveget keres
static uint8_t find_text(const char *const *list, uint8_t count, const char *text)
{
    char list_text[ITEM_NAME_SIZE];
    uint8_t index = 0;
    while (index < count - 1 && strcmp(read_list_text(list_text, list, index), text) != 0) {
        index++;
    }
    return index;
}

static struct Item example;

static void set_example(uint8_t type, uint8_t which, uint8_t count, uint8_t flags)
{
    memset(&example, 0, sizeof(example));
    example.type = type;
    example.which = which;
    example.count = count;
    example.flags = flags;
}

static bool is_example_named(const char *expected)
{
    return strcmp(format_item_name(&example), expected) == 0;
}

// a példák típusneve és címkéje a névkészletben (test_item_names foglalja és szabadítja fel)
#define EXAMPLE_NAME  "mine"
static uint8_t example_name;

static bool are_potion_names_valid(void)
{
    uint8_t healing = find_text(text_potion_names, POTION_SUBTYPE_COUNT, "healing");
    uint8_t confusion = find_text(text_potion_names, POTION_SUBTYPE_COUNT, "confusion");
    game.potion_colors[healing] = find_text(text_potion_colors, POTION_COLOR_COUNT, "blue");
    game.potion_colors[confusion] = find_text(text_potion_colors, POTION_COLOR_COUNT, "amber");
    game.potion_knowledge[healing] = SUBTYPE_UNKNOWN;
    game.potion_knowledge[confusion] = SUBTYPE_UNKNOWN;

    set_example(ITEM_TYPE_POTION, healing, 1, 0);
    if (!is_example_named("a blue potion")) return false;
    example.count = 2;
    if (!is_example_named("2 blue potions")) return false;

    game.potion_knowledge[healing] = SUBTYPE_IDENTIFIED;
    if (!is_example_named("2 potions of healing(blue)")) return false;
    example.count = 1;
    if (!is_example_named("a potion of healing(blue)")) return false;

    game.potion_knowledge[healing] = example_name;
    if (!is_example_named("a potion called " EXAMPLE_NAME "(blue)")) return false;
    example.count = 2;
    if (!is_example_named("2 potions called " EXAMPLE_NAME "(blue)")) return false;

    set_example(ITEM_TYPE_POTION, confusion, 1, 0);
    return is_example_named("an amber potion");
}

static bool are_scroll_names_valid(void)
{
    uint8_t identify_potion = find_text(text_scroll_names, SCROLL_SUBTYPE_COUNT, "identify potion");
    game.scroll_knowledge[identify_potion] = SUBTYPE_IDENTIFIED;

    set_example(ITEM_TYPE_SCROLL, identify_potion, 1, 0);
    if (!is_example_named("a scroll of identify potion")) return false;
    example.count = 2;
    if (!is_example_named("2 scrolls of identify potion")) return false;

    game.scroll_knowledge[identify_potion] = example_name;
    if (!is_example_named("2 scrolls called " EXAMPLE_NAME)) return false;
    example.count = 1;
    if (!is_example_named("a scroll called " EXAMPLE_NAME)) return false;

    // a 0. altípus a 0 maggal 0 állapotból indul: minden dobás 0, egy-egy "a" szótagú két szó
    game.scroll_title_seed = 0;
    game.scroll_knowledge[0] = SUBTYPE_UNKNOWN;
    set_example(ITEM_TYPE_SCROLL, 0, 1, 0);
    if (!is_example_named("a scroll titled 'a a'")) return false;
    example.count = 3;
    if (!is_example_named("3 scrolls titled 'a a'")) return false;

    // más magnál a név a felirattal: "a scroll titled '<felirat>'"
    game.scroll_title_seed = 12345;
    game.scroll_knowledge[identify_potion] = SUBTYPE_UNKNOWN;
    char expected[sizeof("a scroll titled ''") + SCROLL_TITLE_LENGTH_MAX];
    strcpy(expected, "a scroll titled '");
    write_scroll_title(expected + strlen(expected), identify_potion);
    strcat(expected, "'");
    set_example(ITEM_TYPE_SCROLL, identify_potion, 1, 0);
    return is_example_named(expected);
}

static bool are_food_names_valid(void)
{
    set_example(ITEM_TYPE_FOOD, FOOD_RATION, 1, 0);
    if (!is_example_named("some food")) return false;
    example.count = 3;
    if (!is_example_named("3 rations of food")) return false;

    set_example(ITEM_TYPE_FOOD, FOOD_FRUIT, 1, 0);
    if (!is_example_named("a slime-mold")) return false;
    example.count = 2;
    return is_example_named("2 slime-molds");
}

static void set_weapon_bonuses(int8_t hit_bonus, int8_t damage_bonus)
{
    example.properties.weapon.hit_bonus = hit_bonus;
    example.properties.weapon.damage_bonus = damage_bonus;
}

static bool are_weapon_names_valid(void)
{
    uint8_t mace = find_text(text_weapon_names, WEAPON_SUBTYPE_COUNT, "mace");

    set_example(ITEM_TYPE_WEAPON, mace, 1, 0);
    set_weapon_bonuses(1, 1);
    if (!is_example_named("a mace")) return false;
    example.flags = ITEM_KNOWN;
    if (!is_example_named("a +1,+1 mace")) return false;
    example.label = example_name;
    if (!is_example_named("a +1,+1 mace called " EXAMPLE_NAME)) return false;
    example.label = NO_NAME;
    set_weapon_bonuses(-1, 0);
    if (!is_example_named("a -1,+0 mace")) return false;

    set_example(ITEM_TYPE_WEAPON, WEAPON_ARROW, 1, 0);
    if (!is_example_named("an arrow")) return false;
    example.count = 25;
    if (!is_example_named("25 arrows")) return false;
    example.flags = ITEM_KNOWN;
    if (!is_example_named("25 +0,+0 arrows")) return false;
    example.label = example_name;
    if (!is_example_named("25 +0,+0 arrows called " EXAMPLE_NAME)) return false;
    example.label = NO_NAME;
    example.count = 1;
    return is_example_named("a +0,+0 arrow");
}

static bool are_armor_names_valid(void)
{
    uint8_t ring_mail = find_text(text_armor_names, ARMOR_SUBTYPE_COUNT, "ring mail");

    set_example(ITEM_TYPE_ARMOR, ring_mail, 1, 0);
    example.properties.armor_class = get_armor_base_class(ring_mail) - 1;
    if (!is_example_named("ring mail")) return false;
    example.label = example_name;
    if (!is_example_named("ring mail called " EXAMPLE_NAME)) return false;
    example.flags = ITEM_KNOWN;
    if (!is_example_named("+1 ring mail [protection 4] called " EXAMPLE_NAME)) return false;
    example.label = NO_NAME;
    if (!is_example_named("+1 ring mail [protection 4]")) return false;
    example.properties.armor_class = get_armor_base_class(ring_mail) + 2;
    return is_example_named("-2 ring mail [protection 1]");
}

static bool are_ring_names_valid(void)
{
    uint8_t stealth = find_text(text_ring_names, RING_SUBTYPE_COUNT, "stealth");
    game.ring_stones[RING_PROTECTION] = find_text(text_ring_stones, RING_STONE_COUNT, "ruby");
    game.ring_stones[stealth] = find_text(text_ring_stones, RING_STONE_COUNT, "agate");
    game.ring_knowledge[RING_PROTECTION] = SUBTYPE_UNKNOWN;
    game.ring_knowledge[stealth] = SUBTYPE_UNKNOWN;

    set_example(ITEM_TYPE_RING, RING_PROTECTION, 1, ITEM_KNOWN);
    example.properties.ring_modifier = 1;
    if (!is_example_named("a ruby ring")) return false;
    game.ring_knowledge[RING_PROTECTION] = example_name;
    if (!is_example_named("a ring called " EXAMPLE_NAME " [+1](ruby)")) return false;
    game.ring_knowledge[RING_PROTECTION] = SUBTYPE_IDENTIFIED;
    if (!is_example_named("a ring of protection [+1](ruby)")) return false;
    example.properties.ring_modifier = -1;
    if (!is_example_named("a ring of protection [-1](ruby)")) return false;
    example.flags = 0;
    if (!is_example_named("a ring of protection(ruby)")) return false;
    game.ring_knowledge[RING_PROTECTION] = example_name;
    if (!is_example_named("a ring called " EXAMPLE_NAME "(ruby)")) return false;

    // a mod nélküli gyűrűnek azonosítva sincs modja
    set_example(ITEM_TYPE_RING, stealth, 1, ITEM_KNOWN);
    if (!is_example_named("an agate ring")) return false;
    game.ring_knowledge[stealth] = SUBTYPE_IDENTIFIED;
    return is_example_named("a ring of stealth(agate)");
}

static bool are_stick_names_valid(void)
{
    uint8_t teleport_away = find_text(text_stick_names, STICK_SUBTYPE_COUNT, "teleport away");
    game.stick_materials[STICK_LIGHT] = find_text(text_stick_materials, STICK_MATERIAL_COUNT, "iron");
    game.stick_materials[teleport_away] = find_text(text_stick_materials, STICK_MATERIAL_COUNT, "persimmon wood");
    game.stick_knowledge[STICK_LIGHT] = SUBTYPE_UNKNOWN;
    game.stick_knowledge[teleport_away] = SUBTYPE_UNKNOWN;

    set_example(ITEM_TYPE_STICK, STICK_LIGHT, 1, ITEM_KNOWN);
    example.properties.charges = 5;
    if (!is_example_named("an iron wand")) return false;
    game.stick_knowledge[STICK_LIGHT] = example_name;
    if (!is_example_named("a wand called " EXAMPLE_NAME " [5 charges](iron)")) return false;
    game.stick_knowledge[STICK_LIGHT] = SUBTYPE_IDENTIFIED;
    if (!is_example_named("a wand of light [5 charges](iron)")) return false;
    example.flags = 0;
    if (!is_example_named("a wand of light(iron)")) return false;
    game.stick_knowledge[STICK_LIGHT] = example_name;
    if (!is_example_named("a wand called " EXAMPLE_NAME "(iron)")) return false;

    set_example(ITEM_TYPE_STICK, teleport_away, 1, 0);
    return is_example_named("a persimmon wood staff");
}

static bool are_other_names_valid(void)
{
    set_example(ITEM_TYPE_GOLD, 0, 1, 0);
    example.properties.gold_amount = 25;
    if (!is_example_named("25 Gold pieces")) return false;

    set_example(ITEM_TYPE_AMULET, 0, 1, 0);
    return is_example_named("the Amulet of Yendor");
}

// a kézben tartott tárgy és a viselt páncél neve után az utótag, a címke után is; az utótag a tárgytábla rekordján múlik
static bool are_in_use_names_valid(void)
{
    clear_item_table();
    uint8_t mace = create_item(ITEM_TYPE_WEAPON, WEAPON_MACE);
    uint8_t armor = create_item(ITEM_TYPE_ARMOR, ARMOR_RING_MAIL);
    get_item(mace)->label = example_name;
    game.wielded_item = mace;
    game.worn_armor = armor;

    bool ok = strcmp(format_item_name(get_item(mace)), "a mace called " EXAMPLE_NAME " (weapon in hand)") == 0 &&
        strcmp(format_item_name(get_item(armor)), "ring mail (being worn)") == 0;

    clear_item_table();
    return ok;
}

int test_item_names(uint16_t i)
{
    example_name = store_name(EXAMPLE_NAME);

    bool ok = example_name != NO_NAME && are_potion_names_valid() && are_scroll_names_valid() && are_food_names_valid()
        && are_weapon_names_valid() && are_armor_names_valid() && are_ring_names_valid() && are_stick_names_valid()
        && are_other_names_valid() && are_in_use_names_valid();

    free_name(example_name);
    return ok;
}

// a lista leghosszabb elemének indexe
static uint8_t find_longest_text(const char *const *list, uint8_t count)
{
    char text[ITEM_NAME_SIZE];
    uint8_t longest = 0;
    uint8_t longest_length = 0;
    for (uint8_t index = 0; index < count; index++) {
        uint8_t length = copy_text_from_table(text, list, index, ITEM_NAME_LENGTH_MAX) - text;
        if (length > longest_length) {
            longest = index;
            longest_length = length;
        }
    }
    return longest;
}

#define KNOWLEDGE_ROUND_COUNT  3                // ismeretlen, elnevezett és ismert típus

// minden típus minden altípusa, ismeretlen, a long_name nevű és ismert típusként, a fegyver és a páncél a long_name címkével,
// a leghosszabb álnévvel, 255 darabbal, a leghosszabban kiírt értékekkel és a használatban lévő tárgy utótagjával (a páncél
// viselve, minden más a kézben); az utótag miatt a tárgytábla egy rekordján
static bool do_all_names_fit(uint8_t long_name)
{
    clear_item_table();
    uint8_t slot = allocate_item();
    struct Item *item = get_item(slot);

    memset(game.potion_colors, find_longest_text(text_potion_colors, POTION_COLOR_COUNT), sizeof(game.potion_colors));
    memset(game.ring_stones, find_longest_text(text_ring_stones, RING_STONE_COUNT), sizeof(game.ring_stones));
    memset(game.stick_materials, find_longest_text(text_stick_materials, STICK_MATERIAL_COUNT), sizeof(game.stick_materials));

    // az ismert típus az utolsó: a tesztek után így marad
    uint8_t knowledge_rounds[KNOWLEDGE_ROUND_COUNT] = { SUBTYPE_UNKNOWN, long_name, SUBTYPE_IDENTIFIED };
    for (uint8_t knowledge_round = 0; knowledge_round < KNOWLEDGE_ROUND_COUNT; knowledge_round++) {
        uint8_t knowledge = knowledge_rounds[knowledge_round];
        memset(game.potion_knowledge, knowledge, sizeof(game.potion_knowledge));
        memset(game.scroll_knowledge, knowledge, sizeof(game.scroll_knowledge));
        memset(game.ring_knowledge, knowledge, sizeof(game.ring_knowledge));
        memset(game.stick_knowledge, knowledge, sizeof(game.stick_knowledge));

        for (uint8_t type = 0; type < ITEM_TYPE_COUNT; type++) {
            uint8_t subtype_count = get_tested_subtype_count(type);
            for (uint8_t which = 0; which < subtype_count; which++) {
                set_example(type, which, 255, ITEM_KNOWN);
                example.label = long_name;
                // a fegyver két bónusza -128, a páncél védelme is: "+136 ... [protection 138]"; a töltés 255; az arany 65535
                memset(&example.properties, 0x80, sizeof(example.properties));
                if (type == ITEM_TYPE_STICK) {
                    example.properties.charges = 255;
                }
                if (type == ITEM_TYPE_GOLD) {
                    example.properties.gold_amount = 65535;
                }
                *item = example;
                bool is_armor = type == ITEM_TYPE_ARMOR;
                game.worn_armor = is_armor ? slot : NO_ITEM;
                game.wielded_item = is_armor ? NO_ITEM : slot;
                if (strlen(format_item_name(item)) > ITEM_NAME_LENGTH_MAX) return false;
            }
        }
    }
    clear_item_table();
    return true;
}

int test_longest_item_name(uint16_t i)
{
    char long_name[NAME_SIZE];
    memset(long_name, 'n', NAME_LENGTH_MAX);
    long_name[NAME_LENGTH_MAX] = 0;
    uint8_t long_name_index = store_name(long_name);

    // a leghosszabb név (item_names.h): a leghosszabb nevű, elnevezett típusú bot a legtöbb töltéssel, a kézben; az utótag miatt
    // a tárgytábla rekordja
    uint8_t teleport_away = find_text(text_stick_names, STICK_SUBTYPE_COUNT, "teleport away");
    game.stick_materials[teleport_away] = find_text(text_stick_materials, STICK_MATERIAL_COUNT, "persimmon wood");
    game.stick_knowledge[teleport_away] = long_name_index;

    char expected[ITEM_NAME_SIZE];
    strcpy(expected, "a staff called ");
    strcat(expected, long_name);
    strcat(expected, " [255 charges](persimmon wood) (weapon in hand)");

    clear_item_table();
    uint8_t staff = create_item(ITEM_TYPE_STICK, teleport_away);
    get_item(staff)->flags = ITEM_KNOWN;
    get_item(staff)->properties.charges = 255;
    game.wielded_item = staff;
    bool ok = long_name_index != NO_NAME && strcmp(format_item_name(get_item(staff)), expected) == 0;
    ok = ok && strlen(expected) <= ITEM_NAME_LENGTH_MAX;

    ok = ok && do_all_names_fit(long_name_index);
    free_name(long_name_index);
    return ok;
}

#define NAME_POOL_TEST_FREED  (MAX_NAMES / 2)   // a betelt készletből ez a hely szabadul fel

// a fejlesztői mód foglaltsági üzenete: „Names: <used_count>/<MAX_NAMES>.”
static bool is_name_pool_usage_shown(uint8_t used_count)
{
    static const char start[] = "Names: ";
    char expected[sizeof(start) + 2 * FORMAT_BUFFER_SIZE + 1];
    strcpy(expected, start);
    uint8_t length = sizeof(start) - 1;
    length += format_int(expected + length, used_count, 0);
    expected[length++] = '/';
    length += format_int(expected + length, MAX_NAMES, 0);
    strcpy(expected + length, ".");

    message_clear();
    report_name_pool_usage();
    bool ok = does_message_bar_start_with(expected);
    message_clear();
    return ok;
}

int test_name_pool(uint16_t i)
{
    game.seed = 1;
    init_item_names();
    if (is_name_pool_full() || !is_name_pool_usage_shown(0)) return false;

    // MAX_NAMES név fér el, mindig a legkisebb szabad helyen, és a szövegük visszaolvasható; utána betelt
    char text[NAME_SIZE];
    for (uint8_t count = 0; count < MAX_NAMES; count++) {
        format_int(text, count, 0);
        if (store_name(text) != count + 1) return false;
    }
    for (uint8_t count = 0; count < MAX_NAMES; count++) {
        format_int(text, count, 0);
        if (strcmp(get_name(count + 1), text) != 0) return false;
    }
    if (!is_name_pool_full() || store_name("x") != NO_NAME || !is_name_pool_usage_shown(MAX_NAMES)) return false;

    // a felszabadított hely újra foglalható; a NO_NAME felszabadítása nem változtat semmin
    free_name(NAME_POOL_TEST_FREED);
    free_name(NO_NAME);
    if (is_name_pool_full() || !is_name_pool_usage_shown(MAX_NAMES - 1)) return false;
    if (store_name("x") != NAME_POOL_TEST_FREED || !is_name_pool_full()) return false;

    // az átnevezés helyben cseréli a szöveget, a leghosszabb névre is
    memset(text, 'a', NAME_LENGTH_MAX);
    text[NAME_LENGTH_MAX] = 0;
    set_name(NAME_POOL_TEST_FREED, text);
    if (strcmp(get_name(NAME_POOL_TEST_FREED), text) != 0) return false;

    // új játéknál a készlet üres
    init_item_names();
    return store_name("x") == 1;
}

// a bájitaltípus neve a felfedezések alakjában: egy darab, azonosítatlanul
static void write_discovered_potion_name(char *out, uint8_t which)
{
    set_example(ITEM_TYPE_POTION, which, 1, 0);
    strcpy(out, format_item_name(&example));
}

// a felfedezések listájában a text pontosan egyszer szerepel; a text nem a format_item_name eredménye, hanem másolata
static bool is_discovery_listed_once(const char *text)
{
    uint8_t count = 0;
    for (uint8_t index = 0; get_discovery_entry(index) != NULL; index++) {
        if (strcmp(get_discovery_entry(index), text) == 0) {
            count++;
        }
    }
    return count == 1;
}

#define DISCOVERY_TEST_CALL_COUNT  8            // ennyi újrakeverés alatt az első tétel már más (14 tételnél egy hívásban 1/14 eséllyel marad)

int test_discovery_list(uint16_t i)
{
    init_item_names();
    uint32_t seed = game.seed;
    char expected[ITEM_NAME_SIZE];

    // egy elnevezett és egy ismert bájitaltípus a lista elején, bármilyen sorrendben; a többi típusról semmi
    uint8_t name_index = store_name("mine");
    game.potion_knowledge[0] = name_index;
    game.potion_knowledge[1] = SUBTYPE_IDENTIFIED;
    bool ok = prepare_discovery_list(CHAR_POTION) && get_discovery_entry(2) == NULL;
    write_discovered_potion_name(expected, 0);
    ok = ok && is_discovery_listed_once(expected);
    write_discovered_potion_name(expected, 1);
    ok = ok && is_discovery_listed_once(expected);

    ok = ok && prepare_discovery_list(CHAR_SCROLL) && get_discovery_entry(1) == NULL;
    ok = ok && strcmp(get_discovery_entry(0), "Haven't discovered anything about any scrolls") == 0;

    // a négy lista üres tételekkel elválasztva: 2 bájital, üres, tekercs, üres, gyűrű, üres, pálca
    ok = ok && prepare_discovery_list('*') && get_discovery_entry(8) == NULL;
    ok = ok && get_discovery_entry(2)[0] == 0 && get_discovery_entry(4)[0] == 0 && get_discovery_entry(6)[0] == 0;
    ok = ok && strcmp(get_discovery_entry(7), "Haven't discovered anything about any sticks") == 0;

    ok = ok && !prepare_discovery_list('x');

    // minden ismert típus pontosan egyszer
    memset(game.potion_knowledge, SUBTYPE_IDENTIFIED, sizeof(game.potion_knowledge));
    ok = ok && prepare_discovery_list(CHAR_POTION) && get_discovery_entry(POTION_SUBTYPE_COUNT) == NULL;
    for (uint8_t which = 0; which < POTION_SUBTYPE_COUNT && ok; which++) {
        write_discovered_potion_name(expected, which);
        ok = is_discovery_listed_once(expected);
    }

    // minden hívás újra kever
    strcpy(expected, get_discovery_entry(0));
    bool is_order_changed = false;
    for (uint8_t call = 0; call < DISCOVERY_TEST_CALL_COUNT && !is_order_changed; call++) {
        prepare_discovery_list(CHAR_POTION);
        is_order_changed = strcmp(get_discovery_entry(0), expected) != 0;
    }
    ok = ok && is_order_changed;

    // a keverés nem hívja az RN-t
    ok = ok && game.seed == seed;

    memset(game.potion_knowledge, SUBTYPE_UNKNOWN, sizeof(game.potion_knowledge));
    free_name(name_index);
    return ok;
}
