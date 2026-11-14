#include "move.h"

#include "explore.h"
#include "map.h"
#include "rooms.h"
#include "state.h"
#include "view.h"

bool is_diagonal_step_allowed(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y)
{
    if (from_x == to_x || from_y == to_y) return true;

    return is_walkable_character(get_map_character(to_x, from_y)) && is_walkable_character(get_map_character(from_x, to_y));
}

bool move_player(int8_t step_x, int8_t step_y)
{
    // ide jön: medvecsapda, zavarodottságban véletlen irány

    // a 0. oszlop (sor) előtti mező 255 lesz, azt az is_on_map kívülinek veszi
    uint8_t player_x = game.player.x;
    uint8_t player_y = game.player.y;
    uint8_t target_x = player_x + step_x;
    uint8_t target_y = player_y + step_y;

    if (!is_on_map(target_x, target_y)) return false;
    if (!is_diagonal_step_allowed(player_x, player_y, target_x, target_y)) return false;

    // ide jön: a rejtett csapda felfedése, fogva tartás (ISHELD), szörny a célmezőn: támadás

    uint8_t target_character = get_map_character(target_x, target_y);
    if (!is_walkable_character(target_character)) return false;

    bool is_target_passage = (get_map_flags(target_x, target_y) & TILE_PASSAGE) != 0;
    bool is_player_passage = (get_map_flags(player_x, player_y) & TILE_PASSAGE) != 0;
    bool is_player_on_door = get_map_character(player_x, player_y) == CHAR_DOOR;

    draw_tile(player_x, player_y, calculate_character_left_behind());

    if (target_character == CHAR_DOOR && is_player_passage) {
        enter_room(target_x, target_y);
    } else if (is_target_passage && is_player_on_door) {
        leave_room(target_x, target_y);
    } else if (is_target_passage) {
        // folyosón belül, vagy labirintusba be, illetve onnan ki
        game.player.room = get_room_at(target_x, target_y);
    }

    game.player.x = target_x;
    game.player.y = target_y;
    follow_view_window(target_x);

    // ide jön: csapda kioldása, tárgy felvétele
    return true;
}
