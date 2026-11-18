#include "texts.h"

const char text_illegal_command[] = "illegal command '\1'";
const char text_secret_door[] = "a secret door";
const char text_no_way_down[] = "I see no way down";
const char text_no_way_up[] = "I see no way up";
const char text_way_magically_blocked[] = "your way is magically blocked";
const char text_no_command_yet[] = "you haven't typed a command yet";
const char text_which_direction[] = "which direction?";
const char text_no_monster_there[] = "I see no monster there";

static const char text_excellent_hit[] = "\1 scored an excellent hit on \2";
static const char text_plain_hit[] = "\1 hit \2";

const char *const text_player_hits[ATTACK_TEXT_VARIANT_COUNT] = { text_excellent_hit, text_plain_hit, "\1 have injured \2", "\1 swing and hit \2" };
const char *const text_monster_hits[ATTACK_TEXT_VARIANT_COUNT] = { text_excellent_hit, text_plain_hit, "\1 has injured \2", "\1 swings and hits \2" };
const char *const text_player_misses[ATTACK_TEXT_VARIANT_COUNT] = { "\1 miss \2", "\1 swing and miss \2", "\1 barely miss \2", "\1 don't hit \2" };
const char *const text_monster_misses[ATTACK_TEXT_VARIANT_COUNT] = { "\1 misses \2", "\1 swings and misses \2", "\1 barely misses \2", "\1 doesn't hit \2" };
const char text_you[] = "you";
const char text_something[] = "something";
const char text_monster_article[] = "the ";
const char text_defeated[] = "you have defeated \1";
const char text_welcome_to_level[] = "welcome to level \3";

const char *const text_hunger_states[] = { "", "Hungry", "Weak", "Faint" };

const char *const text_monster_names[MONSTER_TYPE_COUNT] = {
    "aquator",
    "bat",
    "centaur",
    "dragon",
    "emu",
    "venus flytrap",
    "griffin",
    "hobgoblin",
    "ice monster",
    "jabberwock",
    "kestrel",
    "leprechaun",
    "medusa",
    "nymph",
    "orc",
    "phantom",
    "quagga",
    "rattlesnake",
    "snake",
    "troll",
    "black unicorn",
    "vampire",
    "wraith",
    "xeroc",
    "yeti",
    "zombie",
};
