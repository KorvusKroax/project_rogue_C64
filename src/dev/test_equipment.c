#include "tests.h"

#include <stdbool.h>
#include <stdint.h>

#include "../equipment.h"
#include "../items.h"
#include "../map.h"
#include "../message.h"
#include "../monsters.h"
#include "../pack.h"
#include "../player.h"
#include "../rooms.h"
#include "../state.h"

// a tesztmezők: egy folyosó, az elején a játékos, két mezővel arrébb a szörny: egy szörnykör alatt mellé lép, de még nem támad
#define EQUIPMENT_PLAYER_X        10
#define EQUIPMENT_PLAYER_Y        (MAP_TOP_ROW + 2)
#define EQUIPMENT_MONSTER_X       (EQUIPMENT_PLAYER_X + 2)
#define EQUIPMENT_PASSAGE_NUMBER  1

#define EQUIPMENT_TEST_MONSTER  'Z'             // zombi: normál sebességű, nem repül, így egy körben egyet lép
#define EQUIPMENT_TEST_ARMOR    0               // leather armor: a védelme az alapérték, így azonosítva "+0 leather armor"

static void put_equipment_tile(uint8_t x, uint8_t y)
{
    set_map_character(x, y, CHAR_PASSAGE);
    add_map_flags(x, y, TILE_PASSAGE | EQUIPMENT_PASSAGE_NUMBER);
}

// az 1. mélységen a kezdőértékű játékos a folyosó elején, a végén egy ébren lévő zombi; üres a padló és a csomag; a zombi rekordja
static struct Monster *put_equipment_scene(void)
{
    clear_map();
    clear_monsters();
    clear_item_table();
    game.dungeon_level = 1;
    init_player();

    for (uint8_t x = EQUIPMENT_PLAYER_X; x <= EQUIPMENT_MONSTER_X; x++) {
        put_equipment_tile(x, EQUIPMENT_PLAYER_Y);
    }
    game.player.x = EQUIPMENT_PLAYER_X;
    game.player.y = EQUIPMENT_PLAYER_Y;
    game.player.room = get_room_at(EQUIPMENT_PLAYER_X, EQUIPMENT_PLAYER_Y);

    struct Monster *monster = get_monster(create_monster(EQUIPMENT_TEST_MONSTER, EQUIPMENT_MONSTER_X, EQUIPMENT_PLAYER_Y));
    start_monster_running(monster);
    return monster;
}

// a halál tesztje után a játékos újra él
static void clear_equipment_scene(void)
{
    message_clear();
    clear_monsters();
    clear_item_table();
    clear_map();
    init_player();
}

// a szörny még a helyén van: nem telt el üres kör
static bool has_monster_stayed(const struct Monster *monster)
{
    return monster->x == EQUIPMENT_MONSTER_X;
}

// a szörny egyet lépett a játékos felé: egy üres kör telt el
static bool has_monster_stepped_once(const struct Monster *monster)
{
    return monster->x == EQUIPMENT_MONSTER_X - 1;
}

// a kézbe vétel elmarad ezzel az üzenettel, nem visz időt, és a kéz nem változik
static bool is_wield_refused(uint8_t index, const char *message_start)
{
    uint8_t wielded = game.wielded_item;
    message_clear();
    return !wield_item(index) && does_message_bar_start_with(message_start) && game.wielded_item == wielded;
}

