#ifndef TEXTS_H
#define TEXTS_H

// a játék szövegei; a \1, \2, \3 helyőrzők a message_format szerint

#include "monsters.h"

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
extern const char *const text_hunger_states[];     // a hunger_state szerint: 0 rendben (üres), 1 Hungry, 2 Weak, 3 Faint
extern const char *const text_monster_names[MONSTER_TYPE_COUNT];   // a szörnytípusok neve, a betűjük sorrendjében ('A'..'Z')

#endif
