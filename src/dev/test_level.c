#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../level.h"
#include "../map.h"
#include "../platform/platform.h"
#include "../platform/screen.h"
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

// hány szint futott le; ESC-re a teszt LEVEL_TEST_LEVEL_COUNT előtt leáll
static uint16_t level_test_run_count;

// mélységenként a leglassabb generate_level képkockában
static uint16_t level_max_frames[LEVEL_TEST_DEPTH_COUNT];

#define LEVEL_TIME_LIMIT_FRAMES  100            // egy szint legfeljebb 2 másodperc: PAL-on 50 képkocka/s (NTSC-n 60, ott ez szigorúbb)

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

// a labirintusban nincs ajtó: a mezői folyosó-mezők, üresek, vagy a lépcső áll rajtuk; a kijárat is folyosó-mező marad
static bool is_maze_without_doors(const struct Room *room)
{
    uint8_t last_x = room->x + room->width / 2 * 2;
    uint8_t last_y = room->y + room->height / 2 * 2;

    for (uint8_t y = room->y; y <= last_y; y++) {
        for (uint8_t x = room->x; x <= last_x; x++) {
            uint8_t character = get_map_character(x, y);
            if (character != CHAR_PASSAGE && character != CHAR_SOLID_ROCK && character != CHAR_STAIRS) return false;
        }
    }
    return true;
}

// járható mező: padló, ajtó, lépcső, folyosó-mező, a titkosakkal együtt
static bool is_walkable_tile(uint8_t x, uint8_t y)
{
    uint8_t flags = get_map_flags(x, y);
    if (flags & TILE_PASSAGE) return true;
    if (!(flags & TILE_REAL)) return true;

    uint8_t character = get_map_character(x, y);
    return character == CHAR_FLOOR || character == CHAR_DOOR || character == CHAR_STAIRS;
}

// a mező egy nem fantom szoba falain belül van
static bool is_inside_room(uint8_t x, uint8_t y)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if (room->flags & ROOM_PHANTOM) continue;

        bool is_inside_x = x > room->x && x < room->x + room->width - 1;
        bool is_inside_y = y > room->y && y < room->y + room->height - 1;
        if (is_inside_x && is_inside_y) return true;
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
    }
    level_test_run_count = i + 1;

    // a térképen kívüli írást a map.c DEV_CHECK-je ellenőrzi
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

    // a teljes szint ugyanazzal a maggal újra, a labirintus-ellenőrzés után, mert a felső labirintusból induló folyosó kört
    // hozhat létre a labirintusban; a szobáknak ugyanazoknak kell lenniük; közben az idejét is mérjük
    struct Room checked_rooms[ROOM_COUNT];
    memcpy(checked_rooms, game.rooms, sizeof(checked_rooms));
    game.seed = LEVEL_TEST_FIRST_SEED + i;

    uint16_t start_frame = get_frame_count_16();
    generate_level();
    uint16_t frames = get_frame_count_16() - start_frame;
    if (frames > level_max_frames[depth_index]) {
        level_max_frames[depth_index] = frames;
    }

    if (memcmp(checked_rooms, game.rooms, sizeof(checked_rooms)) != 0) return false;

    // az ujjlenyomat még az ellenőrzések előtt, mert azok a TILE_SEEN flaget írják
    uint16_t level_checksum = calculate_level_checksum();

    if (!is_stairs_valid()) return false;
    if (!is_player_position_valid()) return false;
    if (!are_map_tiles_valid()) return false;

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if ((room->flags & ROOM_MAZE) && !is_maze_without_doors(room)) return false;
    }

    // az are_map_tiles_valid után, mert az törli a labirintus-ellenőrzés TILE_SEEN jelöléseit
    if (!is_level_connected()) return false;

    // ugyanazzal a maggal ugyanaz a szint: harmadszor generálva az ujjlenyomatnak egyeznie kell
    game.seed = LEVEL_TEST_FIRST_SEED + i;
    generate_level();
    return calculate_level_checksum() == level_checksum;
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
