#include "explore.h"

#include "game.h"
#include "map.h"
#include "message.h"
#include "monsters.h"
#include "rng.h"
#include "rooms.h"
#include "state.h"
#include "texts.h"
#include "view.h"

// a look_around előző hívásakor a játékos helye és szobája: ha azóta elmozdult, ott törlődik a lámpafény
static uint8_t previous_x;
static uint8_t previous_y;
static uint8_t previous_room;

// világos az a szoba, amelyik nem sötét; a folyosórendszer sötét, és a labirintus mezőin is az a lény szobája
static bool is_room_lit(uint8_t room_index)
{
    return !(game.rooms[room_index].flags & ROOM_DARK);
}

bool is_monster_visible(const struct Monster *monster)
{
    // ide jön: vakon egyik sem látszik
    // ide jön: láthatatlan-látással (CANSEE) a láthatatlan is
    if (monster->flags & MONSTER_INVISIBLE) return false;

    int8_t offset_x = monster->x - game.player.x;
    int8_t offset_y = monster->y - game.player.y;
    bool is_adjacent = offset_x >= -1 && offset_x <= 1 && offset_y >= -1 && offset_y <= 1;

    // a szomszédos szörny a lámpafényben látszik, átlósan csak akkor, ha a sarok legalább egyik mezője járható
    if (is_adjacent) {
        if (offset_x == 0 || offset_y == 0) return true;
        return is_walkable_character(get_map_character(game.player.x, monster->y))
            || is_walkable_character(get_map_character(monster->x, game.player.y));
    }

    return monster->room == game.player.room && is_room_lit(monster->room);
}

uint8_t get_visible_character(uint8_t x, uint8_t y)
{
    uint8_t monster_index = find_monster_at(x, y);
    if (monster_index != NO_MONSTER) {
        const struct Monster *monster = get_monster(monster_index);
        // ide jön: hallucinálva véletlen nagybetű
        if (is_monster_visible(monster)) return monster->disguise;
    }
    return get_map_character(x, y);
}

void enter_room(uint8_t x, uint8_t y)
{
    game.player.room = get_room_at(x, y);
    // ide jön: a szoba szörnyeinek ébresztése (door_open)

    // ide jön: vakon nem rajzol
    if (!is_room_lit(game.player.room)) return;

    // az egész szoba a falakkal együtt; a szörnyek alatt a mező jele lesz az emlék
    const struct Room *room = &game.rooms[game.player.room];
    for (uint8_t tile_y = room->y; tile_y < room->y + room->height; tile_y++) {
        for (uint8_t tile_x = room->x; tile_x < room->x + room->width; tile_x++) {
            uint8_t monster_index = find_monster_at(tile_x, tile_y);
            if (monster_index != NO_MONSTER) {
                get_monster(monster_index)->character_below = get_map_character(tile_x, tile_y);
            }
            // ide jön: szörnyészleléssel (SEEMONST) a nem látható szörny inverzen
            draw_tile(tile_x, tile_y, get_visible_character(tile_x, tile_y));
        }
    }
}

// a játékos ajtóról lép ki, és ajtaja csak normál szobának van, ezért a régi szoba sem fantom, sem labirintus
// a szörny a képernyőn látott betűje alapján tűnik el, így a tárgyjellel álcázott xeroc jele marad
void leave_room(uint8_t x, uint8_t y)
{
    const struct Room *room = &game.rooms[game.player.room];
    bool is_dark = (room->flags & ROOM_DARK) != 0;
    // ide jön: vakon a padló marad
    uint8_t floor_character = is_dark ? CHAR_SOLID_ROCK : CHAR_FLOOR;

    game.player.room = get_room_at(x, y);

    for (uint8_t tile_y = room->y; tile_y < room->y + room->height; tile_y++) {
        for (uint8_t tile_x = room->x; tile_x < room->x + room->width; tile_x++) {
            uint8_t seen_character = get_seen_character(tile_x, tile_y);

            if (seen_character == CHAR_FLOOR) {
                if (is_dark) {
                    draw_tile(tile_x, tile_y, CHAR_SOLID_ROCK);
                }
            } else if (seen_character >= 'A' && seen_character <= 'Z') {
                // ide jön: szörnyészleléssel (SEEMONST) a szörny jele inverzen marad
                bool is_door = get_map_character(tile_x, tile_y) == CHAR_DOOR;
                draw_tile(tile_x, tile_y, is_door ? CHAR_DOOR : floor_character);
            }
        }
    }

    // ide jön: a régi szoba szörnyeinek ébresztése (door_open)
}

