#include "tests.h"
#include "checks.h"

#include <stdbool.h>
#include <string.h>
#include <c64/vic.h>

#include "../chase.h"
#include "../explore.h"
#include "../items.h"
#include "../level.h"
#include "../map.h"
#include "../message.h"
#include "../monsters.h"
#include "../move.h"
#include "../pack.h"
#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../rng.h"
#include "../rooms.h"
#include "../state.h"
#include "../view.h"

// a szint-önteszt mélységei (LEVEL_TEST_DEPTH_COUNT, tests.h): a próbák sorban ezeken mennek körbe
static const uint8_t level_test_depths[LEVEL_TEST_DEPTH_COUNT] = { 1, 10, 26, 40 };

#define LEVEL_TEST_FIRST_SEED  1                // az i. próba magja LEVEL_TEST_FIRST_SEED + i

// a rögzített magokkal az arányok mindig ugyanazok; a tűrés csak azt mutatja, mennyire térhetnek el a várttól
#define DARK_RATIO_TOLERANCE_PERCENT  5         // a sötét (és labirintus) szobák aránya ennyi százalékponttal térhet el
#define MAZE_RATIO_TOLERANCE_PERCENT  30        // a labirintusok száma a várt érték ennyi százalékával térhet el

// mélységenként: a nem fantom szobák, ebből a sötétek (a labirintussal együtt, mert az is sötétként indul), ebből a labirintusok
static uint16_t level_room_counts[LEVEL_TEST_DEPTH_COUNT];
static uint16_t level_dark_counts[LEVEL_TEST_DEPTH_COUNT];
static uint16_t level_maze_counts[LEVEL_TEST_DEPTH_COUNT];

// hány szint futott le; ESC-re a teszt LEVEL_TEST_LEVEL_COUNT előtt leáll
static uint16_t level_test_run_count;

// mélységenként a leglassabb generate_level képkockában
static uint16_t level_max_frames[LEVEL_TEST_DEPTH_COUNT];

#define LEVEL_TIME_LIMIT_FRAMES  150            // egy szint legfeljebb 3 másodperc: PAL-on 50 képkocka/s (NTSC-n 60, ott ez szigorúbb)

// a szoba nem lóg ki a szektorából, és a térkép sorain belül van (a 0. és a 23. logikai sorba nem ér);
// a normál és a fantom szoba a szektor utolsó oszlopát és sorát üresen hagyja; a labirintus legtávolabbi mezője
// a helye + a mérete páros része, és a szektor utolsó soráig is érhet (a felső szektorsorban), a bal szélső a 0. oszloptól kezdődik
static bool is_room_inside_sector(uint8_t room_index)
{
    const struct Room *room = &game.rooms[room_index];
    uint8_t corner_x = room_index % SECTOR_GRID_SIZE * SECTOR_WIDTH + SECTOR_FIRST_COLUMN;
    uint8_t corner_y = room_index / SECTOR_GRID_SIZE * SECTOR_HEIGHT;
    uint8_t first_x = corner_x;
    uint8_t end_x = corner_x + SECTOR_WIDTH - 1;    // az első oszlop, ahova a szoba már nem ér
    uint8_t end_y = corner_y + SECTOR_HEIGHT - 1;
    uint8_t last_x = room->x;
    uint8_t last_y = room->y;

    if (room->flags & ROOM_MAZE) {
        if (corner_x == SECTOR_FIRST_COLUMN) {
            first_x = 0;
        }
        end_x++;
        end_y++;
        last_x = room->x + room->width / 2 * 2;
        last_y = room->y + room->height / 2 * 2;
    } else if (!(room->flags & ROOM_PHANTOM)) {
        last_x = room->x + room->width - 1;
        last_y = room->y + room->height - 1;
    }

    if (room->x < first_x || last_x >= end_x) return false;
    if (room->y < corner_y || last_y >= end_y) return false;
    if (room->y < MAP_TOP_ROW || last_y >= MAP_TOP_ROW + MAP_ROWS) return false;
    return true;
}

// a labirintus cellái a bal felső cellából a járatokon át mind elérhetők: elárasztás a TILE_SEEN flaggel
// (a generálás nem használja), soronként oda-vissza, amíg terjed
static uint16_t count_reachable_maze_cells(const struct Room *room)
{
    uint8_t last_x = room->x + room->width / 2 * 2;
    uint8_t last_y = room->y + room->height / 2 * 2;
    uint16_t reached_count = 1;
    bool spread = true;

    add_map_flags(room->x, room->y, TILE_SEEN);

    while (spread) {
        spread = false;
        for (uint8_t y = room->y; y <= last_y; y += 2) {
            for (uint8_t x = room->x; x <= last_x; x += 2) {
                if (!(get_map_flags(x, y) & TILE_SEEN)) continue;

                // a négy szomszéd: a köztes mező járat, és a szomszéd még nincs jelölve
                if (x + 2 <= last_x && (get_map_flags(x + 1, y) & TILE_PASSAGE) && !(get_map_flags(x + 2, y) & TILE_SEEN)) {
                    add_map_flags(x + 2, y, TILE_SEEN);
                    reached_count++;
                    spread = true;
                }
                if (x >= room->x + 2 && (get_map_flags(x - 1, y) & TILE_PASSAGE) && !(get_map_flags(x - 2, y) & TILE_SEEN)) {
                    add_map_flags(x - 2, y, TILE_SEEN);
                    reached_count++;
                    spread = true;
                }
                if (y + 2 <= last_y && (get_map_flags(x, y + 1) & TILE_PASSAGE) && !(get_map_flags(x, y + 2) & TILE_SEEN)) {
                    add_map_flags(x, y + 2, TILE_SEEN);
                    reached_count++;
                    spread = true;
                }
                if (y >= room->y + 2 && (get_map_flags(x, y - 1) & TILE_PASSAGE) && !(get_map_flags(x, y - 2) & TILE_SEEN)) {
                    add_map_flags(x, y - 2, TILE_SEEN);
                    reached_count++;
                    spread = true;
                }
            }
        }
    }

    return reached_count;
}

// a labirintus egyetlen DFS-fa: minden cellája járat, két cella közti átlós mező soha, pontosan cellák - 1 összekötő mező van,
// és minden cella elérhető (összefüggő és cellák - 1 éle van, tehát fa)
static bool is_maze_tree(const struct Room *room)
{
    uint8_t last_x = room->x + room->width / 2 * 2;
    uint8_t last_y = room->y + room->height / 2 * 2;
    uint16_t cell_count = 0;
    uint16_t link_count = 0;

    for (uint8_t y = room->y; y <= last_y; y++) {
        for (uint8_t x = room->x; x <= last_x; x++) {
            bool is_passage = (get_map_flags(x, y) & TILE_PASSAGE) != 0;
            bool odd_x = ((x - room->x) & 1) != 0;
            bool odd_y = ((y - room->y) & 1) != 0;

            if (odd_x && odd_y) {
                if (is_passage) return false;
            } else if (!odd_x && !odd_y) {
                if (!is_passage) return false;
                cell_count++;
            } else if (is_passage) {
                link_count++;
            }
        }
    }

    if (link_count != cell_count - 1) return false;
    return count_reachable_maze_cells(room) == cell_count;
}

