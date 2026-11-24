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
#define EQUIPMENT_TEST_WEAPON   0               // mace
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
    uint8_t mace = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, EQUIPMENT_TEST_WEAPON));
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
