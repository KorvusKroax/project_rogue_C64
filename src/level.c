#include "level.h"

#include "corridors.h"
#include "map.h"
#include "rng.h"
#include "rooms.h"
#include "state.h"
#include "view.h"

// mire keresünk mezőt
#define FIND_FOR_ITEM    0                      // üres padló, labirintusban üres járat
#define FIND_FOR_PLAYER  1                      // járható jel, szörny nélkül

// a legnagyobb szoba a szektor utolsó oszlopa és sora nélkül, a falai nélkül ennyi mezője van (default: 23 × 5 = 115)
#define MAX_ROOM_INNER_TILES  ((SECTOR_WIDTH - 3) * (SECTOR_HEIGHT - 3))

// nagy szektornál (pl. 1×1-es rácsnál) a jelöltek száma 255-nél több is lehet
#if MAX_ROOM_INNER_TILES > 255
    typedef uint16_t tile_count_t;
#else
    typedef uint8_t tile_count_t;
#endif

static bool is_tile_suitable(const struct Room *room, uint8_t x, uint8_t y, uint8_t purpose)
{
    uint8_t character = get_map_character(x, y);

    if (purpose == FIND_FOR_ITEM) {
        return character == (room->flags & ROOM_MAZE ? CHAR_PASSAGE : CHAR_FLOOR);
    }

    if (get_map_flags(x, y) & TILE_MONSTER) return false;
    return is_walkable_character(character);
}

// a szoba falain belüli alkalmas mezők száma
static tile_count_t count_suitable_tiles(const struct Room *room, uint8_t purpose)
{
    tile_count_t count = 0;
    uint8_t right_x = room->x + room->width - 1;
    uint8_t bottom_y = room->y + room->height - 1;

    for (uint8_t y = room->y + 1; y < bottom_y; y++) {
        for (uint8_t x = room->x + 1; x < right_x; x++) {
            if (is_tile_suitable(room, x, y, purpose)) {
                count++;
            }
        }
    }
    return count;
}

// a chosen. alkalmas mező (0-tól, soronként balról jobbra); chosen kisebb, mint az alkalmas mezők száma
static void get_suitable_tile(const struct Room *room, uint8_t purpose, tile_count_t chosen, uint8_t *x, uint8_t *y)
{
    uint8_t right_x = room->x + room->width - 1;
    uint8_t bottom_y = room->y + room->height - 1;

    for (uint8_t tile_y = room->y + 1; tile_y < bottom_y; tile_y++) {
        for (uint8_t tile_x = room->x + 1; tile_x < right_x; tile_x++) {
            if (!is_tile_suitable(room, tile_x, tile_y, purpose)) continue;

            if (chosen == 0) {
                *x = tile_x;
                *y = tile_y;
                return;
            }
            chosen--;
        }
    }
}

// egy alkalmas mező a szobában, egyetlen dobással; egy jelöltnél nincs mit dobni
static bool find_tile_in_room(uint8_t room_index, uint8_t purpose, uint8_t *x, uint8_t *y)
{
    const struct Room *room = &game.rooms[room_index];
    tile_count_t count = count_suitable_tiles(room, purpose);

    if (count == 0) return false;

    tile_count_t chosen = 0;
    if (count > 1) {
        chosen = rnd(count);
    }
    get_suitable_tile(room, purpose, chosen, x, y);
    return true;
}

// ANY_ROOM-nál a még nem próbált nem fantom szobák közül egy dobással választ, amíg talál, vagy el nem fogynak
static bool find_tile(uint8_t room_index, uint8_t purpose, uint8_t *x, uint8_t *y)
{
    if (room_index != ANY_ROOM) {
        return find_tile_in_room(room_index, purpose, x, y);
    }

    bool tried[ROOM_COUNT];
    uint8_t untried_count = 0;
    for (uint8_t i = 0; i < ROOM_COUNT; i++) {
        tried[i] = (game.rooms[i].flags & ROOM_PHANTOM) != 0;
        if (!tried[i]) {
            untried_count++;
        }
    }

    while (untried_count > 0) {
        uint8_t chosen = 0;
        if (untried_count > 1) {
            chosen = rnd(untried_count);
        }

        // a chosen. még nem próbált szoba
        uint8_t chosen_room = 0;
        while (tried[chosen_room] || chosen > 0) {
            if (!tried[chosen_room]) {
                chosen--;
            }
            chosen_room++;
        }

        if (find_tile_in_room(chosen_room, purpose, x, y)) return true;

        tried[chosen_room] = true;
        untried_count--;
    }
    return false;
}

bool find_empty_floor(uint8_t room_index, uint8_t *x, uint8_t *y)
{
    return find_tile(room_index, FIND_FOR_ITEM, x, y);
}

bool find_walkable_tile(uint8_t room_index, uint8_t *x, uint8_t *y)
{
    return find_tile(room_index, FIND_FOR_PLAYER, x, y);
}

// a lépcső egy véletlen nem fantom szoba üres padlójára (labirintusban járatra)
static void place_stairs(void)
{
    uint8_t x;
    uint8_t y;

    // a még üres szobákban mindig van hely, ezért a visszatérési érték nem kell;
    // ha a szobák felépítése változik, erre figyelni kell: hely nélkül az x és az y érvénytelen, és egy véletlen mező íródna felül
    find_empty_floor(ANY_ROOM, &x, &y);

    set_map_character(x, y, CHAR_STAIRS);
}

// a játékos helye egy véletlen nem fantom szoba járható mezőjén
static void choose_player_position(void)
{
    // a szobákban még csak a lépcső van, ezért mindig van hely, és a visszatérési érték nem kell;
    // ha a szobák felépítése változik, erre figyelni kell: hely nélkül a játékos helye érvénytelen maradna
    find_walkable_tile(ANY_ROOM, &game.player.x, &game.player.y);
}

void generate_level(void)
{
    // ide jön: ha a mélység nagyobb az eddigi legnagyobbnál (max_level), az lesz az új legnagyobb
    clear_map();
    // ide jön: a szörnyek a csomagjaikkal és a padlón lévő tárgyak törlése, a táblákban a helyük felszabadul

    generate_rooms();
    connect_rooms();

    // a lépcső és a játékos még az üres szobákba kerül, minden járat látható: mindkettő mindig talál helyet
    place_stairs();
    choose_player_position();
    hide_passages(game.player.x, game.player.y);

    // ide jön: szobánként arany és kezdő szörny
    // ide jön: a szintek számlálója az étel nélküli tárgygeneráláshoz (no_food) eggyel nő
    // ide jön: tárgyak (kincses szoba, 9 próba, amulett)
    // ide jön: csapdák
    // ide jön: minden szörnynél a szoba, amelyikben áll
}

void new_level(void)
{
    // ide jön: a játékost nem tartja fogva semmi (ISHELD le)
    clear_seen_map();
    generate_level();
    // ide jön: a lépcsőt a játékos még nem látta (seenstairs)

    // ide jön: a játékos szobájának megvilágítása és a szörnyek ébresztése (enter_room)
    center_view_window(game.player.x);

    // ide jön: szörnyészlelés alatt a szörnyek azonnal kirajzolódnak (turn_see)
    // ide jön: hallucináció alatt a jelek azonnal hallucináltak (visuals)
}
