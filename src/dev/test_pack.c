#include "tests.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../platform/key.h"
#include "../item_names.h"
#include "../items.h"
#include "../map.h"
#include "../message.h"
#include "../monsters.h"
#include "../move.h"
#include "../pack.h"
#include "../player.h"
#include "../rooms.h"
#include "../state.h"

#define PACK_TEST_POTION_COUNT  3               // ennyi egyforma bájital egy tárgy a csomagban, és ennyi helyet foglal
#define PACK_TEST_ARROW_COUNT   25              // egy ekkora nyílköteg egy helyet foglal

// a bájitalok és a nyílköteg után marad még hely a tekercseknek
#if MAX_PACK < PACK_TEST_POTION_COUNT + 2
    #error "a csomagteszthez kevés a csomag helye"
#endif

int test_pack_slots(uint16_t i)
{
    clear_item_table();
    game.seed = 1;

    // egyforma bájitalok: egy tárgy, darabonként egy hely; a csatolt rekordok felszabadulnak
    uint8_t potion = NO_ITEM;
    for (uint8_t count = 0; count < PACK_TEST_POTION_COUNT; count++) {
        uint8_t index = create_item(ITEM_TYPE_POTION, 0);
        if (!has_pack_room_for(index)) return false;
        potion = add_item_to_pack(index);
    }
    if (get_item(potion)->count != PACK_TEST_POTION_COUNT) return false;
    if (calculate_pack_slots_used() != PACK_TEST_POTION_COUNT || game.item_count != 1) return false;

    // a nyílköteg egy hely
    uint8_t arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    get_item(arrows)->count = PACK_TEST_ARROW_COUNT;
    if (add_item_to_pack(arrows) != arrows) return false;
    if (calculate_pack_slots_used() != PACK_TEST_POTION_COUNT + 1) return false;

    // a többi hely egyforma tekercsekkel telik meg, utána új tárgy nem fér el
    while (calculate_pack_slots_used() < MAX_PACK) {
        uint8_t index = create_item(ITEM_TYPE_SCROLL, 0);
        if (!has_pack_room_for(index)) return false;
        add_item_to_pack(index);
    }
    uint8_t extra_potion = create_item(ITEM_TYPE_POTION, 0);
    if (has_pack_room_for(extra_potion)) return false;

    // a meglévő nyílköteghez csatolt nyilak még beférnek, és a köteg nő
    uint8_t more_arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    get_item(more_arrows)->group = get_item(arrows)->group;
    uint8_t more_count = get_item(more_arrows)->count;
    if (!has_pack_room_for(more_arrows)) return false;
    if (add_item_to_pack(more_arrows) != arrows) return false;
    if (get_item(arrows)->count != PACK_TEST_ARROW_COUNT + more_count) return false;
    if (calculate_pack_slots_used() != MAX_PACK) return false;

    // egy másik nyílköteg már nem
    uint8_t other_arrows = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    bool other_fits = has_pack_room_for(other_arrows);

    clear_item_table();
    return !other_fits;
}

#define PACK_TEST_LETTER_ITEM_COUNT  4          // ennyi különböző tárgy kerül a csomagba a betűk tesztjében
#define PACK_TEST_REMOVED_POSITION   1          // közülük ez (0-tól számolva) kerül ki