// a titkos ajtó és a titkos folyosó-mező rejtett (nincs TILE_REAL); a titkos ajtó nem folyosó-mező
static bool is_door_tile(uint8_t x, uint8_t y)
{
    uint8_t flags = get_map_flags(x, y);
    if (flags & TILE_PASSAGE) return false;
    return !(flags & TILE_REAL) || get_map_character(x, y) == CHAR_DOOR;
}

// az ajtó egy normál szoba falán van, sarok nélkül; a titkos ajtó jele a fal jele: felső és alsó falon vízszintes, oldalfalon függőleges
static bool is_door_on_room_wall(uint8_t x, uint8_t y)
{
    bool is_secret = !(get_map_flags(x, y) & TILE_REAL);
    uint8_t character = get_map_character(x, y);

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if (room->flags & (ROOM_PHANTOM | ROOM_MAZE)) continue;

        uint8_t right_x = room->x + room->width - 1;
        uint8_t bottom_y = room->y + room->height - 1;
        bool is_between_corners_x = x > room->x && x < right_x;
        bool is_between_corners_y = y > room->y && y < bottom_y;

        if (is_between_corners_x && (y == room->y || y == bottom_y)) {
            return !is_secret || character == CHAR_WALL_HORIZONTAL;
        }
        if (is_between_corners_y && (x == room->x || x == right_x)) {
            return !is_secret || character == CHAR_WALL_VERTICAL;
        }
    }
    return false;
}

// a teljes térkép: minden ajtó egy szoba falán, és az 1. szinten nincs rejtett mező; közben a TILE_SEEN törlődik az elárasztáshoz
static bool are_map_tiles_valid(void)
{
    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            remove_map_flags(x, y, TILE_SEEN);

            if (game.dungeon_level == 1 && !(get_map_flags(x, y) & TILE_REAL)) return false;
            if (is_door_tile(x, y) && !is_door_on_room_wall(x, y)) return false;
        }
    }
    return true;
}

// a labirintusban nincs ajtó: a mezői folyosó-mezők, üresek, vagy a lépcső vagy egy tárgy áll rajtuk; a kijárat is folyosó-mező marad
static bool is_maze_without_doors(const struct Room *room)
{
    uint8_t last_x = room->x + room->width / 2 * 2;
    uint8_t last_y = room->y + room->height / 2 * 2;

    for (uint8_t y = room->y; y <= last_y; y++) {
        for (uint8_t x = room->x; x <= last_x; x++) {
            uint8_t character = get_map_character(x, y);
            if (character == CHAR_PASSAGE || character == CHAR_SOLID_ROCK || character == CHAR_STAIRS) continue;
            if (!is_item_character(character)) return false;
        }
    }
    return true;
}

// a mező a labirintus területén van: a helyétől a mérete páros részéig
static bool is_inside_maze(const struct Room *room, uint8_t x, uint8_t y)
{
    uint8_t last_x = room->x + room->width / 2 * 2;
    uint8_t last_y = room->y + room->height / 2 * 2;
    return x >= room->x && x <= last_x && y >= room->y && y <= last_y;
}

// hány bejegyzés áll az ajtótáblában az x, y mezőn
static uint8_t count_exits_at(uint8_t x, uint8_t y)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < game.exit_count; i++) {
        if (game.exits[i].x == x && game.exits[i].y == y) {
            count++;
        }
    }
    return count;
}

// az ajtótábla: minden bejegyzés egy nem fantom szobáé; normál szobánál a szoba ajtaja (a titkos is), labirintusnál a labirintus folyosó-mezője;
// a térkép minden ajtója pontosan egy bejegyzés
static bool is_exit_table_valid(void)
{
    if (game.exit_count > MAX_EXITS) return false;

    for (uint8_t i = 0; i < game.exit_count; i++) {
        const struct Exit *entry = &game.exits[i];
        if (entry->room >= ROOM_COUNT) return false;

        const struct Room *room = &game.rooms[entry->room];
        if (room->flags & ROOM_PHANTOM) return false;

        if (room->flags & ROOM_MAZE) {
            if (!(get_map_flags(entry->x, entry->y) & TILE_PASSAGE)) return false;
            if (!is_inside_maze(room, entry->x, entry->y)) return false;
        } else {
            if (!is_door_tile(entry->x, entry->y)) return false;
            if (get_room_at(entry->x, entry->y) != entry->room) return false;
        }
    }

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            if (is_door_tile(x, y) && count_exits_at(x, y) != 1) return false;
        }
    }
    return true;
}

// a mező egy folyosórendszer része: folyosó-mező (a titkos és a labirintusé is) vagy ajtó (a titkos is)
static bool is_passage_system_tile(uint8_t x, uint8_t y)
{
    return (get_map_flags(x, y) & TILE_PASSAGE) || is_door_tile(x, y);
}

static uint8_t get_passage_number_at(uint8_t x, uint8_t y)
{
    return get_map_flags(x, y) & TILE_PASSAGE_NUMBER;
}

