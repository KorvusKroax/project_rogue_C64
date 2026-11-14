#include "move.h"

#include "explore.h"
#include "map.h"
#include "rooms.h"
#include "state.h"
#include "view.h"

// a futás állapota: fut-e, merre, és óvatos (CTRL) futás-e
static bool is_run_active;
static int8_t run_step_x;
static int8_t run_step_y;
static bool is_run_careful;

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

    if (target_character == CHAR_DOOR || target_character == CHAR_STAIRS) {
        stop_running();
    }

    // ide jön: csapda kioldása, tárgy felvétele
    return true;
}

void start_running(int8_t step_x, int8_t step_y, bool is_careful)
{
    is_run_active = true;
    run_step_x = step_x;
    run_step_y = step_y;
    is_run_careful = is_careful;
}

void stop_running(void)
{
    is_run_active = false;
}

bool is_running(void)
{
    return is_run_active;
}

// befordulás után az első bejárású kanyar megjelölése
static void mark_passage_turn(void)
{
    if (get_map_flags(game.player.x, game.player.y) & TILE_SEEN) return;

    add_map_flags(game.player.x, game.player.y, TILE_SEEN);
    // ide jön: jump_to_run_end mellett egyszeri képernyőfrissítés
}

bool take_run_step(void)
{
    // ide jön: vakon nincs kanyarkövetés
    if ((options.flags & OPTION_FOLLOW_PASSAGES) && find_passage_turn(game.player.x, game.player.y, &run_step_x, &run_step_y)) {
        mark_passage_turn();
    }

    if (!move_player(run_step_x, run_step_y)) {
        stop_running();
        return false;
    }

    // ide jön: vakon a CTRL-futás is SHIFT-futás
    if (is_run_careful && is_something_interesting_beside(game.player.x, game.player.y, run_step_x, run_step_y)) {
        stop_running();
    }
    return true;
}

// a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
static bool is_passage_turn_allowed(uint8_t x, uint8_t y)
{
    if (!is_on_map(x, y)) return false;
    if (get_map_character(x, y) == CHAR_DOOR) return true;
    return (get_map_flags(x, y) & (TILE_PASSAGE | TILE_REAL)) == (TILE_PASSAGE | TILE_REAL);
}

bool find_passage_turn(uint8_t x, uint8_t y, int8_t *step_x, int8_t *step_y)
{
    if (!(get_map_flags(x, y) & TILE_PASSAGE)) return false;
    if (*step_x != 0 && *step_y != 0) return false;

    uint8_t target_x = x + *step_x;
    uint8_t target_y = y + *step_y;
    if (is_on_map(target_x, target_y) && is_walkable_character(get_map_character(target_x, target_y))) return false;

    // a két derékszögű oldalirány: (side_x, side_y) és az ellentéte
    int8_t side_x = *step_y;
    int8_t side_y = *step_x;
    bool is_side_allowed = is_passage_turn_allowed(x + side_x, y + side_y);
    bool is_opposite_side_allowed = is_passage_turn_allowed(x - side_x, y - side_y);
    if (is_side_allowed == is_opposite_side_allowed) return false;

    if (is_side_allowed) {
        *step_x = side_x;
        *step_y = side_y;
    } else {
        *step_x = -side_x;
        *step_y = -side_y;
    }
    return true;
}

bool is_something_interesting_beside(uint8_t x, uint8_t y, int8_t step_x, int8_t step_y)
{
    uint8_t orthogonal_passage_count = 0;

    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            if (offset_x == 0 && offset_y == 0) continue;

            // a hátsó oldal: ortogonális futásnál a hátsó oszlop vagy sor, átlósnál a hátsó félsík három mezője
            if (offset_x * step_x + offset_y * step_y < 0) continue;

            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t tile_x = x + offset_x;
            uint8_t tile_y = y + offset_y;
            if (!is_on_map(tile_x, tile_y)) continue;
            if (!is_neighbor_visible(x, y, tile_x, tile_y)) continue;

            bool is_orthogonal = offset_x == 0 || offset_y == 0;
            // ide jön: a látható szörny
            uint8_t character = get_map_character(tile_x, tile_y);
            if (character == CHAR_DOOR) {
                if (is_orthogonal) return true;
            } else if (character == CHAR_PASSAGE) {
                if (is_orthogonal) orthogonal_passage_count++;
            } else if (character != CHAR_FLOOR && character != CHAR_WALL_HORIZONTAL && character != CHAR_WALL_VERTICAL && character != CHAR_SOLID_ROCK) {
                return true;
            }
        }
    }

    return orthogonal_passage_count > 1;
}
