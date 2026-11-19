#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>
#include <stdint.h>

void run_test_screens(void);        // a tesztképernyők menüje: mindegyik billentyűre indul, ESC-re vissza a menübe; nem tér vissza

// egy önteszt: az i. próbát futtatja, igaz = sikerült
// int, nem bool: az Oscar64 1.32.273 a függvénymutatón át hívott 8 bites visszatérési értéket rossz helyről olvassa
int test_RN(uint16_t i);
int test_rnd(uint16_t i);
int test_ROLL(uint16_t i);
int test_spread(uint16_t i);
int test_saturating_arithmetic(uint16_t i);
int test_format(uint16_t i);
int test_convert_text_to_number(uint16_t i);
int test_key_decode(uint16_t i);
int test_view_follow(uint16_t i);
int test_prepare_message_text(uint16_t i);
int test_message_chars_fitting_row(uint16_t i);
int test_message_chars_fitting_bar(uint16_t i);
int test_map_flags(uint16_t i);
int test_diagonal_step(uint16_t i);
int test_neighbor_visibility(uint16_t i);
int test_careful_run(uint16_t i);
int test_passage_turn(uint16_t i);
int test_search(uint16_t i);
int test_monster_types(uint16_t i);
int test_monster_table(uint16_t i);
int test_monster_order(uint16_t i);
int test_monster_generation(uint16_t i);
int test_monster_tiles(uint16_t i);
int test_monster_visibility(uint16_t i);
int test_monster_waking(uint16_t i);
int test_monster_chase(uint16_t i);
int test_strength_bonuses(uint16_t i);
int test_monster_names(uint16_t i);
int test_player_hit_rate(uint16_t i);
int test_monster_hit_rate(uint16_t i);
int test_move_attack(uint16_t i);
int test_monster_attack(uint16_t i);
int test_fight_diagonal(uint16_t i);
int test_fight_hit_limit(uint16_t i);
int test_fight_other_attack(uint16_t i);
int test_fight_target_moves(uint16_t i);
int test_monster_kill(uint16_t i);
int test_monster_removal(uint16_t i);
int test_fight_target_death(uint16_t i);
int test_experience_levels(uint16_t i);
int test_ice_monster(uint16_t i);
int test_vampire(uint16_t i);
int test_wraith(uint16_t i);
int test_rattlesnake(uint16_t i);
int test_strength_change(uint16_t i);
int test_flytrap(uint16_t i);
int test_flytrap_hold(uint16_t i);
int test_xeroc_reveal(uint16_t i);
int test_player_death(uint16_t i);

// szint-önteszt: hosszú, VICE-ben warp módban
#define LEVEL_TEST_LEVEL_COUNT  1000            // ennyi véletlen szint, rögzített magokkal
#define LEVEL_TEST_DEPTH_COUNT  4               // ennyi mélységen, felváltva (test_level.c)

#define LEVEL_MOVE_TEST_LEVEL_COUNT  100        // ennyi szinten lépnek a szörnyek, ugyanazokkal a magokkal, mint a generálás tesztjében

int test_level_generation(uint16_t i);          // az i. szint generálása és ellenőrzése: szobák, labirintusok, összekötés
int test_level_monster_moves(uint16_t i);       // az i. szinten körök a játékos lépésével, előbb 5, aztán minden szörny üldöz: a lépéseik után is érvényesek a szörnymezők; közben a kör idejét méri
bool show_level_room_ratios(void);              // mélységenként a sötét szobák és a labirintusok várt és kapott száma, a test_level_generation után; igaz, ha mind a tűrésen belül (leállított futásnál nem ítél, igazat ad)
bool show_level_generation_times(void);         // mélységenként a leglassabb szintgenerálás képkockában, a test_level_generation után; igaz, ha mind a 2 másodperces kereten belül
void show_level_turn_times(void);               // mélységenként egy kör ideje a képkocka százalékában, a test_level_monster_moves után

#endif