// a folyosórendszerek: minden folyosó-mező és ajtó sorszámot kap (1..MAX_PASSAGES), más mező nem; az ortogonális szomszédoké egyezik;
// a sorszámok 1-től hézag nélkül követik egymást; az ajtó bejegyzésében az ajtó sorszáma áll, a labirintus kijáratáéban semmi;
// a rendszerek rekordja fantom és sötét; a kitöltés munkaterülete, a látott térkép, a bejárt mezőkön újra üres
static bool are_passage_numbers_valid(void)
{
    uint16_t used_numbers = 0;                  // sorszámonként egy bit
    uint8_t highest_number = 0;

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            uint8_t number = get_passage_number_at(x, y);

            if (!is_passage_system_tile(x, y)) {
                if (number != NO_PASSAGE_NUMBER) return false;
                continue;
            }

            if (number == NO_PASSAGE_NUMBER || number > MAX_PASSAGES) return false;
            if (game.seen_map[y - MAP_TOP_ROW][x] != CHAR_SOLID_ROCK) return false;

            used_numbers |= (uint16_t)1 << number;
            if (number > highest_number) {
                highest_number = number;
            }

            // a jobb és az alsó szomszéd: így minden szomszédos pár egyszer kerül sorra
            if (x < MAP_COLUMNS - 1 && is_passage_system_tile(x + 1, y) && get_passage_number_at(x + 1, y) != number) return false;
            if (y < MAP_TOP_ROW + MAP_ROWS - 1 && is_passage_system_tile(x, y + 1) && get_passage_number_at(x, y + 1) != number) return false;
        }
    }

    // az 1..highest_number bitek mind be vannak kapcsolva, a 0. nem
    uint16_t expected_numbers = (uint16_t)(((uint16_t)1 << highest_number) - 1) << 1;
    if (used_numbers != expected_numbers) return false;

    for (uint8_t i = 0; i < game.exit_count; i++) {
        const struct Exit *entry = &game.exits[i];
        uint8_t expected_number = get_passage_number_at(entry->x, entry->y);
        if (game.rooms[entry->room].flags & ROOM_MAZE) {
            expected_number = NO_PASSAGE_NUMBER;
        }
        if (entry->passage_number != expected_number) return false;
    }

    for (uint8_t room_index = ROOM_COUNT; room_index < ROOM_TABLE_SIZE; room_index++) {
        if (game.rooms[room_index].flags != (ROOM_PHANTOM | ROOM_DARK)) return false;
    }
    return true;
}

// járható mező: padló, ajtó, lépcső, tárgy, folyosó-mező, a titkosakkal együtt
static bool is_walkable_tile(uint8_t x, uint8_t y)
{
    uint8_t flags = get_map_flags(x, y);
    if (flags & TILE_PASSAGE) return true;
    if (!(flags & TILE_REAL)) return true;

    uint8_t character = get_map_character(x, y);
    return character == CHAR_FLOOR || character == CHAR_DOOR || character == CHAR_STAIRS || is_item_character(character);
}

// a nem fantom szoba indexe, amelynek a falain belül a mező van, vagy ROOM_COUNT, ha nincs ilyen
static uint8_t get_room_around(uint8_t x, uint8_t y)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if (room->flags & ROOM_PHANTOM) continue;

        bool is_inside_x = x > room->x && x < room->x + room->width - 1;
        bool is_inside_y = y > room->y && y < room->y + room->height - 1;
        if (is_inside_x && is_inside_y) return room_index;
    }
    return ROOM_COUNT;
}

// a mező egy nem fantom szoba falain belül van
static bool is_inside_room(uint8_t x, uint8_t y)
{
    return get_room_around(x, y) < ROOM_COUNT;
}

// a mező egy normál szoba területén van, a fallal együtt
static bool is_in_normal_room_area(uint8_t x, uint8_t y)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if (room->flags & (ROOM_PHANTOM | ROOM_MAZE)) continue;

        bool is_in_x = x >= room->x && x < room->x + room->width;
        bool is_in_y = y >= room->y && y < room->y + room->height;
        if (is_in_x && is_in_y) return true;
    }
    return false;
}

// pontosan egy lépcső van, egy nem fantom szoba falain belül, nem rejtett mezőn
static bool is_stairs_valid(void)
{
    uint8_t stairs_count = 0;
    uint8_t stairs_x = 0;
    uint8_t stairs_y = 0;

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            if (get_map_character(x, y) == CHAR_STAIRS) {
                stairs_count++;
                stairs_x = x;
                stairs_y = y;
            }
        }
    }

    if (stairs_count != 1) return false;
    if (!(get_map_flags(stairs_x, stairs_y) & TILE_REAL)) return false;
    return is_inside_room(stairs_x, stairs_y);
}

// a játékos egy nem fantom szoba falain belül, járható, nem rejtett mezőn áll
static bool is_player_position_valid(void)
{
    uint8_t x = game.player.x;
    uint8_t y = game.player.y;

    if (!(get_map_flags(x, y) & TILE_REAL)) return false;
    if (!is_walkable_character(get_map_character(x, y))) return false;
    return is_inside_room(x, y);
}

// a kezdő szörnyek: egy nem fantom szoba falain belül, nem rejtett, járható mezőn, nem a játékos helyén; alszanak;
// a room mezőjük a mező szobája (labirintusban a folyosórendszer); szobánként legfeljebb egy, kivéve a kincses szobát,
// ahol a szoba kezdő szörnye mellett aljasak vannak: minden szobában legfeljebb egy nem aljas, és legfeljebb egy szobában több szörny
static bool are_initial_monsters_valid(void)
{
    uint8_t room_monster_counts[ROOM_COUNT];
    uint8_t room_not_mean_counts[ROOM_COUNT];
    memset(room_monster_counts, 0, sizeof(room_monster_counts));
    memset(room_not_mean_counts, 0, sizeof(room_not_mean_counts));

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        uint8_t x = monster->x;
        uint8_t y = monster->y;

        if (monster->type < 'A' || monster->type > 'Z') return false;
        if (monster->flags & MONSTER_RUNNING) return false;
        if (x == game.player.x && y == game.player.y) return false;
        if (!(get_map_flags(x, y) & TILE_REAL)) return false;
        if (!is_walkable_character(get_map_character(x, y))) return false;
        if (monster->room != get_room_at(x, y)) return false;

        uint8_t room_index = get_room_around(x, y);
        if (room_index >= ROOM_COUNT) return false;
        room_monster_counts[room_index]++;
        if (!(monster->flags & MONSTER_MEAN)) {
            room_not_mean_counts[room_index]++;
        }
    }

    uint8_t crowded_room_count = 0;
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        if (room_not_mean_counts[room_index] > 1) return false;
        if (room_monster_counts[room_index] > 1) {
            crowded_room_count++;
        }
    }
    return crowded_room_count <= 1;
}

// a lista hossza; ha a lista szabálytalan (MAX_ITEMS-nál hosszabb, vagyis kör van benne, vagy érvénytelen indexre mutat), MAX_ITEMS + 1
static uint8_t count_list_items(uint8_t list_first)
{
    uint8_t count = 0;
    for (uint8_t index = list_first; index != NO_ITEM; index = get_item(index)->next) {
        if (index > MAX_ITEMS || count == MAX_ITEMS) return MAX_ITEMS + 1;
        count++;
    }
    return count;
}

// a szörnyek csomagjainak tárgyai összesen
static uint16_t count_monster_pack_items(void)
{
    uint16_t count = 0;
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        count += count_list_items(get_monster(index)->first_pack_item);
    }
    return count;
}

// a tárgytábla foglalt helyei
static uint8_t count_occupied_items(void)
{
    uint8_t count = 0;
    for (uint8_t slot = 0; slot < MAX_ITEMS; slot++) {
        if (game.items[slot].type != ITEM_TYPE_FREE) {
            count++;
        }
    }
    return count;
}