// sötét, nem fantom szobában a pozíció körüli 3×3 padlója törlődik, a játékos mezője kivételével; folyosón nem
static void erase_lamp_light(uint8_t x, uint8_t y, uint8_t room_index)
{
    // ide jön: vakon nem töröl
    if ((game.rooms[room_index].flags & (ROOM_PHANTOM | ROOM_DARK)) != ROOM_DARK) return;

    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t tile_x = x + offset_x;
            uint8_t tile_y = y + offset_y;
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
    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t x = player_x + offset_x;
            uint8_t y = player_y + offset_y;
            if (!is_on_map(x, y)) continue;
            if (x == player_x && y == player_y) continue;
            if (!is_neighbor_visible(player_x, player_y, x, y)) continue;

            // ide jön: szörnyészleléssel (SEEMONST) a láthatatlan szörny mezője kimarad, és a futás megáll
            // ide jön: a szörny ébresztése (wake_monster)
            // ide jön: szörny nélküli mezőn hallucinálva véletlen tárgyjel (trip_ch)
            draw_tile(x, y, get_visible_character(x, y));
        }
    }

    draw_tile(player_x, player_y, CHAR_PLAYER);
}

bool try_to_find_hidden_tile(uint8_t x, uint8_t y)
{
    if (get_map_flags(x, y) & TILE_REAL) return false;

    // ide jön: hallucinálva +3, vakon +2
    uint8_t penalty = 0;
    uint8_t character = get_map_character(x, y);

    if (character == CHAR_WALL_HORIZONTAL || character == CHAR_WALL_VERTICAL) {
        if (rnd(5 + penalty) != 0) return false;
        set_map_character(x, y, CHAR_DOOR);
    } else if (character == CHAR_SOLID_ROCK) {
        if (rnd(3 + penalty) != 0) return false;
        set_map_character(x, y, CHAR_PASSAGE);
    } else {
        // ide jön: rejtett csapda (a padló jele)
        return false;
    }

    add_map_flags(x, y, TILE_REAL);
    return true;
}

void search_around(void)
{
    uint8_t player_x = game.player.x;
    uint8_t player_y = game.player.y;

    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t x = player_x + offset_x;
            uint8_t y = player_y + offset_y;
            if (!is_on_map(x, y)) continue;
            if (x == player_x && y == player_y) continue;
            if (!try_to_find_hidden_tile(x, y)) continue;

            if (get_map_character(x, y) == CHAR_DOOR) {
                message(text_secret_door);
            }
            // a felfedett mezőt a következő look_around rajzolja ki
            stop_running_and_repeating();
        }
    }
}

void forget_previous_position(void)
{
    previous_x = game.player.x;
    previous_y = game.player.y;
    previous_room = game.player.room;
}

// a játékos mindig látja a saját mezőjét, ezért a MAP jele kerül vissza; fantom szobában (folyosón) a padlóból folyosójel lesz
uint8_t calculate_character_left_behind(void)
{
    uint8_t character = get_map_character(game.player.x, game.player.y);

    if (character == CHAR_FLOOR && (game.rooms[game.player.room].flags & ROOM_PHANTOM)) {
        character = CHAR_PASSAGE;
    }
    return character;
}
