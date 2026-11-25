#include "chase.h"

#include <stdint.h>
#include <stdbool.h>

#include "combat.h"
#include "explore.h"
#include "items.h"
#include "map.h"
#include "monsters.h"
#include "move.h"
#include "num.h"
#include "player.h"
#include "rng.h"
#include "rooms.h"
#include "state.h"
#include "view.h"

// a mező a játékos körüli 3×3-ban van (a játékos mezője is)
static bool is_beside_player(uint8_t x, uint8_t y)
{
    int8_t offset_x = x - game.player.x;
    int8_t offset_y = y - game.player.y;
    return offset_x >= -1 && offset_x <= 1 && offset_y >= -1 && offset_y <= 1;
}

// a szörny a from mezőről a szomszédos x, y mezőre léphet: a térképen van, járható, nem áll rajta szörny (az álcázott xeroc sem),
// és átlósan nem sarkon át; a játékos mezője is ilyen, oda támadni lép
static bool is_monster_step_allowed(uint8_t from_x, uint8_t from_y, uint8_t x, uint8_t y)
{
    if (!is_on_map(x, y)) return false;
    if (!is_diagonal_step_allowed(from_x, from_y, x, y)) return false;
    if (!is_walkable_character(get_map_character(x, y))) return false;
    if (get_map_flags(x, y) & TILE_MONSTER) return false;
    // ide jön: scare monster tekercsre nem lép
    return true;
}

// zavarodott lépés: egy véletlen szomszédos mező, vagy a saját mező, ha oda nem léphet (rndmove)
static void choose_random_step(const struct Monster *monster, uint8_t *x, uint8_t *y)
{
    // előbb az y, aztán az x; a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
    uint8_t random_y = monster->y + rnd(3) - 1;
    uint8_t random_x = monster->x + rnd(3) - 1;

    *x = monster->x;
    *y = monster->y;
    if (is_monster_step_allowed(monster->x, monster->y, random_x, random_y)) {
        *x = random_x;
        *y = random_y;
    }
}

// a szobatábla rekordjának kijárata-e: szobánál az ajtaja, folyosórendszernél a hozzá csatlakozó ajtó
static bool is_exit_of_room(const struct Exit *exit, uint8_t room_index)
{
    if (room_index < ROOM_COUNT) return exit->room == room_index;
    return exit->passage_number != NO_PASSAGE_NUMBER && PASSAGE_ROOM_INDEX(exit->passage_number) == room_index;
}

// a lépés célja: ha a célpont a szörny szobájában van, maga a célpont; különben a szoba kijáratai közül a célponthoz legközelebbi;
// ajtón állva a folyosórendszeré is, mert az ajtó mindkettőhöz tartozik: ha a célpont a folyosórendszerben van, maga a célpont,
// különben a folyosórendszer kijárata, ha közelebb van a szoba legjobb kijáratánál
static void choose_chase_goal(const struct Monster *monster, uint8_t *goal_x, uint8_t *goal_y)
{
    uint8_t destination_x = game.player.x;
    uint8_t destination_y = game.player.y;
    uint8_t destination_room = game.player.room;
    if (monster->destination_item != DESTINATION_PLAYER) {
        const struct FloorPosition *position = &get_item(monster->destination_item)->location.floor_position;
        destination_x = position->x;
        destination_y = position->y;
        destination_room = get_room_at(destination_x, destination_y);
    }

    uint8_t room_index = monster->room;
    bool is_on_door = get_map_character(monster->x, monster->y) == CHAR_DOOR;
    uint16_t best_distance = UINT16_MAX;
    *goal_x = destination_x;
    *goal_y = destination_y;

    // a célpont másik szobában (folyosórendszerben) van, és a szint összefüggő, ezért a ciklus talál kijáratot
    while (room_index != destination_room) {
        for (uint8_t exit_index = 0; exit_index < game.exit_count; exit_index++) {
            const struct Exit *exit = &game.exits[exit_index];
            if (!is_exit_of_room(exit, room_index)) continue;

            uint16_t distance = calculate_distance_squared(exit->x, exit->y, destination_x, destination_y);
            if (distance < best_distance) {
                best_distance = distance;
                *goal_x = exit->x;
                *goal_y = exit->y;
            }
        }
        if (!is_on_door) return;

        room_index = PASSAGE_ROOM_INDEX(get_map_flags(monster->x, monster->y) & TILE_PASSAGE_NUMBER);
        is_on_door = false;
    }

    *goal_x = destination_x;
    *goal_y = destination_y;
    // ide jön: a sárkány tüze, ha a játékos vele egy vonalban és lőtávolságon belül van; ilyenkor a sárkány nem lép
}