// a szintteszt eddigi szintjein a szörnyek kezdő tárgyai összesen: a teszt végére legalább egynek lennie kell
static uint16_t level_starting_item_count;

// a szörnyek kezdő tárgyai: csak tárgyat gyűjtő típusnál (carry > 0), szörnyenként legfeljebb egy, érvényes típusú, érvényes
// kezdőértékekkel; a legmélyebb elért szint fölött egy sem
static bool are_starting_items_valid(void)
{
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        uint8_t item_index = monster->first_pack_item;
        if (item_index == NO_ITEM) continue;

        if (game.dungeon_level < game.max_level) return false;
        if (monster_types[monster->type - 'A'].carry_chance == 0) return false;
        if (item_index > MAX_ITEMS) return false;
        const struct Item *item = get_item(item_index);
        if (item->next != NO_ITEM) return false;
        if (!is_item_type_and_subtype_valid(item->type, item->which)) return false;
        if (!are_item_initial_values_valid(item)) return false;
        level_starting_item_count++;
    }
    return true;
}

// a padlón lévő tárgyak: a lista legfeljebb MAX_ITEMS hosszú (nincs benne kör); minden tárgy érvényes típusú, a kezdőértékei érvényesek,
// egy nem fantom szoba falain belül, nem rejtett mezőn van, a mezőjén a jele áll; két tárgy nincs egy mezőn, és két köteg azonosítója
// nem egyezik; a tárgytábla foglalt helyei éppen a lista és a szörnyek csomagjainak tárgyai, és a game.item_count ennyi
static bool are_floor_items_valid(void)
{
    uint8_t floor_item_count = 0;

    for (uint8_t index = game.first_floor_item; index != NO_ITEM; index = get_item(index)->next) {
        if (index > MAX_ITEMS) return false;
        if (floor_item_count == MAX_ITEMS) return false;
        floor_item_count++;

        const struct Item *item = get_item(index);
        if (item->type == ITEM_TYPE_GOLD || item->type == ITEM_TYPE_AMULET) {
            if (item->which != 0) return false;
        } else if (!is_item_type_and_subtype_valid(item->type, item->which)) {
            return false;
        }
        if (!are_item_initial_values_valid(item)) return false;
        uint8_t x = item->location.floor_position.x;
        uint8_t y = item->location.floor_position.y;
        if (get_map_character(x, y) != get_item_character(item->type)) return false;
        if (!(get_map_flags(x, y) & TILE_REAL)) return false;
        if (!is_inside_room(x, y)) return false;

        // a lista előtte álló tárgyai közül egyik sincs ugyanezen a mezőn, és egyiknek sem ugyanez a kötegazonosítója
        for (uint8_t other = game.first_floor_item; other != index; other = get_item(other)->next) {
            const struct Item *other_item = get_item(other);
            if (other_item->location.floor_position.x == x && other_item->location.floor_position.y == y) return false;
            if (item->group != 0 && other_item->group == item->group) return false;
        }
    }

    uint16_t listed_count = floor_item_count + count_monster_pack_items();
    return count_occupied_items() == listed_count && game.item_count == listed_count;
}

// az arany: szobánként legfeljebb egy, a mennyisége a mélység szerinti tartományban;
// az amulett: az AMULET_LEVEL mélységtől pontosan egy, ha nincs a játékosnál, különben egy sem;
// az are_floor_items_valid után, mert az ellenőrzi, hogy a lista véges, és minden tárgy egy szoba falain belül van
static bool are_gold_and_amulet_valid(void)
{
    bool room_has_gold[ROOM_COUNT];
    memset(room_has_gold, 0, sizeof(room_has_gold));
    uint8_t amulet_count = 0;
    uint16_t gold_amount_max = GOLD_AMOUNT_BASE + GOLD_AMOUNT_PER_LEVEL * game.dungeon_level + GOLD_AMOUNT_MIN - 1;

    for (uint8_t index = game.first_floor_item; index != NO_ITEM; index = get_item(index)->next) {
        const struct Item *item = get_item(index);

        if (item->type == ITEM_TYPE_AMULET) {
            amulet_count++;
        }
        if (item->type != ITEM_TYPE_GOLD) continue;

        uint16_t amount = item->properties.gold_amount;
        if (amount < GOLD_AMOUNT_MIN || amount > gold_amount_max) return false;

        uint8_t room_index = get_room_around(item->location.floor_position.x, item->location.floor_position.y);
        if (room_has_gold[room_index]) return false;
        room_has_gold[room_index] = true;
    }

    uint8_t expected_amulet_count = game.dungeon_level >= AMULET_LEVEL && !is_carrying_amulet() ? 1 : 0;
    return amulet_count == expected_amulet_count;
}

// van étel a padlón
static bool is_food_on_floor(void)
{
    for (uint8_t index = game.first_floor_item; index != NO_ITEM; index = get_item(index)->next) {
        if (get_item(index)->type == ITEM_TYPE_FOOD) return true;
    }
    return false;
}

// van étel egy szörny csomagjában; az are_starting_items_valid után, mert az ellenőrzi, hogy egy csomagban legfeljebb egy tárgy van
static bool is_food_carried_by_monster(void)
{
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        uint8_t item_index = get_monster(index)->first_pack_item;
        if (item_index != NO_ITEM && get_item(item_index)->type == ITEM_TYPE_FOOD) return true;
    }
    return false;
}

// a mezők szobája: minden folyosó-mezőé (a fantom szoba pontja és a labirintus mezői is) a sorszáma szerinti folyosórendszer;
// a normál szoba teljes területe (fal, ajtó, padló, lépcső) az övé; a többi járható mező is egy normál szoba területén van,
// ezért a get_room_at minden járható mezőn talál szobát
static bool are_room_lookups_valid(void)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if (room->flags & (ROOM_PHANTOM | ROOM_MAZE)) continue;

        for (uint8_t y = room->y; y < room->y + room->height; y++) {
            for (uint8_t x = room->x; x < room->x + room->width; x++) {
                if (get_room_at(x, y) != room_index) return false;
            }
        }
    }

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            uint8_t flags = get_map_flags(x, y);

            if (flags & TILE_PASSAGE) {
                if (get_room_at(x, y) != PASSAGE_ROOM_INDEX(flags & TILE_PASSAGE_NUMBER)) return false;
            } else if (is_walkable_tile(x, y)) {
                if (!is_in_normal_room_area(x, y)) return false;
            }
        }
    }
    return true;
}

// az elárasztás verme: minden mező legfeljebb egyszer kerül bele, mert a berakáskor megjelölődik
static uint8_t flood_stack_x[MAP_ROWS * MAP_COLUMNS];
static uint8_t flood_stack_y[MAP_ROWS * MAP_COLUMNS];
static uint16_t flood_stack_count;

