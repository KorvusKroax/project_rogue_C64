#include "rooms.h"

#include "rng.h"
#include "state.h"

// a logikai 80×24-es képernyő 3×3 szektorra osztva; az i. szektor sora i / 3, oszlopa i % 3
#define SECTOR_WIDTH   26                       // a térkép 80 oszlopa 3 szektorra, lefelé kerekítve
#define SECTOR_HEIGHT  8                        // a térkép 22 sora az üzenet- és a státuszsorral együtt: 24 / 3

#define MAX_PHANTOM_ROOMS  3

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
    }
}
