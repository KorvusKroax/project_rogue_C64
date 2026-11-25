#ifndef TEXTS_H
#define TEXTS_H

// a játék szövegei; a \1, \2, \3 helyőrzők a message_format szerint

#include "items.h"
#include "monsters.h"
#include "player.h"

// a hossza fordításkor kell (a sáv utolsó sorában mindig marad neki hely), ezért makró
#define TEXT_MORE         "--More--"
#define TEXT_MORE_LENGTH  (sizeof(TEXT_MORE) - 1)

// a lista végén és oldalanként; a hossza fordításkor kell (az overwrite ablak legalább ilyen széles), ezért makró
#define TEXT_PRESS_SPACE         "--Press space to continue--"
#define TEXT_PRESS_SPACE_LENGTH  (sizeof(TEXT_PRESS_SPACE) - 1)

extern const char text_illegal_command[];
extern const char text_secret_door[];
extern const char text_no_way_down[];
extern const char text_no_way_up[];
extern const char text_way_magically_blocked[];
extern const char text_wrenching_sensation[];
extern const char text_no_command_yet[];
extern const char text_which_direction[];
extern const char text_no_monster_there[];
extern const char text_being_held[];
extern const char text_xeroc_revealed[];
extern const char text_found_gold[];                // \3: a mennyiség
extern const char text_you_now_have[];              // \1: a tárgy neve, \2: a betűje
extern const char text_no_room_in_pack[];
extern const char text_moved_onto[];                // \1: a tárgy neve
extern const char text_nothing_to_pick_up[];
extern const char text_empty_handed[];
extern const char text_which_object[];              // \1: a parancs célja, pl. text_item_purpose_drop
extern const char text_item_purpose_drop[];
extern const char text_not_carrying_anything[];
extern const char text_ran_out[];
extern const char text_not_valid_item[];            // \1: a leütött billentyű
extern const char text_something_there_already[];
extern const char text_dropped[];                   // \1: a tárgy neve
extern const char text_no_room_for_that[];
extern const char text_nothing_appropriate[];
extern const char text_item_purpose_call[];
extern const char text_cant_call_that[];
extern const char text_already_identified[];
extern const char text_no_room_for_names[];
extern const char text_was_called[];                // \1: a régi név
extern const char text_what_to_call_it[];
extern const char text_name_removed[];
extern const char text_discovery_question[];
extern const char text_discovery_type_error[];
extern const char text_nothing_discovered[];        // \1: a típus szava, pl. text_potion_kind
extern const char text_item_purpose_wield[];
extern const char text_item_purpose_wear[];
extern const char text_appears_cursed[];
extern const char text_cant_wield_armor[];
extern const char text_already_in_use[];
extern const char text_now_wielding[];              // \1: a tárgy neve, \2: a betűje
extern const char text_cant_wear_that[];
extern const char text_already_wearing[];
extern const char text_now_wearing[];               // \1: a páncél neve
extern const char text_not_wearing_armor[];
extern const char text_used_to_be_wearing[];        // \1: a páncél betűje, \2: a neve
extern const char text_wielding_item[];             // \1: a kézben tartott tárgy betűje, \2: a neve
extern const char text_wielding_nothing[];
extern const char text_wearing_item[];              // \1: a viselt páncél betűje, \2: a neve
extern const char text_wearing_nothing[];

// a harci üzenetek: a találat és a hiba szövege véletlenül választva a változatok közül; \1 a támadó, \2 a megtámadott
#define ATTACK_TEXT_VARIANT_COUNT  4
extern const char *const text_player_hits[ATTACK_TEXT_VARIANT_COUNT];
extern const char *const text_monster_hits[ATTACK_TEXT_VARIANT_COUNT];
extern const char *const text_player_misses[ATTACK_TEXT_VARIANT_COUNT];
extern const char *const text_monster_misses[ATTACK_TEXT_VARIANT_COUNT];
extern const char text_you[];
extern const char text_something[];                 // a nem látott szörny neve
extern const char text_monster_article[];           // a látott szörny neve elé
extern const char text_defeated[];
extern const char text_welcome_to_level[];
extern const char text_you_can_move_again[];
extern const char text_frozen_by[];
extern const char text_suddenly_weaker[];
extern const char text_bite_weaker[];
extern const char text_killed_by[];                 // ideiglenes, a játék végéig (M10-01); \1: névelő ("the " vagy üres), \2: a halál oka
extern const char *const text_death_causes[DEATH_CAUSE_COUNT];     // a nem szörny okozta halál okai, a DEATH_CAUSE_ kódok sorrendjében
extern const char *const text_monster_names[MONSTER_TYPE_COUNT];   // a szörnytípusok neve, a betűjük sorrendjében ('A'..'Z')

// a tárgytípusok valódi neve, az esélytáblájuk (items.c) sorrendjében
extern const char *const text_potion_names[POTION_SUBTYPE_COUNT];
extern const char *const text_scroll_names[SCROLL_SUBTYPE_COUNT];
extern const char *const text_weapon_names[WEAPON_SUBTYPE_COUNT];
extern const char *const text_armor_names[ARMOR_SUBTYPE_COUNT];
extern const char *const text_ring_names[RING_SUBTYPE_COUNT];
extern const char *const text_stick_names[STICK_SUBTYPE_COUNT];

// az álnevek listái (8.8); a GameState álnév-indexei ezekbe mutatnak
#define POTION_COLOR_COUNT     27
#define RING_STONE_COUNT       26
#define STICK_METAL_COUNT      22                                       // a közös anyaglista eleje: a fémek, belőlük pálca (wand) lesz
#define STICK_WOOD_COUNT       33                                       // utánuk a fák, belőlük bot (staff)
#define STICK_MATERIAL_COUNT   (STICK_METAL_COUNT + STICK_WOOD_COUNT)
#define SCROLL_SYLLABLE_COUNT  146
extern const char *const text_potion_colors[POTION_COLOR_COUNT];
extern const char *const text_ring_stones[RING_STONE_COUNT];
extern const char *const text_stick_materials[STICK_MATERIAL_COUNT];
extern const char *const text_scroll_syllables[SCROLL_SYLLABLE_COUNT];

// a tárgynevek szavai (item_names.c)
extern const char text_potion_kind[];
extern const char text_scroll_kind[];
extern const char text_ring_kind[];
extern const char text_wand_kind[];
extern const char text_staff_kind[];
extern const char text_stick_kind[];                // a pálca és a bot közös szava a felfedezések listájában
extern const char text_of[];                        // "a potion of healing"
extern const char text_called[];                    // "a potion called <név>(blue)", "a mace called <címke>"
extern const char text_titled[];                    // "a scroll titled 'zok bie'"
extern const char text_charges[];                   // "[5 charges]"
extern const char text_protection[];                // "[protection 4]"
extern const char text_some_food[];
extern const char text_rations_of_food[];           // "3 rations of food"
extern const char text_fruit_name[];                // a gyümölcs neve; amíg nincs beállítása, rögzített
extern const char text_gold_pieces[];               // "25 Gold pieces"
extern const char text_amulet_name[];
extern const char text_weapon_in_hand[];            // a használatban lévő tárgy utótagja: a kézben tartott tárgyé
extern const char text_being_worn[];                // a viselt páncélé

#endif