// a következő mező a cél felé: a szomszédok közül a célhoz legközelebbi, az egyformán közeliek közül egyenletes eséllyel;
// ha egyik sem közelebb, a saját mező; zavarodottan (a phantom és a denevér időnként magától is) véletlen mező;
// igaz, ha megérkezett: a mező maga a cél vagy a játékosé (chase)
static bool choose_chase_step(struct Monster *monster, uint8_t goal_x, uint8_t goal_y, uint8_t *step_x, uint8_t *step_y)
{
    uint8_t best_x = monster->x;
    uint8_t best_y = monster->y;
    uint16_t best_distance;

    bool is_confused_step = (monster->flags & MONSTER_CONFUSED) && rnd(5) != 0;
    if (is_confused_step || (monster->type == 'P' && rnd(5) == 0) || (monster->type == 'B' && rnd(2) == 0)) {
        choose_random_step(monster, &best_x, &best_y);
        if (rnd(20) == 0) {
            monster->flags &= ~MONSTER_CONFUSED;
        }
        best_distance = calculate_distance_squared(best_x, best_y, goal_x, goal_y);
    } else {
        best_distance = calculate_distance_squared(monster->x, monster->y, goal_x, goal_y);
        // a saját mező az első az egyformán jók között
        uint8_t tie_count = 1;

        // oszloponként, azon belül soronként; a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
                uint8_t x = monster->x + offset_x;
                uint8_t y = monster->y + offset_y;
                // a saját mezőn a TILE_MONSTER bit van, így az kimarad
                if (!is_monster_step_allowed(monster->x, monster->y, x, y)) continue;

                uint16_t distance = calculate_distance_squared(x, y, goal_x, goal_y);
                if (distance < best_distance) {
                    best_x = x;
                    best_y = y;
                    best_distance = distance;
                    tie_count = 1;
                } else if (distance == best_distance) {
                    tie_count++;
                    if (rnd(tie_count) == 0) {
                        best_x = x;
                        best_y = y;
                    }
                }
            }
        }
    }

    *step_x = best_x;
    *step_y = best_y;
    return best_distance == 0 || (best_x == game.player.x && best_y == game.player.y);
}

// az új mező emléke (oldch): a képernyő jele; sötét szobában (folyosón is) üres mező, ha az új vagy az előző emlék padló;
// különben a játékos körüli 3×3-ban a MAP jele
static uint8_t calculate_character_below(const struct Monster *monster, uint8_t x, uint8_t y)
{
    uint8_t character = get_seen_character(x, y);

    // ide jön: vakon a képernyő jele marad
    bool is_dark = (game.rooms[monster->room].flags & ROOM_DARK) != 0;
    if (is_dark && (monster->character_below == CHAR_FLOOR || character == CHAR_FLOOR)) return CHAR_SOLID_ROCK;
    if (is_beside_player(x, y)) return get_map_character(x, y);
    return character;
}

// az x, y mezőn fekszik-e a szörny célpont tárgya
static bool is_destination_item_at(const struct Monster *monster, uint8_t x, uint8_t y)
{
    if (monster->destination_item == DESTINATION_PLAYER) return false;

    const struct FloorPosition *position = &get_item(monster->destination_item)->location.floor_position;
    return position->x == x && position->y == y;
}

// a szörny felveszi a célpont tárgyat, mielőtt a mezőjére lép: a tárgy a csomagjába kerül, a mező jele padló vagy folyosó lesz,
// és ha a játékos látja a mezőt, azonnal így rajzolódik ki, hogy a szörny alá már ez kerüljön (J32); utána új célpontot választ
static void pick_up_destination_item(struct Monster *monster)
{
    uint8_t index = monster->destination_item;
    const struct FloorPosition *position = &get_item(index)->location.floor_position;
    uint8_t x = position->x;
    uint8_t y = position->y;

    take_item_from_floor(index);
    insert_item_after(&monster->first_pack_item, NO_ITEM, index);
    if (is_tile_visible(x, y)) {
        draw_tile(x, y, get_map_character(x, y));
    }
    monster->destination_item = choose_monster_destination(monster);
}

