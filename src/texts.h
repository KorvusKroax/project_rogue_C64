#ifndef TEXTS_H
#define TEXTS_H

// a játék szövegei; a \1, \2, \3 helyőrzők a message_format szerint

#include "items.h"
#include "monsters.h"
#include "player.h"

// a hossza fordításkor kell (a sáv utolsó sorában mindig marad neki hely), ezért makró
#define TEXT_MORE         "--More--"
#define TEXT_MORE_LENGTH  (sizeof(TEXT_MORE) - 1)

extern const char text_illegal_command[];
extern const char text_secret_door[];
extern const char text_no_way_down[];
extern const char text_no_way_up[];
extern const char text_way_magically_blocked[];
extern const char text_no_command_yet[];
extern const char text_which_direction[];
extern const char text_no_monster_there[];
extern const char text_being_held[];
extern const char text_xeroc_revealed[];

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
extern const char *const text_hunger_states[];     // a hunger_state szerint: 0 rendben (üres), 1 Hungry, 2 Weak, 3 Faint
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
extern const char text_of[];                        // "a potion of healing"
extern const char text_titled[];                    // "a scroll titled 'zok bie'"
extern const char text_charges[];                   // "[5 charges]"
extern const char text_protection[];                // "[protection 4]"
extern const char text_some_food[];
extern const char text_rations_of_food[];           // "3 rations of food"
extern const char text_fruit_name[];                // a gyümölcs neve; amíg nincs beállítása, rögzített
extern const char text_gold_pieces[];               // "25 Gold pieces"
extern const char text_amulet_name[];

#endif