// a járható, még jelöletlen mező megjelölve (TILE_SEEN) a verembe kerül
static void visit_tile(uint8_t x, uint8_t y)
{
    if (get_map_flags(x, y) & TILE_SEEN) return;
    if (!is_walkable_tile(x, y)) return;

    add_map_flags(x, y, TILE_SEEN);
    flood_stack_x[flood_stack_count] = x;
    flood_stack_y[flood_stack_count] = y;
    flood_stack_count++;
}

// a szoba egy mezője, amelyik biztosan járható: a normál szobánál a bal felső padló, a labirintusnál a bal felső cella
static uint8_t get_room_inner_x(const struct Room *room)
{
    return room->flags & ROOM_MAZE ? room->x : room->x + 1;
}

static uint8_t get_room_inner_y(const struct Room *room)
{
    return room->flags & ROOM_MAZE ? room->y : room->y + 1;
}

// a szint összefüggő: a játékos mezőjéből elárasztva (BFS), a négy szomszéd felé, minden nem fantom szoba és minden fantom-csomópont elérhető
static bool is_level_connected(void)
{
    flood_stack_count = 0;
    visit_tile(game.player.x, game.player.y);

    while (flood_stack_count > 0) {
        flood_stack_count--;
        uint8_t x = flood_stack_x[flood_stack_count];
        uint8_t y = flood_stack_y[flood_stack_count];

        if (x > 0) visit_tile(x - 1, y);
        if (x < MAP_COLUMNS - 1) visit_tile(x + 1, y);
        if (y > MAP_TOP_ROW) visit_tile(x, y - 1);
        if (y < MAP_TOP_ROW + MAP_ROWS - 1) visit_tile(x, y + 1);
    }

    // a fantom szobánál maga a csomópont
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        uint8_t x = room->x;
        uint8_t y = room->y;
        if (!(room->flags & ROOM_PHANTOM)) {
            x = get_room_inner_x(room);
            y = get_room_inner_y(room);
        }
        if (!(get_map_flags(x, y) & TILE_SEEN)) return false;
    }
    return true;
}

// a teljes szint ujjlenyomata: a MAP két rétege, a szobák és a játékos helye, összegzéssel (Fletcher-féle két összeg)
static uint16_t calculate_level_checksum(void)
{
    uint8_t sum = 0;
    uint8_t sum_of_sums = 0;
    const uint8_t *bytes = &game.map_characters[0][0];

    for (uint16_t i = 0; i < sizeof(game.map_characters); i++) {
        sum += bytes[i];
        sum_of_sums += sum;
    }
    bytes = &game.map_flags[0][0];
    for (uint16_t i = 0; i < sizeof(game.map_flags); i++) {
        sum += bytes[i];
        sum_of_sums += sum;
    }
    bytes = (const uint8_t *)game.rooms;
    for (uint16_t i = 0; i < sizeof(game.rooms); i++) {
        sum += bytes[i];
        sum_of_sums += sum;
    }
    sum += game.player.x;
    sum_of_sums += sum;
    sum += game.player.y;
    sum_of_sums += sum;

    return ((uint16_t)sum_of_sums << 8) | sum;
}

int test_level_generation(uint16_t i)
{
    uint8_t depth_index = i % LEVEL_TEST_DEPTH_COUNT;

    if (i == 0) {
        for (uint8_t d = 0; d < LEVEL_TEST_DEPTH_COUNT; d++) {
            level_room_counts[d] = 0;
            level_dark_counts[d] = 0;
            level_maze_counts[d] = 0;
            level_max_frames[d] = 0;
        }
        level_starting_item_count = 0;
    }
    level_test_run_count = i + 1;

    // a térképen kívüli írást a map.c DEV_CHECK-je ellenőrzi
    game.seed = LEVEL_TEST_FIRST_SEED + i;
    game.dungeon_level = level_test_depths[depth_index];
    // minden mélység a legmélyebb elért szint, így a szörnyek mindenhol kaphatnak kezdő tárgyat
    game.max_level = game.dungeon_level;
    clear_map();
    generate_rooms();

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];

        if (!is_room_inside_sector(room_index)) return false;

        if (room->flags & ROOM_PHANTOM) continue;

        level_room_counts[depth_index]++;

        if (room->flags & (ROOM_DARK | ROOM_MAZE)) {
            // az 1. szinten soha nincs sötét szoba
            if (game.dungeon_level == 1) return false;
            level_dark_counts[depth_index]++;
        }

        if (room->flags & ROOM_MAZE) {
            level_maze_counts[depth_index]++;
            if (!is_maze_tree(room)) return false;
        }
    }

    // a teljes szint ugyanazzal a maggal újra, a labirintus-ellenőrzés után, mert a felső labirintusból induló folyosó kört
    // hozhat létre a labirintusban; a szobáknak ugyanazoknak kell lenniük; közben az idejét is mérjük
    struct Room checked_rooms[ROOM_COUNT];
    memcpy(checked_rooms, game.rooms, sizeof(checked_rooms));
    game.seed = LEVEL_TEST_FIRST_SEED + i;

    // az étel nélküli szintek számlálója próbánként más értékről indul, így a 255-ös megállás is sorra kerül
    uint8_t levels_without_food = (uint8_t)i;
    game.levels_without_food = levels_without_food;

    uint16_t start_frame = get_frame_count_16();
    generate_level();
    uint16_t frames = get_frame_count_16() - start_frame;
    if (frames > level_max_frames[depth_index]) {
        level_max_frames[depth_index] = frames;
    }

    if (memcmp(checked_rooms, game.rooms, sizeof(checked_rooms)) != 0) return false;

    // az ujjlenyomat még az ellenőrzések előtt, mert azok a TILE_SEEN flaget írják
    uint16_t level_checksum = calculate_level_checksum();

    if (!are_starting_items_valid()) return false;
    if (!are_floor_items_valid()) return false;
    if (!are_gold_and_amulet_valid()) return false;

    // a szint tárgyai előtt eggyel nő, és egy generált étel nullázza: a padlóra kerülő étel a növelés után jön, így 0 marad;
    // a szörny kezdő tárgya a szobák szörnyeinél a növelés előtt (1 lesz), a kincses szobáéinál utána (0 marad)
    uint8_t expected_levels_without_food = levels_without_food == UINT8_MAX ? UINT8_MAX : levels_without_food + 1;
    if (is_food_on_floor()) {
        if (game.levels_without_food != 0) return false;
    } else if (is_food_carried_by_monster()) {
        if (game.levels_without_food > 1) return false;
    } else if (game.levels_without_food != expected_levels_without_food) {
        return false;
    }

    if (!is_stairs_valid()) return false;
    if (!is_player_position_valid()) return false;
    if (!are_room_lookups_valid()) return false;
    if (!are_map_tiles_valid()) return false;
    if (!is_exit_table_valid()) return false;
    if (!are_passage_numbers_valid()) return false;
    if (!are_monster_tiles_valid()) return false;
    if (!are_initial_monsters_valid()) return false;

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if ((room->flags & ROOM_MAZE) && !is_maze_without_doors(room)) return false;
    }

    // az are_map_tiles_valid után, mert az törli a labirintus-ellenőrzés TILE_SEEN jelöléseit
    if (!is_level_connected()) return false;

    // ugyanazzal a maggal és állapottal ugyanaz a szint: harmadszor generálva az ujjlenyomatnak egyeznie kell
    game.seed = LEVEL_TEST_FIRST_SEED + i;
    game.levels_without_food = levels_without_food;
    generate_level();
    if (calculate_level_checksum() != level_checksum) return false;

    // a teljes futás alatt legalább egy szörny kapott kezdő tárgyat
    return i < LEVEL_TEST_LEVEL_COUNT - 1 || level_starting_item_count > 0;
}