int test_pack_letters(uint16_t i)
{
    clear_item_table();

    // különböző altípusú bájitalok: a betűik sorban az elsőtől
    uint8_t items[PACK_TEST_LETTER_ITEM_COUNT];
    for (uint8_t position = 0; position < PACK_TEST_LETTER_ITEM_COUNT; position++) {
        items[position] = add_item_to_pack(create_item(ITEM_TYPE_POTION, position));
        if (get_item(items[position])->location.pack_letter != FIRST_PACK_LETTER + position) return false;
    }

    // egy tárgy kikerül: a többi betűje nem változik
    remove_item_from_list(&game.first_pack_item, items[PACK_TEST_REMOVED_POSITION]);
    free_item(items[PACK_TEST_REMOVED_POSITION]);
    for (uint8_t position = 0; position < PACK_TEST_LETTER_ITEM_COUNT; position++) {
        if (position == PACK_TEST_REMOVED_POSITION) continue;
        if (get_item(items[position])->location.pack_letter != FIRST_PACK_LETTER + position) return false;
    }

    // az új tárgy a legkisebb szabad betűt kapja: előbb a felszabadultat, utána az eddig nem használt következőt
    uint8_t refill = add_item_to_pack(create_item(ITEM_TYPE_POTION, PACK_TEST_LETTER_ITEM_COUNT));
    if (get_item(refill)->location.pack_letter != FIRST_PACK_LETTER + PACK_TEST_REMOVED_POSITION) return false;
    uint8_t next = add_item_to_pack(create_item(ITEM_TYPE_POTION, PACK_TEST_LETTER_ITEM_COUNT + 1));
    bool is_next_letter_right = get_item(next)->location.pack_letter == FIRST_PACK_LETTER + PACK_TEST_LETTER_ITEM_COUNT;

    clear_item_table();
    return is_next_letter_right;
}

int test_pack_order(uint16_t i)
{
    clear_item_table();

    // két-két altípus bájitalból, tekercsből és páncélból; a páncél nem kötegel, így két egyforma két tárgy
    uint8_t first_potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    uint8_t first_scroll = add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 0));
    uint8_t second_potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 1));
    uint8_t first_armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 0));
    uint8_t second_scroll = add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 1));
    uint8_t second_armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 1));
    uint8_t first_armor_again = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 0));

    // új típus a végére, új altípus az azonos típusúak után, ismétlődő altípus az azonos altípusúak után
    const uint8_t expected[] = { first_potion, second_potion, first_scroll, second_scroll, first_armor, first_armor_again, second_armor };
    if (!is_item_list(game.first_pack_item, expected, sizeof(expected))) return false;

    // a köteghez csatolt bájital nem változtat a sorrenden
    if (add_item_to_pack(create_item(ITEM_TYPE_POTION, 0)) != first_potion) return false;
    bool is_order_kept = is_item_list(game.first_pack_item, expected, sizeof(expected)) && get_item(first_potion)->count == 2;

    clear_item_table();
    return is_order_kept;
}

// a felvétel tesztmezői: a játékos egy folyosó-mezőn, jobbra mellette egy folyosó- és egy padlómező
#define PICKUP_PLAYER_X        10
#define PICKUP_PLAYER_Y        (MAP_TOP_ROW + 2)
#define PICKUP_PASSAGE_NUMBER  1

#define PICKUP_TEST_GOLD_START   100            // ennyi aranya van a játékosnak a felvétel előtt
#define PICKUP_TEST_GOLD_AMOUNT  25             // ennyi arany fekszik a padlón

static void put_pickup_passage_tile(uint8_t x, uint8_t y)
{
    set_map_character(x, y, CHAR_PASSAGE);
    add_map_flags(x, y, TILE_PASSAGE | PICKUP_PASSAGE_NUMBER);
}

// a kezdőértékű játékos a tesztmezőjén, szörnyek nélkül; üres a padló és a csomag
static void put_pickup_scene(void)
{
    clear_map();
    clear_monsters();
    clear_item_table();
    init_player();

    put_pickup_passage_tile(PICKUP_PLAYER_X, PICKUP_PLAYER_Y);
    put_pickup_passage_tile(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    set_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y, CHAR_FLOOR);
    game.player.x = PICKUP_PLAYER_X;
    game.player.y = PICKUP_PLAYER_Y;
    game.player.room = get_room_at(PICKUP_PLAYER_X, PICKUP_PLAYER_Y);
}

static void clear_pickup_scene(void)
{
    clear_item_table();
    clear_map();
}

