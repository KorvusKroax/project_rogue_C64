#include "explore.h"

#include "map.h"
#include "rooms.h"
#include "state.h"
#include "view.h"

// a look_around előző hívásakor a játékos helye és szobája: ha azóta elmozdult, ott törlődik a lámpafény
static uint8_t previous_x;
static uint8_t previous_y;
static uint8_t previous_room;

// világos az a szoba, amelyik nem sötét és nem labirintus; folyosón sötét van
static bool is_room_lit(uint8_t room_index)
{
    if (room_index == IN_PASSAGE) return false;
    return !(game.rooms[room_index].flags & (ROOM_DARK | ROOM_MAZE));
}

void enter_room(uint8_t x, uint8_t y)
{
    game.player.room = get_room_at(x, y);
    // ide jön: a szoba szörnyeinek ébresztése (door_open)

    // ide jön: vakon nem rajzol
    if (!is_room_lit(game.player.room)) return;

    // az egész szoba a falakkal együtt
    const struct Room *room = &game.rooms[game.player.room];
    for (uint8_t tile_y = room->y; tile_y < room->y + room->height; tile_y++) {
        for (uint8_t tile_x = room->x; tile_x < room->x + room->width; tile_x++) {
            // ide jön: a szörnyek helyén a látható szörnyek
            draw_tile(tile_x, tile_y, get_map_character(tile_x, tile_y));
        }
    }
}

// a játékos ajtóról lép ki, és ajtaja csak normál szobának van, ezért a régi szoba sem fantom, sem labirintus
void leave_room(uint8_t x, uint8_t y)
{
    const struct Room *room = &game.rooms[game.player.room];
    bool is_dark = (room->flags & ROOM_DARK) != 0;

    game.player.room = get_room_at(x, y);

    // ide jön: vakon a padló marad
    if (is_dark) {
        for (uint8_t tile_y = room->y; tile_y < room->y + room->height; tile_y++) {
            for (uint8_t tile_x = room->x; tile_x < room->x + room->width; tile_x++) {
                if (get_seen_character(tile_x, tile_y) == CHAR_FLOOR) {
                    draw_tile(tile_x, tile_y, CHAR_SOLID_ROCK);
                }
            }
        }
    }

    // ide jön: a szörnyek jele helyére a padló (ajtón az ajtó), és a régi szoba szörnyeinek ébresztése (door_open)
}

// sötét szobában a pozíció körüli 3×3 padlója törlődik, a játékos mezője kivételével
static void erase_lamp_light(uint8_t x, uint8_t y, uint8_t room_index)
{
    // ide jön: vakon nem töröl
    if (room_index == IN_PASSAGE || !(game.rooms[room_index].flags & ROOM_DARK)) return;

    // eltolással, mert a 0. oszlop (sor) előtti mező 255 lesz: azt a térképen kívüliként kihagyja
    for (uint8_t offset_y = 0; offset_y < 3; offset_y++) {
        for (uint8_t offset_x = 0; offset_x < 3; offset_x++) {
            uint8_t tile_x = x - 1 + offset_x;
            uint8_t tile_y = y - 1 + offset_y;
            if (!is_on_map(tile_x, tile_y)) continue;
            if (tile_x == game.player.x && tile_y == game.player.y) continue;

            if (get_seen_character(tile_x, tile_y) == CHAR_FLOOR) {
                draw_tile(tile_x, tile_y, CHAR_SOLID_ROCK);
            }
        }
    }
}

bool is_neighbor_visible(uint8_t from_x, uint8_t from_y, uint8_t x, uint8_t y)
{
    uint8_t character = get_map_character(x, y);
    if (character == CHAR_SOLID_ROCK) return false;

    uint8_t from_character = get_map_character(from_x, from_y);
    bool is_passage = (get_map_flags(x, y) & TILE_PASSAGE) != 0;
    bool is_from_passage = (get_map_flags(from_x, from_y) & TILE_PASSAGE) != 0;

    // folyosó és szoba között csak ajtón át
    if (character != CHAR_DOOR && from_character != CHAR_DOOR && is_passage != is_from_passage) return false;

    // két folyosó- vagy ajtómező között átlósan csak akkor, ha a sarok legalább egyik mezője járható
    bool is_diagonal = x != from_x && y != from_y;
    if (is_diagonal && (is_passage || character == CHAR_DOOR) && (is_from_passage || from_character == CHAR_DOOR)) {
        if (!is_walkable_character(get_map_character(from_x, y)) && !is_walkable_character(get_map_character(x, from_y))) return false;
    }

    return true;
}

void look_around(void)
{
    uint8_t player_x = game.player.x;
    uint8_t player_y = game.player.y;

    if (player_x != previous_x || player_y != previous_y) {
        erase_lamp_light(previous_x, previous_y, previous_room);
        previous_x = player_x;
        previous_y = player_y;
        previous_room = game.player.room;
    }

    // ide jön: vakon csak a játékos mezője, de a szomszédos szörnyek ébresztése vakon is
    // eltolással, mint az erase_lamp_light
    for (uint8_t offset_y = 0; offset_y < 3; offset_y++) {
        for (uint8_t offset_x = 0; offset_x < 3; offset_x++) {
            uint8_t x = player_x - 1 + offset_x;
            uint8_t y = player_y - 1 + offset_y;
            if (!is_on_map(x, y)) continue;
            if (x == player_x && y == player_y) continue;
            if (!is_neighbor_visible(player_x, player_y, x, y)) continue;

            // ide jön: szörny a mezőn (ébresztés, a jele, ha látszik), hallucinálva véletlen tárgyjel
            draw_tile(x, y, get_map_character(x, y));
        }
    }

    draw_tile(player_x, player_y, CHAR_PLAYER);
}

void forget_previous_position(void)
{
    previous_x = game.player.x;
    previous_y = game.player.y;
    previous_room = game.player.room;
}

// a játékos mindig látja a saját mezőjét, ezért a MAP jele kerül vissza; a folyosón álló padlóból folyosójel lesz
uint8_t calculate_character_left_behind(void)
{
    uint8_t character = get_map_character(game.player.x, game.player.y);

    if (character == CHAR_FLOOR && game.player.room == IN_PASSAGE) {
        character = CHAR_PASSAGE;
    }
    return character;
}