int test_wield_item(uint16_t i)
{
    struct Monster *monster = put_equipment_scene();
    uint8_t potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    uint8_t armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));

    // a `*` lista csak a fegyvereket mutatja
    message_clear();
    bool ok = !show_pack_list(PACK_FILTER_WEAPON) && does_message_bar_start_with("You don't have anything appropriate.");
    uint8_t mace = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, WEAPON_MACE));
    message_clear();
    ok = ok && show_pack_list(PACK_FILTER_WEAPON) && does_message_bar_start_with("c) a mace");

    // páncélt nem vesz kézbe
    ok = ok && is_wield_refused(armor, "You can't wield armor.");

    // a fegyver kézbe kerül, a kézben tartott tárgy használatban van
    message_clear();
    ok = ok && wield_item(mace) && game.wielded_item == mace && does_message_bar_start_with("You are now wielding a mace (c).");
    ok = ok && is_wield_refused(mace, "That's already in use.");

    // bármi más is kézbe vehető; a viselt páncélra is a páncél elutasítása jön
    ok = ok && wield_item(potion) && game.wielded_item == potion;
    game.worn_armor = armor;
    ok = ok && is_wield_refused(armor, "You can't wield armor.");

    // az átkozott tárgy a kézben marad; üres kéznél nincs mit cserélni
    ok = ok && !report_cursed_wielded_item();
    get_item(potion)->flags |= ITEM_CURSED;
    message_clear();
    ok = ok && report_cursed_wielded_item() && does_message_bar_start_with("You can't. It appears to be cursed.");
    game.wielded_item = NO_ITEM;
    ok = ok && !report_cursed_wielded_item();

    // a kézbe vétel nem jár üres körrel
    ok = ok && has_monster_stayed(monster);

    clear_equipment_scene();
    return ok;
}

int test_wear_armor(uint16_t i)
{
    struct Monster *monster = put_equipment_scene();
    uint8_t potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    uint8_t armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));

    // a `*` lista csak a páncélokat mutatja
    message_clear();
    bool ok = show_pack_list(PACK_FILTER_ARMOR) && does_message_bar_start_with("b) leather armor");
    uint8_t other_armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));

    // nem páncélra időbe kerül, de üres kör nélkül
    message_clear();
    ok = ok && wear_armor(potion) && does_message_bar_start_with("You can't wear that.") && game.worn_armor == NO_ITEM;
    ok = ok && has_monster_stayed(monster);

    // a páncél egy üres kör után kerül fel, és azonosítódik
    message_clear();
    ok = ok && !(get_item(armor)->flags & ITEM_KNOWN);
    ok = ok && wear_armor(armor) && game.worn_armor == armor && (get_item(armor)->flags & ITEM_KNOWN);
    ok = ok && has_monster_stepped_once(monster) && does_message_bar_start_with("You are now wearing +0 leather");

    // ha már van rajta páncél, a másik nem kerül fel, és nem visz időt
    message_clear();
    ok = ok && !wear_armor(other_armor) && does_message_bar_start_with("You are already wearing some.") && game.worn_armor == armor;

    // ha a játékos az üres körben meghal, a parancs ott véget ér: a páncél nem kerül fel, és üzenet sincs
    game.worn_armor = NO_ITEM;
    kill_player(EQUIPMENT_TEST_MONSTER);
    message_clear();
    ok = ok && wear_armor(other_armor) && game.worn_armor == NO_ITEM && !(get_item(other_armor)->flags & ITEM_KNOWN);
    ok = ok && is_message_bar_empty();

    clear_equipment_scene();
    return ok;
}

#define WEAR_THEFT_TRIES_MAX  100               // ennyi felvételi próba alatt a nimfa biztosan talál (rögzített maggal)

// a `W` üres körében a játékos mellett álló nimfa ellopja a felveendő páncélt, mert az még nincs használatban (a védett páncél
// mágikus): a páncél nem kerül fel, és nincs már a csomagban (J34); amíg a nimfa mellé üt, a páncél felkerül, ilyenkor újra le
int test_wear_stolen_armor(uint16_t i)
{
    put_equipment_scene();
    uint8_t armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));
    get_item(armor)->flags |= ITEM_PROTECTED;
    create_monster('N', EQUIPMENT_PLAYER_X + 1, EQUIPMENT_PLAYER_Y);
    // a rekord címe a táblából (README, Oscar64-hibák, 8.)
    start_monster_running(&game.monsters[game.monster_count - 1]);
    game.seed = 1;

    uint8_t tries = 0;
    while (game.monster_count == 2 && tries < WEAR_THEFT_TRIES_MAX) {
        game.worn_armor = NO_ITEM;
        wear_armor(armor);
        tries++;
    }
    bool ok = game.monster_count == 1 && game.worn_armor == NO_ITEM && !is_item_in_pack(armor);

    clear_equipment_scene();
    return ok;
}