int test_pickup_by_step(uint16_t i)
{
    put_pickup_scene();
    uint8_t potion = create_item(ITEM_TYPE_POTION, 0);
    put_item_on_floor(potion, PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    uint8_t scroll = create_item(ITEM_TYPE_SCROLL, 0);
    put_item_on_floor(scroll, PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y);

    // futva a tárgyra lépve felveszi, és a futás megáll; a futás lépése a move_player, mint a fogva tartás tesztjében (test_combat.c)
    start_running(1, 0, false);
    bool ok =
        move_player(1, 0) &&
        !is_running() &&
        find_floor_item_at(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == CHAR_PASSAGE &&
        game.first_pack_item == potion &&
        get_item(potion)->location.pack_letter == FIRST_PACK_LETTER;

    // az `m` lépése a tárgyat a padlón hagyja, a jelével együtt
    ok = ok &&
        move_player_without_pickup(1, 0) &&
        game.player.x == PICKUP_PLAYER_X + 2 &&
        find_floor_item_at(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == scroll &&
        get_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == CHAR_SCROLL &&
        get_item(potion)->next == NO_ITEM;

    // a `,` felveszi, és a padlómező jele újra padló
    pick_up_item(scroll);
    const uint8_t expected[] = { potion, scroll };
    ok = ok &&
        game.first_floor_item == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == CHAR_FLOOR &&
        is_item_list(game.first_pack_item, expected, sizeof(expected));

    clear_pickup_scene();
    return ok;
}

int test_pickup_gold(uint16_t i)
{
    put_pickup_scene();
    game.gold = PICKUP_TEST_GOLD_START;
    uint8_t gold = create_item(ITEM_TYPE_GOLD, 0);
    get_item(gold)->properties.gold_amount = PICKUP_TEST_GOLD_AMOUNT;
    put_item_on_floor(gold, PICKUP_PLAYER_X, PICKUP_PLAYER_Y);

    pick_up_item(gold);
    bool ok =
        game.gold == PICKUP_TEST_GOLD_START + PICKUP_TEST_GOLD_AMOUNT &&
        game.item_count == 0 &&
        game.first_floor_item == NO_ITEM &&
        game.first_pack_item == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X, PICKUP_PLAYER_Y) == CHAR_PASSAGE;

    clear_pickup_scene();
    return ok;
}

int test_pickup_full_pack(uint16_t i)
{
    put_pickup_scene();
    while (calculate_pack_slots_used() < MAX_PACK) {
        add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 0));
    }
    uint8_t potion = create_item(ITEM_TYPE_POTION, 0);
    put_item_on_floor(potion, PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    uint8_t item_count = game.item_count;

    bool ok =
        move_player(1, 0) &&
        game.player.x == PICKUP_PLAYER_X + 1 &&
        find_floor_item_at(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == potion &&
        get_map_character(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == CHAR_POTION &&
        calculate_pack_slots_used() == MAX_PACK &&
        game.item_count == item_count;

    clear_pickup_scene();
    return ok;
}

#define LEVITATION_TEST_TURNS  1000             // a lebegés ideje: a teszt alatt nem jár le (az időzítők itt nem ketyegnek)

int test_pickup_levitating(uint16_t i)
{
    put_pickup_scene();
    uint8_t potion = create_item(ITEM_TYPE_POTION, 0);
    put_item_on_floor(potion, PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);
    uint8_t scroll = create_item(ITEM_TYPE_SCROLL, 0);
    put_item_on_floor(scroll, PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y);
    game.timers[TIMER_LEVITATION] = LEVITATION_TEST_TURNS;
    message_clear();

    // futva a tárgyra lépve a futás megáll, de a tárgy a padlón marad, és nincs üzenet
    start_running(1, 0, false);
    bool ok =
        move_player(1, 0) &&
        !is_running() &&
        game.player.x == PICKUP_PLAYER_X + 1 &&
        find_floor_item_at(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == potion &&
        get_map_character(PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y) == CHAR_POTION &&
        game.first_pack_item == NO_ITEM &&
        is_message_bar_empty();

    // az `m` lépése a „moved onto” üzenetet sem adja
    ok = ok &&
        move_player_without_pickup(1, 0) &&
        game.player.x == PICKUP_PLAYER_X + 2 &&
        find_floor_item_at(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == scroll &&
        is_message_bar_empty();

    game.timers[TIMER_LEVITATION] = 0;
    clear_pickup_scene();
    return ok;
}

// a felvétel tesztmezőin, a csomagban egy PACK_TEST_POTION_COUNT darabos bájitalköteg; az eredmény a köteg
static uint8_t put_drop_scene(void)
{
    put_pickup_scene();
    uint8_t potion = NO_ITEM;
    for (uint8_t count = 0; count < PACK_TEST_POTION_COUNT; count++) {
        potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    }
    return potion;
}

int test_drop_stack_piece(uint16_t i)
{
    uint8_t potion = put_drop_scene();
    uint8_t arrows = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW));
    uint8_t arrow_count = get_item(arrows)->count;

    // a bájitalkötegből egy darab, új rekordként; a köteg a csomagban marad
    bool ok = drop_item(potion);
    uint8_t dropped = find_floor_item_at(PICKUP_PLAYER_X, PICKUP_PLAYER_Y);
    ok = ok &&
        dropped != NO_ITEM &&
        dropped != potion &&
        get_item(dropped)->type == ITEM_TYPE_POTION &&
        get_item(dropped)->count == 1 &&
        get_map_character(PICKUP_PLAYER_X, PICKUP_PLAYER_Y) == CHAR_POTION &&
        get_item(potion)->count == PACK_TEST_POTION_COUNT - 1 &&
        is_item_in_pack(potion);

    // a nyílköteg egészben kerül le, és kikerül a csomagból
    game.player.x = PICKUP_PLAYER_X + 2;
    ok = ok &&
        drop_item(arrows) &&
        find_floor_item_at(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == arrows &&
        get_item(arrows)->count == arrow_count &&
        get_map_character(PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y) == CHAR_WEAPON &&
        !is_item_in_pack(arrows);

    clear_pickup_scene();
    return ok;
}

int test_drop_full_table(uint16_t i)
{
    uint8_t potion = put_drop_scene();
    uint8_t scroll = add_item_to_pack(create_item(ITEM_TYPE_SCROLL, 0));
    // a tárgytábla többi helye is foglalt
    while (create_item(ITEM_TYPE_FOOD, FOOD_RATION) != NO_ITEM) {}

    // a köteg darabolásához nincs hely: semmi nem változik
    bool ok =
        game.item_count == MAX_ITEMS &&
        !drop_item(potion) &&
        get_item(potion)->count == PACK_TEST_POTION_COUNT &&
        game.first_floor_item == NO_ITEM &&
        get_map_character(PICKUP_PLAYER_X, PICKUP_PLAYER_Y) == CHAR_PASSAGE &&
        game.item_count == MAX_ITEMS;

    // egy egész tárgy így is lerakható
    ok = ok &&
        drop_item(scroll) &&
        game.first_floor_item == scroll &&
        !is_item_in_pack(scroll);

    clear_pickup_scene();
    return ok;
}

int test_amulet_carried(uint16_t i)
{
    put_pickup_scene();
    add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    uint8_t amulet = create_item(ITEM_TYPE_AMULET, 0);
    put_item_on_floor(amulet, PICKUP_PLAYER_X + 1, PICKUP_PLAYER_Y);

    // a padlón fekvő amulett nincs a játékosnál, felvéve nála van, lerakva újra nincs
    bool ok = !is_carrying_amulet();
    ok = ok &&
        move_player(1, 0) &&
        is_carrying_amulet() &&
        drop_item(amulet) &&
        !is_carrying_amulet();

    clear_pickup_scene();
    return ok;
}

// a névkészlet tesztjeihez: a felvétel tesztmezője, és új játékként minden típus ismeretlen, a névkészlet üres
static void put_naming_scene(void)
{
    put_pickup_scene();
    game.seed = 1;
    init_item_names();
}

int test_label_freed_with_item(uint16_t i)
{
    put_naming_scene();

    // az új tárgynak nincs címkéje, a felszabadított tárgy címkéje is felszabadul
    uint8_t mace = create_item(ITEM_TYPE_WEAPON, 0);
    bool ok = get_item(mace)->label == NO_NAME;
    uint8_t label = store_name("x");
    get_item(mace)->label = label;
    free_item(mace);
    ok = ok && store_name("y") == label;

    // a szintváltáskor a padlón maradt tárgy címkéje is
    uint8_t armor = create_item(ITEM_TYPE_ARMOR, 0);
    get_item(armor)->label = label;
    put_item_on_floor(armor, PICKUP_PLAYER_X + 2, PICKUP_PLAYER_Y);
    free_item_list(&game.first_floor_item);
    ok = ok && game.item_count == 0 && store_name("z") == label;

    clear_pickup_scene();
    return ok;
}

int test_label_merge(uint16_t i)
{
    put_naming_scene();

    // a csomagban egy címke nélküli nyílköteg, a padlóról két, belőle kivált nyíl jön vissza, mindkettő címkével
    uint8_t stack = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW));
    uint8_t first_piece = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    uint8_t second_piece = create_item(ITEM_TYPE_WEAPON, WEAPON_ARROW);
    get_item(first_piece)->group = get_item(stack)->group;
    get_item(second_piece)->group = get_item(stack)->group;
    uint8_t first_label = store_name("x");
    uint8_t second_label = store_name("y");
    get_item(first_piece)->label = first_label;
    get_item(second_piece)->label = second_label;

    // a címke nélküli köteg átveszi a beolvadó tárgy címkéjét
    bool ok = add_item_to_pack(first_piece) == stack && get_item(stack)->label == first_label && strcmp(get_name(first_label), "x") == 0;
    // a címkés kötegé marad, a beolvadó tárgyé felszabadul
    ok = ok && add_item_to_pack(second_piece) == stack && get_item(stack)->label == first_label;
    ok = ok && game.item_count == 1 && store_name("z") == second_label;

    clear_pickup_scene();
    return ok;
}

int test_pack_nameable_list(uint16_t i)
{
    put_naming_scene();
    add_item_to_pack(create_item(ITEM_TYPE_FOOD, FOOD_RATION));
    add_item_to_pack(create_item(ITEM_TYPE_AMULET, 0));

    // az étel és az amulett nem kerül a listába
    message_clear();
    bool ok = !show_pack_list(PACK_FILTER_NAMEABLE) && does_message_bar_start_with("You don't have anything appropriate.");

    // az egyetlen elnevezhető tárgy listasora üzenetként jelenik meg
    uint8_t potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    char row_start[] = { get_item(potion)->location.pack_letter, ')', ' ', 0 };
    message_clear();
    ok = ok && show_pack_list(PACK_FILTER_NAMEABLE) && does_message_bar_start_with(row_start);

    message_clear();
    clear_pickup_scene();
    return ok;
}

// az elnevezés leütései; a törlés oktális kóddal, hogy az utána álló betű ne olvadjon bele
#define NAMING_KEY_DELETE  "\177"
#define NAMING_KEY_ESCAPE  "\033"
#define NAMING_KEY_RETURN  "\r"

// a tárgy elnevezése a megadott leütésekkel, üres sávval kezdve, mint a játékban a parancs leütése után; igaz, ha a leütések mind
// elfogytak, és nem maradt több
static bool name_item_with_keys(uint8_t index, const char *keys)
{
    message_clear();
    set_test_keys(keys);
    name_item(index);
    bool ok = read_key() == 0;
    set_test_keys(NULL);
    return ok;
}

// a típus elnevezése: a mező üresen indul, RETURN után a sáv üres; az átnevezés nem foglal új helyet, ESC-re a név marad,
// üres névre törlődik
static bool is_potion_naming_right(uint8_t potion)
{
    uint8_t which = get_item(potion)->which;

    // név nélkül üres beírásra nem kap nevet
    bool ok = name_item_with_keys(potion, NAMING_KEY_RETURN) && is_message_bar_empty();
    ok = ok && game.potion_knowledge[which] == SUBTYPE_UNKNOWN;

    // a beírt szöveg a típus neve lesz
    ok = ok && name_item_with_keys(potion, "ab" NAMING_KEY_RETURN) && is_message_bar_empty();
    uint8_t name_index = game.potion_knowledge[which];
    ok = ok && name_index != NO_NAME && name_index != SUBTYPE_IDENTIFIED && strcmp(get_name(name_index), "ab") == 0;

    // átnevezés javított elírással: a régi név előtte üzenetben (a sáv RETURN után üres, ezért az utolsó üzenet visszahívásával),
    // és a hely marad
    ok = ok && name_item_with_keys(potion, "rex" NAMING_KEY_DELETE "d" NAMING_KEY_RETURN);
    repeat_last_message();
    ok = ok && does_message_bar_start_with("Was called \"ab\".");
    ok = ok && game.potion_knowledge[which] == name_index && strcmp(get_name(name_index), "red") == 0;

    // ESC-re a név marad
    ok = ok && name_item_with_keys(potion, "x" NAMING_KEY_ESCAPE);
    ok = ok && game.potion_knowledge[which] == name_index && strcmp(get_name(name_index), "red") == 0;

    // üres névre a név törlődik, és a helye felszabadul
    ok = ok && name_item_with_keys(potion, NAMING_KEY_RETURN);
    ok = ok && does_message_bar_start_with("You remove the name.");
    ok = ok && game.potion_knowledge[which] == SUBTYPE_UNKNOWN && store_name("y") == name_index;
    free_name(name_index);

    // az azonosított típus nem kap nevet
    game.potion_knowledge[which] = SUBTYPE_IDENTIFIED;
    ok = ok && name_item_with_keys(potion, "") && does_message_bar_start_with("That has already been identified.");
    ok = ok && game.potion_knowledge[which] == SUBTYPE_IDENTIFIED;
    game.potion_knowledge[which] = SUBTYPE_UNKNOWN;
    return ok;
}

int test_name_item(uint16_t i)
{
    put_naming_scene();
    uint8_t potion = add_item_to_pack(create_item(ITEM_TYPE_POTION, 0));
    uint8_t mace = add_item_to_pack(create_item(ITEM_TYPE_WEAPON, 0));
    uint8_t armor = add_item_to_pack(create_item(ITEM_TYPE_ARMOR, 0));
    uint8_t food = add_item_to_pack(create_item(ITEM_TYPE_FOOD, FOOD_RATION));
    uint8_t amulet = add_item_to_pack(create_item(ITEM_TYPE_AMULET, 0));

    bool ok = is_potion_naming_right(potion);

    // a fegyver saját címkét kap; üres névre nem kap
    ok = ok && name_item_with_keys(mace, NAMING_KEY_RETURN) && get_item(mace)->label == NO_NAME;
    ok = ok && name_item_with_keys(mace, "sword" NAMING_KEY_RETURN) && strcmp(get_name(get_item(mace)->label), "sword") == 0;

    // a mező legfeljebb NAME_LENGTH_MAX karaktert fogad, a többi leütés elvész
    char long_name_keys[NAME_LENGTH_MAX + 3];
    memset(long_name_keys, 'n', NAME_LENGTH_MAX + 1);
    long_name_keys[NAME_LENGTH_MAX + 1] = '\r';
    long_name_keys[NAME_LENGTH_MAX + 2] = 0;
    ok = ok && name_item_with_keys(mace, long_name_keys) && strlen(get_name(get_item(mace)->label)) == NAME_LENGTH_MAX;

    // étel és amulett nem kap nevet
    ok = ok && name_item_with_keys(food, "") && does_message_bar_start_with("You can't call that anything.");
    ok = ok && name_item_with_keys(amulet, "") && does_message_bar_start_with("You can't call that anything.");

    // betelt névkészletnél a még név nélküli páncél nem kap nevet, a címkés fegyver átnevezhető
    while (store_name("f") != NO_NAME) {
    }
    ok = ok && name_item_with_keys(armor, "") && does_message_bar_start_with("You have no room for more names.");
    ok = ok && get_item(armor)->label == NO_NAME;
    ok = ok && name_item_with_keys(mace, "axe" NAMING_KEY_RETURN);
    ok = ok && strcmp(get_name(get_item(mace)->label), "axe") == 0;

    message_clear();
    clear_pickup_scene();
    return ok;
}
