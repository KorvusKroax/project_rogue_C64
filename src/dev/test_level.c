#include "tests.h"

#include <stdbool.h>

#include "../corridors.h"
#include "../map.h"
#include "../rooms.h"
#include "../state.h"

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

// ideiglenes, az are_rooms_connected-del együtt: a folyosók után a térképen ellenőrizzük, hogy minden szoba elérhető
static bool rooms_reached[ROOM_COUNT];

// az összekötés: mindkét szobánál ugyanaz látszik, minden szoba elérhető a 0.-ból (a gráf összefüggő),
// és a folyosók száma a feszítőfa ROOM_COUNT - 1 éle, meg legfeljebb MAX_EXTRA_CORRIDORS extra
static bool is_room_graph_connected(void)
{
    uint8_t corridor_count = 0;

    for (uint8_t room_a = 0; room_a < ROOM_COUNT; room_a++) {
        rooms_reached[room_a] = false;
        for (uint8_t room_b = room_a + 1; room_b < ROOM_COUNT; room_b++) {
            bool connected = are_rooms_connected(room_a, room_b);
            if (connected != are_rooms_connected(room_b, room_a)) return false;
            if (connected) corridor_count++;
        }
    }

    if (corridor_count < ROOM_COUNT - 1 || corridor_count > ROOM_COUNT - 1 + MAX_EXTRA_CORRIDORS) return false;

    // elárasztás a 0. szobából, amíg terjed
    uint8_t reached_count = 1;
    bool spread = true;
    rooms_reached[0] = true;

    while (spread) {
        spread = false;
        for (uint8_t room_a = 0; room_a < ROOM_COUNT; room_a++) {
            if (!rooms_reached[room_a]) continue;
            for (uint8_t room_b = 0; room_b < ROOM_COUNT; room_b++) {
                if (!rooms_reached[room_b] && are_rooms_connected(room_a, room_b)) {
                    rooms_reached[room_b] = true;
                    reached_count++;
                    spread = true;
                }
            }
        }
    }

    return reached_count == ROOM_COUNT;
}

int test_level_generation(uint16_t i)
{
    uint8_t depth_index = i % LEVEL_TEST_DEPTH_COUNT;

    if (i == 0) {
        for (uint8_t d = 0; d < LEVEL_TEST_DEPTH_COUNT; d++) {
            level_room_counts[d] = 0;
            level_dark_counts[d] = 0;
            level_maze_counts[d] = 0;
        }
    }

    // a labirintus vermét a generálás, a térképen kívüli írást a map.c DEV_CHECK-jei ellenőrzik
    game.seed = LEVEL_TEST_FIRST_SEED + i;
    game.dungeon_level = level_test_depths[depth_index];
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

    // a labirintus-ellenőrzés után, mert a felső labirintusból induló folyosó kört hozhat létre a labirintusban
    connect_rooms();
    if (!is_room_graph_connected()) return false;

    return true;
}

// a test_level_generation számlálóiból: a sötét szobák aránya min(mélység - 1, DARK_ROOM_CHANCE_DIVISOR) / DARK_ROOM_CHANCE_DIVISOR,
// a labirintusoké a sötét szobák 1 / MAZE_CHANCE_DIVISOR része; az i. próba az i. mélység
int test_level_room_ratios(uint16_t i)
{
    uint32_t room_count = level_room_counts[i];
    uint32_t dark_count = level_dark_counts[i];
    uint32_t maze_count = level_maze_counts[i];
    uint8_t dark_chance = level_test_depths[i] - 1;

    if (room_count == 0) return false;

    if (dark_chance > DARK_ROOM_CHANCE_DIVISOR) {
        dark_chance = DARK_ROOM_CHANCE_DIVISOR;
    }

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
