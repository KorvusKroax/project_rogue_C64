#include "rooms.h"

#include "dev/dev.h"
#include "rng.h"
#include "state.h"

// a logikai 80×24-es képernyő 3×3 szektorra osztva; az i. szektor sora i / 3, oszlopa i % 3
#define SECTOR_WIDTH   26                       // a térkép 80 oszlopa 3 szektorra, lefelé kerekítve
#define SECTOR_HEIGHT  8                        // a térkép 22 sora az üzenet- és a státuszsorral együtt: 24 / 3

#define MAX_PHANTOM_ROOMS  3

#define MIN_ROOM_SIZE  4                        // falakkal együtt

// a szoba helye és mérete a szektorán belül, a flagjei szerint
static void place_room(uint8_t room_index)
{
    struct Room *room = &game.rooms[room_index];
    uint8_t corner_x = room_index % 3 * SECTOR_WIDTH + 1;    // a szektor bal felső sarka
    uint8_t corner_y = room_index / 3 * SECTOR_HEIGHT;

    // fantom szoba: egyetlen folyosó-csomópont, a szektor szélétől legalább 1 mezőre
    if (room->flags & ROOM_PHANTOM) {
        room->x = corner_x + rnd(SECTOR_WIDTH - 2) + 1;
        room->y = corner_y + rnd(SECTOR_HEIGHT - 2) + 1;
        room->width = 0;
        room->height = 0;

        DEV_CHECK(room->y >= 1 && room->y <= MAP_ROWS, "phantom room outside the map");
        return;
    }

    // labirintus szoba: kitölti a szektort
    if (room->flags & ROOM_MAZE) {
        room->x = corner_x == 1 ? 0 : corner_x;
        room->y = corner_y;
        room->width = SECTOR_WIDTH - 1;
        room->height = SECTOR_HEIGHT - 1;

        // a 0. sor az üzenetsor
        if (room->y == 0) {
            room->y = 1;
            room->height--;
        }
        return;
    }

    // normál szoba: véletlen méret, és olyan hely, hogy a szektoron belül maradjon; ha a felső fala a 0. sorba (az üzenetsorba) esne, újrasorsol
    do {
        room->width = rnd(SECTOR_WIDTH - MIN_ROOM_SIZE) + MIN_ROOM_SIZE;
        room->height = rnd(SECTOR_HEIGHT - MIN_ROOM_SIZE) + MIN_ROOM_SIZE;
        room->x = corner_x + rnd(SECTOR_WIDTH - room->width);
        room->y = corner_y + rnd(SECTOR_HEIGHT - room->height);
    } while (room->y == 0);
}

uint8_t choose_random_room(void)
{
    uint8_t room_index = rnd(ROOM_COUNT);

    while (game.rooms[room_index].flags & ROOM_PHANTOM) {
        room_index = rnd(ROOM_COUNT);
    }

    return room_index;
}

void generate_rooms(void)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        game.rooms[room_index].flags = 0;
    }

    // a choose_random_room csak olyan szobát választ, amelyik még nem fantom
    uint8_t phantom_room_count = rnd(MAX_PHANTOM_ROOMS + 1);
    for (uint8_t i = 0; i < phantom_room_count; i++) {
        game.rooms[choose_random_room()].flags |= ROOM_PHANTOM;
    }

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        struct Room *room = &game.rooms[room_index];

        if (room->flags & ROOM_PHANTOM) {
            place_room(room_index);
            continue;
        }

        // a mélységgel nő a sötét szobák aránya, az 1. szinten nincs ilyen
        if (rnd(10) < game.dungeon_level - 1) {
            room->flags |= ROOM_DARK;

            // értékadás: a labirintus nem sötét
            if (rnd(15) == 0) {
                room->flags = ROOM_MAZE;
            }
        }

        place_room(room_index);
    }
}