int test_take_off_armor(uint16_t i)
{
    struct Monster *monster = put_equipment_scene();
    uint8_t armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));

    // ha nincs rajta páncél, nem visz időt
    message_clear();
    bool ok = !take_off_armor() && does_message_bar_start_with("You aren't wearing any armor.");

    // az átkozott páncél rajta marad; időbe kerül, de üres kör nélkül
    game.worn_armor = armor;
    get_item(armor)->flags |= ITEM_CURSED;
    message_clear();
    ok = ok && take_off_armor() && does_message_bar_start_with("You can't. It appears to be cursed.") && game.worn_armor == armor;
    ok = ok && has_monster_stayed(monster);

    // a páncél egy üres kör után kerül le
    get_item(armor)->flags &= ~ITEM_CURSED;
    message_clear();
    ok = ok && take_off_armor() && game.worn_armor == NO_ITEM && does_message_bar_start_with("You used to be wearing a) leather");
    ok = ok && has_monster_stepped_once(monster);

    // ha a játékos az üres körben meghal, a parancs ott véget ér: a páncél rajta marad, és üzenet sincs
    game.worn_armor = armor;
    kill_player(EQUIPMENT_TEST_MONSTER);
    message_clear();
    ok = ok && take_off_armor() && game.worn_armor == armor && is_message_bar_empty();

    clear_equipment_scene();
    return ok;
}

#define EQUIPMENT_TEST_POTION_COUNT  3          // a kézben tartott bájitalköteg darabszáma

// a játékos mezőjére lerakott tárgy; lekerül a padlóról, hogy a következő lerakásnak is legyen helye; NO_ITEM, ha nincs ott tárgy
static uint8_t take_dropped_item(void)
{
    uint8_t index = find_floor_item_at(EQUIPMENT_PLAYER_X, EQUIPMENT_PLAYER_Y);
    if (index != NO_ITEM) {
        take_item_from_floor(index);
    }
    return index;
}

int test_drop_item_in_use(uint16_t i)
{
    struct Monster *monster = put_equipment_scene();
    uint8_t mace = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, WEAPON_MACE));
    uint8_t potion = NO_ITEM;
    for (uint8_t count = 0; count < EQUIPMENT_TEST_POTION_COUNT; count++) {
        potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    }
    uint8_t armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));

    // az átkozott fegyver a kézben marad; időbe kerül, de üres kör nélkül
    game.wielded_item = mace;
    get_item(mace)->flags |= ITEM_CURSED;
    message_clear();
    bool ok = drop_item(mace) && does_message_bar_start_with("You can't. It appears to be cursed.");
    ok = ok && game.wielded_item == mace && is_item_in_pack(mace) && take_dropped_item() == NO_ITEM;

    // különben a kéz kiürül, és a fegyver lekerül
    get_item(mace)->flags &= ~ITEM_CURSED;
    ok = ok && drop_item(mace) && game.wielded_item == NO_ITEM && take_dropped_item() == mace;

    // a kézben tartott kötegből egy darab kerül le, és a kéz akkor is kiürül
    game.wielded_item = potion;
    ok = ok && drop_item(potion) && game.wielded_item == NO_ITEM && get_item(potion)->count == EQUIPMENT_TEST_POTION_COUNT - 1;
    uint8_t piece = take_dropped_item();
    ok = ok && piece != NO_ITEM && piece != potion;

    // ha a darabolásnak nincs helye, a köteg a kézben marad, és nem visz időt
    game.wielded_item = potion;
    while (create_item(ITEM_TYPE_FOOD, FOOD_RATION) != NO_ITEM) {}
    message_clear();
    ok = ok && !drop_item(potion) && does_message_bar_start_with("There's no room for that.");
    ok = ok && game.wielded_item == potion && get_item(potion)->count == EQUIPMENT_TEST_POTION_COUNT - 1;

    // eddig egyik lerakás sem járt üres körrel; a viselt páncél egy üres kör után kerül le
    ok = ok && has_monster_stayed(monster);
    game.worn_armor = armor;
    message_clear();
    ok = ok && drop_item(armor) && game.worn_armor == NO_ITEM && has_monster_stepped_once(monster);
    ok = ok && does_message_bar_start_with("Dropped leather armor.") && take_dropped_item() == armor;

    // ha a játékos az üres körben meghal, a parancs ott véget ér: a páncél rajta és a csomagban marad, üzenet sincs
    add_item_to_pack(armor);
    game.worn_armor = armor;
    kill_player(EQUIPMENT_TEST_MONSTER);
    message_clear();
    ok = ok && drop_item(armor) && game.worn_armor == armor && is_item_in_pack(armor) && take_dropped_item() == NO_ITEM;
    ok = ok && is_message_bar_empty();

    clear_equipment_scene();
    return ok;
}