int test_level_amulet_loot(uint16_t i)
{
    uint8_t amulet = create_item(ITEM_TYPE_AMULET, 0);
    insert_item_after(&game.first_pack_item, NO_ITEM, amulet);
    game.seed = LEVEL_TEST_FIRST_SEED + i;
    game.max_level = AMULET_LEVEL + 1;

    // a legmélyebb elért szint fölött üres a padló, és a szörnyeknek sincs tárgyuk
    game.dungeon_level = AMULET_LEVEL;
    generate_level();
    bool ok = game.first_floor_item == NO_ITEM && count_monster_pack_items() == 0;

    // a legmélyebb elért szinten van arany vagy tárgy, de újabb amulett nincs
    game.dungeon_level = AMULET_LEVEL + 1;
    generate_level();
    ok = ok && game.first_floor_item != NO_ITEM && are_gold_and_amulet_valid() && are_starting_items_valid();

    game.first_pack_item = NO_ITEM;
    free_item(amulet);
    return ok;
}

#define MONSTER_MOVE_TEST_TURNS     20          // ennyi kör TURN_TEST_RUNNING_MONSTERS ébren lévő szörnnyel, aztán ennyi mindegyikkel
#define TURN_TEST_RUNNING_MONSTERS  5           // a spec szerint egy kör a szokásos 3–5 ébren lévő szörnnyel mérendő

// az idő raszter-sorban (PAL; NTSC-n 263 sor egy képkocka)
#define RASTER_LINES_PER_FRAME  312
#define RASTER_UNSAFE_LINES     2               // a raszter-IRQ sorától ennyi sorig a frame_count még a régi lehet

struct RasterTime {
    uint8_t  frame;                             // a frame_count
    uint16_t lines_since_interrupt;             // a raszter-IRQ sora óta eltelt sorok: ennél a sornál lép a frame_count
};

// a frame_count és a raszter-sor egy képkockából; ha olvasás közben jött a megszakítás, vagy éppen most jön, újra
static void read_raster_time(struct RasterTime *time)
{
    while (true) {
        uint8_t frame = frame_count;
        uint16_t line = vic.raster | ((uint16_t)(vic.ctrl1 & 0x80) << 1);
        if (frame != frame_count) continue;

        uint16_t lines_since_interrupt = line >= RASTER_INTERRUPT_ROW ? line - RASTER_INTERRUPT_ROW : line + RASTER_LINES_PER_FRAME - RASTER_INTERRUPT_ROW;
        if (lines_since_interrupt < RASTER_UNSAFE_LINES) continue;

        time->frame = frame;
        time->lines_since_interrupt = lines_since_interrupt;
        return;
    }
}

static uint16_t calculate_elapsed_lines(const struct RasterTime *start, const struct RasterTime *end)
{
    uint8_t frames = end->frame - start->frame;
    return (uint16_t)frames * RASTER_LINES_PER_FRAME + end->lines_since_interrupt - start->lines_since_interrupt;
}

// mélységenként: a körök ideje TURN_TEST_RUNNING_MONSTERS ébren lévő szörnnyel (összeg, darab, legnagyobb), és a legnagyobb, ha mind ébren van
static uint32_t turn_line_sums[LEVEL_TEST_DEPTH_COUNT];
static uint16_t turn_counts[LEVEL_TEST_DEPTH_COUNT];
static uint16_t turn_max_lines[LEVEL_TEST_DEPTH_COUNT];
static uint16_t turn_max_lines_all_running[LEVEL_TEST_DEPTH_COUNT];

// a játékos lépése egy véletlen irányba; (0, 0), ha pihen: ha az irány a térképen kívülre vagy szörnyre esne (a megtámadása még nincs meg)
static void choose_test_player_step(int8_t *step_x, int8_t *step_y)
{
    *step_x = (int8_t)rnd(3) - 1;
    *step_y = (int8_t)rnd(3) - 1;

    // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
    uint8_t x = game.player.x + *step_x;
    uint8_t y = game.player.y + *step_y;
    if (!is_on_map(x, y) || (get_map_flags(x, y) & TILE_MONSTER)) {
        *step_x = 0;
        *step_y = 0;
    }
}

#define LEVEL_TEST_PLAYER_HIT_POINTS  10000     // egy kör alatt ennyit a legtöbb gyorsított szörny sem sebez

// egy kör, ahogy a játékban: a játékos lépése (vagy pihenése), a körülnézés és a szörnyek lépése; az ideje raszter-sorban,
// a képernyőre másolás (show_map_changes) nélkül, de a szörnyek támadásainak üzenetével
static uint16_t take_timed_turn(void)
{
    // mint a játékban a parancs beolvasásakor
    message_clear();
    // a szörnyek támadásai alatt se haljon meg: halála után a szörnyek már nem lépnének
    game.player.hit_points = LEVEL_TEST_PLAYER_HIT_POINTS;

    int8_t step_x;
    int8_t step_y;
    choose_test_player_step(&step_x, &step_y);

    struct RasterTime start;
    struct RasterTime end;
    read_raster_time(&start);
    if (step_x != 0 || step_y != 0) {
        move_player(step_x, step_y);
    }
    look_around();
    move_monsters();
    read_raster_time(&end);

    return calculate_elapsed_lines(&start, &end);
}

// minden szörny járható mezőn áll, nem a játékos mezőjén, és a room mezője a mezőjének szobája
static bool are_monster_positions_valid(void)
{
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        uint8_t x = monster->x;
        uint8_t y = monster->y;

        if (!is_walkable_character(get_map_character(x, y))) return false;
        if (x == game.player.x && y == game.player.y) return false;
        if (monster->room != get_room_at(x, y)) return false;
    }
    return true;
}