// a szörny az x, y mezőre lép, ha az nem a saját mezője: a régi mezőn az emléke jelenik meg, az új mezőnek új emléke lesz;
// végül a jele kirajzolódik, ha a játékos látja (relocate)
static void relocate_monster(struct Monster *monster, uint8_t x, uint8_t y)
{
    if (x != monster->x || y != monster->y) {
        draw_tile(monster->x, monster->y, monster->character_below);
        remove_map_flags(monster->x, monster->y, TILE_MONSTER);
        monster->room = get_room_at(x, y);
        monster->character_below = calculate_character_below(monster, x, y);
        monster->x = x;
        monster->y = y;
        add_map_flags(x, y, TILE_MONSTER);
    }

    if (is_monster_visible(monster)) {
        draw_tile(x, y, monster->disguise);
    }
    // ide jön: szörnyészleléssel (SEEMONST) a nem látható szörny típusa inverzen
}

// egy lépés a célpont felé; a játékos mellé érve megtámadja, a célpont tárgyat felveszi (do_chase)
static void take_chase_step(struct Monster *monster)
{
    // a játékos halála után a kör hátralévő részében már egy szörny sem lép (a gyorsított és a repülő második lépése sem)
    if (is_player_dead()) return;

    // ide jön: a kapzsi szörny célpontja a játékos, ha a szobájában már nincs arany
    uint8_t goal_x;
    uint8_t goal_y;
    choose_chase_goal(monster, &goal_x, &goal_y);

    // a játékos mezőjére sosem lép: ha a legjobb mező az, megtámadja; a játékos célpontnál ez a cél is, tárgy célpontnál a játékos
    // az útjában áll (J33); a többi megérkezésnél a lépés maga a cél
    uint8_t step_x;
    uint8_t step_y;
    if (choose_chase_step(monster, goal_x, goal_y, &step_x, &step_y)) {
        if (step_x == game.player.x && step_y == game.player.y) {
            attack_player(monster);
            return;
        }
        // ide jön: a szoba aranyát a kapzsi szörny nem veszi fel, hanem ráül, és a lépés után nem fut tovább (ISRUN le)
        if (is_destination_item_at(monster, goal_x, goal_y)) {
            pick_up_destination_item(monster);
        }
    } else if (monster->type == 'F') {
        // a Venus flytrap nem mozdul el
        return;
    }

    relocate_monster(monster, step_x, step_y);
}

// a lassított szörny minden második körben lép, a gyorsított kétszer (move_monst)
static void take_monster_turn(struct Monster *monster)
{
    if (!(monster->flags & MONSTER_SLOW) || (monster->flags & MONSTER_SLOW_TURN)) {
        take_chase_step(monster);
    }
    // ide jön: ha a szörny meghalt, nincs több lépése
    if (monster->flags & MONSTER_HASTED) {
        take_chase_step(monster);
    }
    monster->flags ^= MONSTER_SLOW_TURN;
}

void move_monsters(void)
{
    for (uint8_t index = game.monster_count; index != NO_MONSTER; index--) {
        struct Monster *monster = get_monster(index);
        if ((monster->flags & (MONSTER_RUNNING | MONSTER_HELD)) != MONSTER_RUNNING) continue;

        bool is_fight_target = (monster->flags & MONSTER_TARGET) != 0;
        uint8_t old_x = monster->x;
        uint8_t old_y = monster->y;

        // ide jön: ha a szörny meghalt (pl. a játékost megtámadva eltűnt), a következő jön
        take_monster_turn(monster);
        // a repülő a játékostól távol kétszer lép
        if ((monster->flags & MONSTER_FLYING) && !is_beside_player(monster->x, monster->y)) {
            take_monster_turn(monster);
        }

        // az f-harc célpontja elmozdult
        if (is_fight_target && (monster->x != old_x || monster->y != old_y)) {
            stop_fight();
        }
    }
}