#define STARTING_PACK_ITEM_COUNT  5             // étel, gyűrűs ing, buzogány, rövid íj, nyílköteg

// a kezdőfelszerelés fegyvere: a típusa és a bónuszai, azonosítva és átok nélkül
static bool is_starting_weapon(const struct Item *item, uint8_t which, int8_t hit_bonus, int8_t damage_bonus)
{
    return item->type == ITEM_TYPE_WEAPON && item->which == which && item->flags == ITEM_KNOWN &&
        item->properties.weapon.hit_bonus == hit_bonus && item->properties.weapon.damage_bonus == damage_bonus;
}

int test_starting_equipment(uint16_t i)
{
    clear_item_table();
    game.seed = i + 1;
    create_starting_equipment();

    // a betűk a csomag sorrendjében: étel, gyűrűs ing, buzogány, rövid íj, nyílköteg
    uint8_t items[STARTING_PACK_ITEM_COUNT];
    for (uint8_t position = 0; position < STARTING_PACK_ITEM_COUNT; position++) {
        items[position] = find_pack_item_by_letter(FIRST_PACK_LETTER + position);
    }
    bool ok = is_item_list(game.first_pack_item, items, STARTING_PACK_ITEM_COUNT);

    const struct Item *food = get_item(items[0]);
    const struct Item *armor = get_item(items[1]);
    const struct Item *arrows = get_item(items[4]);
    ok = ok &&
        food->type == ITEM_TYPE_FOOD && food->which == FOOD_RATION && food->count == 1 &&
        armor->type == ITEM_TYPE_ARMOR && armor->which == ARMOR_RING_MAIL && armor->flags == ITEM_KNOWN &&
        armor->properties.armor_class == get_armor_base_class(ARMOR_RING_MAIL) - STARTING_ARMOR_ENCHANTMENT &&
        is_starting_weapon(get_item(items[2]), WEAPON_MACE, STARTING_MACE_HIT_BONUS, STARTING_MACE_DAMAGE_BONUS) &&
        is_starting_weapon(get_item(items[3]), WEAPON_SHORT_BOW, STARTING_BOW_HIT_BONUS, 0) &&
        is_starting_weapon(arrows, WEAPON_ARROW, 0, 0) && arrows->group != 0 &&
        arrows->count >= STARTING_ARROW_COUNT_MIN && arrows->count < STARTING_ARROW_COUNT_MIN + STARTING_ARROW_COUNT_RANGE;

    // a gyűrűs ing viselve, a buzogány kézben
    ok = ok && game.worn_armor == items[1] && game.wielded_item == items[2];

    clear_item_table();
    return ok;
}

int test_report_items_in_use(uint16_t i)
{
    clear_item_table();
    message_clear();
    report_wielded_item();
    bool ok = does_message_bar_start_with("You are wielding nothing.");
    message_clear();
    report_worn_armor();
    ok = ok && does_message_bar_start_with("You are wearing nothing.");

    // a betű és a név; a név után közvetlenül a pont áll, vagyis nincs utótag
    game.worn_armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, EQUIPMENT_TEST_ARMOR));
    game.wielded_item = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, WEAPON_MACE));
    message_clear();
    report_wielded_item();
    ok = ok && does_message_bar_start_with("You are wielding (b) a mace.");
    message_clear();
    report_worn_armor();
    ok = ok && does_message_bar_start_with("You are wearing (a) leather armor.");

    message_clear();
    clear_item_table();
    return ok;
}