// minden foglalt tárgy egy listában van: a padlón, a játékos vagy egy szörny csomagjában; a padló tárgyainak mezőjén a jelük áll
static bool are_item_lists_valid(void)
{
    uint8_t floor_count = count_list_items(game.first_floor_item);
    uint8_t pack_count = count_list_items(game.first_pack_item);
    if (floor_count > MAX_ITEMS || pack_count > MAX_ITEMS) return false;

    uint16_t listed_count = floor_count + pack_count + count_monster_pack_items();
    if (count_occupied_items() != listed_count || game.item_count != listed_count) return false;

    for (uint8_t index = game.first_floor_item; index != NO_ITEM; index = get_item(index)->next) {
        const struct Item *item = get_item(index);
        uint8_t x = item->location.floor_position.x;
        uint8_t y = item->location.floor_position.y;
        if (get_map_character(x, y) != get_item_character(item->type)) return false;
    }
    return true;
}

// MONSTER_MOVE_TEST_TURNS kör; mindegyik után a szörnymezők és a tárgylisták ellenőrzése, az idők a mélység számlálóiba
static bool run_timed_turns(uint8_t depth_index, bool are_all_running)
{
    for (uint8_t turn = 0; turn < MONSTER_MOVE_TEST_TURNS; turn++) {
        uint16_t lines = take_timed_turn();

        if (are_all_running) {
            if (lines > turn_max_lines_all_running[depth_index]) {
                turn_max_lines_all_running[depth_index] = lines;
            }
        } else {
            turn_line_sums[depth_index] += lines;
            turn_counts[depth_index]++;
            if (lines > turn_max_lines[depth_index]) {
                turn_max_lines[depth_index] = lines;
            }
        }

        if (!are_monster_tiles_valid()) return false;
        if (!are_monster_positions_valid()) return false;
        if (!are_item_lists_valid()) return false;
    }
    return true;
}

int test_level_monster_moves(uint16_t i)
{
    uint8_t depth_index = i % LEVEL_TEST_DEPTH_COUNT;

    if (i == 0) {
        for (uint8_t d = 0; d < LEVEL_TEST_DEPTH_COUNT; d++) {
            turn_line_sums[d] = 0;
            turn_counts[d] = 0;
            turn_max_lines[d] = 0;
            turn_max_lines_all_running[d] = 0;
        }
    }

    game.seed = LEVEL_TEST_FIRST_SEED + i;
    game.dungeon_level = level_test_depths[depth_index];
    clear_seen_map();
    generate_level();
    // mint a new_level: a kezdőszoba és a look_around kezdőpontja; hogy melyik szörny van ébren, azt lent a teszt dönti el
    enter_room(game.player.x, game.player.y);
    forget_previous_position();

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        struct Monster *monster = get_monster(index);
        monster->flags &= ~MONSTER_RUNNING;
        if (index <= TURN_TEST_RUNNING_MONSTERS) {
            start_monster_running(monster);
        }
    }
    if (!run_timed_turns(depth_index, false)) return false;

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        start_monster_running(get_monster(index));
    }
    return run_timed_turns(depth_index, true);
}

// az arány-táblázat oszlopai: mélység, szobák, sötét szobák várt és kapott száma, labirintusok várt és kapott száma, eredmény;
// a számok és a fejlécek jobbra igazítva RATIO_COLUMN_WIDTH széles mezőben, a mezők között egy szóköz
#define RATIO_COLUMN_WIDTH      5
#define RATIO_COLUMN_STEP       (RATIO_COLUMN_WIDTH + 1)
#define RATIO_DEPTH_COLUMN      0
#define RATIO_ROOMS_COLUMN      (RATIO_DEPTH_COLUMN + RATIO_COLUMN_STEP)
#define RATIO_DARK_WANT_COLUMN  (RATIO_ROOMS_COLUMN + RATIO_COLUMN_STEP)
#define RATIO_DARK_GOT_COLUMN   (RATIO_DARK_WANT_COLUMN + RATIO_COLUMN_STEP)
#define RATIO_MAZE_WANT_COLUMN  (RATIO_DARK_GOT_COLUMN + RATIO_COLUMN_STEP)
#define RATIO_MAZE_GOT_COLUMN   (RATIO_MAZE_WANT_COLUMN + RATIO_COLUMN_STEP)
#define RATIO_RESULT_COLUMN     (RATIO_MAZE_GOT_COLUMN + RATIO_COLUMN_STEP)    // "OK" vagy "FAIL"
#define RATIO_RESULT_WIDTH      4                                               // a "FAIL" hossza
#if RATIO_RESULT_COLUMN + RATIO_RESULT_WIDTH > SCREEN_WIDTH
#error "az arány-táblázat nem fér ki a képernyő szélességébe"
#endif

// a sötét szobák esélye a mélységen: min(mélység - 1, DARK_ROOM_CHANCE_DIVISOR) / DARK_ROOM_CHANCE_DIVISOR
static uint8_t get_dark_chance(uint8_t depth_index)
{
    uint8_t dark_chance = level_test_depths[depth_index] - 1;
    if (dark_chance > DARK_ROOM_CHANCE_DIVISOR) {
        dark_chance = DARK_ROOM_CHANCE_DIVISOR;
    }
    return dark_chance;
}

// a sötét szobák aránya legfeljebb DARK_RATIO_TOLERANCE_PERCENT százalékponttal, a labirintusok száma a sötétek
// 1 / MAZE_CHANCE_DIVISOR részétől legfeljebb MAZE_RATIO_TOLERANCE_PERCENT százalékkal tér el
static bool is_depth_ratio_ok(uint8_t depth_index)
{
    uint32_t room_count = level_room_counts[depth_index];
    uint32_t dark_count = level_dark_counts[depth_index];
    uint32_t maze_count = level_maze_counts[depth_index];
    uint8_t dark_chance = get_dark_chance(depth_index);

    if (room_count == 0) return false;

    // százalékban: |sötét / szobák - esély / osztó| <= tűrés
    int32_t dark_difference = (int32_t)(dark_count * 100 * DARK_ROOM_CHANCE_DIVISOR) - (int32_t)(room_count * 100 * dark_chance);
    int32_t dark_limit = (int32_t)(room_count * DARK_RATIO_TOLERANCE_PERCENT * DARK_ROOM_CHANCE_DIVISOR);
    if (dark_difference > dark_limit || dark_difference < -dark_limit) return false;

    // |labirintusok - sötétek / osztó| <= a várt érték tűrés százaléka
    int32_t maze_difference = (int32_t)(maze_count * MAZE_CHANCE_DIVISOR * 100) - (int32_t)(dark_count * 100);
    int32_t maze_limit = (int32_t)(dark_count * MAZE_RATIO_TOLERANCE_PERCENT);
    if (maze_difference > maze_limit || maze_difference < -maze_limit) return false;

    return true;
}

static void put_ratio_number(uint8_t column, uint32_t number)
{
    set_screen_cursor_x(column);
    put_int(number, RATIO_COLUMN_WIDTH);
}

// a fejléc jobbra igazítva, mint a számok
static void put_ratio_header(uint8_t column, const char *text, uint8_t length)
{
    set_screen_cursor_x(column + RATIO_COLUMN_WIDTH - length);
    put_text(text);
}

// leállított futásnál csak a számok: a tűrés a teljes futás szobaszámára van szabva, kevés szintnél a véletlen is kilóg belőle
bool show_level_room_ratios(void)
{
    bool all_ok = true;
    bool is_level_test_complete = level_test_run_count == LEVEL_TEST_LEVEL_COUNT;

    set_screen_cursor_x(1);
    put_text("LEVEL-ROOM RATIOS");
    move_screen_cursor_to_next_row();

    put_ratio_header(RATIO_DARK_WANT_COLUMN, "DARK", 4);
    put_ratio_header(RATIO_MAZE_WANT_COLUMN, "MAZE", 4);
    move_screen_cursor_to_next_row();

    put_ratio_header(RATIO_DEPTH_COLUMN, "DEPTH", 5);
    put_ratio_header(RATIO_ROOMS_COLUMN, "ROOMS", 5);
    put_ratio_header(RATIO_DARK_WANT_COLUMN, "WANT", 4);
    put_ratio_header(RATIO_DARK_GOT_COLUMN, "GOT", 3);
    put_ratio_header(RATIO_MAZE_WANT_COLUMN, "WANT", 4);
    put_ratio_header(RATIO_MAZE_GOT_COLUMN, "GOT", 3);
    move_screen_cursor_to_next_row();

    for (uint8_t depth_index = 0; depth_index < LEVEL_TEST_DEPTH_COUNT; depth_index++) {
        uint32_t room_count = level_room_counts[depth_index];
        uint32_t dark_count = level_dark_counts[depth_index];

        // a várt értékek kerekítve
        uint32_t dark_want = (room_count * get_dark_chance(depth_index) + DARK_ROOM_CHANCE_DIVISOR / 2) / DARK_ROOM_CHANCE_DIVISOR;
        uint32_t maze_want = (dark_count + MAZE_CHANCE_DIVISOR / 2) / MAZE_CHANCE_DIVISOR;

        put_ratio_number(RATIO_DEPTH_COLUMN, level_test_depths[depth_index]);
        put_ratio_number(RATIO_ROOMS_COLUMN, room_count);
        put_ratio_number(RATIO_DARK_WANT_COLUMN, dark_want);
        put_ratio_number(RATIO_DARK_GOT_COLUMN, dark_count);
        put_ratio_number(RATIO_MAZE_WANT_COLUMN, maze_want);
        put_ratio_number(RATIO_MAZE_GOT_COLUMN, level_maze_counts[depth_index]);

        set_screen_cursor_x(RATIO_RESULT_COLUMN);
        if (!is_level_test_complete) {
            put_text("-");
        } else if (is_depth_ratio_ok(depth_index)) {
            put_text("OK");
        } else {
            put_text("FAIL");
            all_ok = false;
        }
        move_screen_cursor_to_next_row();
    }

    return all_ok;
}

bool show_level_generation_times(void)
{
    bool all_ok = true;

    set_screen_cursor_x(1);
    put_text("LEVEL TIME (FRAMES)");
    move_screen_cursor_to_next_row();

    put_ratio_header(RATIO_DEPTH_COLUMN, "DEPTH", 5);
    put_ratio_header(RATIO_ROOMS_COLUMN, "MAX", 3);
    put_ratio_header(RATIO_DARK_WANT_COLUMN, "LIMIT", 5);
    move_screen_cursor_to_next_row();

    for (uint8_t depth_index = 0; depth_index < LEVEL_TEST_DEPTH_COUNT; depth_index++) {
        put_ratio_number(RATIO_DEPTH_COLUMN, level_test_depths[depth_index]);
        put_ratio_number(RATIO_ROOMS_COLUMN, level_max_frames[depth_index]);
        put_ratio_number(RATIO_DARK_WANT_COLUMN, LEVEL_TIME_LIMIT_FRAMES);

        set_screen_cursor_x(RATIO_RESULT_COLUMN);
        if (level_max_frames[depth_index] <= LEVEL_TIME_LIMIT_FRAMES) {
            put_text("OK");
        } else {
            put_text("FAIL");
            all_ok = false;
        }
        move_screen_cursor_to_next_row();
    }

    return all_ok;
}

static uint32_t convert_lines_to_frame_percent(uint32_t lines)
{
    return lines * 100 / RASTER_LINES_PER_FRAME;
}

// az AVG és a MAX TURN_TEST_RUNNING_MONSTERS ébren lévő szörnnyel, az ALL a legnagyobb, ha mind ébren van; nincs keret, csak mérés
void show_level_turn_times(void)
{
    set_screen_cursor_x(1);
    put_text("TURN (% OF FRAME): 5 / ALL AWAKE");
    move_screen_cursor_to_next_row();

    put_ratio_header(RATIO_DEPTH_COLUMN, "DEPTH", 5);
    put_ratio_header(RATIO_ROOMS_COLUMN, "AVG", 3);
    put_ratio_header(RATIO_DARK_WANT_COLUMN, "MAX", 3);
    put_ratio_header(RATIO_DARK_GOT_COLUMN, "ALL", 3);
    move_screen_cursor_to_next_row();

    for (uint8_t depth_index = 0; depth_index < LEVEL_TEST_DEPTH_COUNT; depth_index++) {
        uint16_t count = turn_counts[depth_index];
        uint32_t average_lines = count == 0 ? 0 : turn_line_sums[depth_index] / count;

        put_ratio_number(RATIO_DEPTH_COLUMN, level_test_depths[depth_index]);
        put_ratio_number(RATIO_ROOMS_COLUMN, convert_lines_to_frame_percent(average_lines));
        put_ratio_number(RATIO_DARK_WANT_COLUMN, convert_lines_to_frame_percent(turn_max_lines[depth_index]));
        put_ratio_number(RATIO_DARK_GOT_COLUMN, convert_lines_to_frame_percent(turn_max_lines_all_running[depth_index]));
        move_screen_cursor_to_next_row();
    }
}
