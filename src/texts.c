#include "texts.h"

const char text_illegal_command[] = "illegal command '\1'";
const char text_secret_door[] = "a secret door";
const char text_no_way_down[] = "I see no way down";
const char text_no_way_up[] = "I see no way up";
const char text_way_magically_blocked[] = "your way is magically blocked";
const char text_no_command_yet[] = "you haven't typed a command yet";
const char text_which_direction[] = "which direction?";
const char text_no_monster_there[] = "I see no monster there";
const char text_being_held[] = "you are being held";
const char text_xeroc_revealed[] = "wait! That's a xeroc!";
const char text_found_gold[] = "you found \3 gold pieces";
const char text_you_now_have[] = "you now have \1 (\2)";
const char text_no_room_in_pack[] = "there's no room in your pack";
const char text_moved_onto[] = "you moved onto \1";
const char text_nothing_to_pick_up[] = "there is nothing here to pick up";
const char text_empty_handed[] = "you are empty handed";
const char text_which_object[] = "which object do you want to \1? (* for list): ";
const char text_item_purpose_drop[] = "drop";
const char text_not_carrying_anything[] = "you aren't carrying anything";
const char text_ran_out[] = "you ran out";
const char text_not_valid_item[] = "'\1' is not a valid item";
const char text_something_there_already[] = "there is something there already";
const char text_dropped[] = "dropped \1";
const char text_no_room_for_that[] = "there's no room for that";

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
const char text_you_can_move_again[] = "you can move again";
const char text_frozen_by[] = "you are frozen by \1";
const char text_suddenly_weaker[] = "you suddenly feel weaker";
const char text_bite_weaker[] = "you feel a bite in your leg and now feel weaker";
const char text_killed_by[] = "killed by \1\2";

const char *const text_death_causes[DEATH_CAUSE_COUNT] = { "hypothermia" };

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

const char *const text_potion_names[POTION_SUBTYPE_COUNT] = {
    "confusion",
    "hallucination",
    "poison",
    "gain strength",
    "see invisible",
    "healing",
    "monster detection",
    "magic detection",
    "raise level",
    "extra healing",
    "haste self",
    "restore strength",
    "blindness",
    "levitation",
};

const char *const text_scroll_names[SCROLL_SUBTYPE_COUNT] = {
    "monster confusion",
    "magic mapping",
    "hold monster",
    "sleep",
    "enchant armor",
    "identify potion",
    "identify scroll",
    "identify weapon",
    "identify armor",
    "identify ring, wand or staff",
    "scare monster",
    "food detection",
    "teleportation",
    "enchant weapon",
    "create monster",
    "remove curse",
    "aggravate monsters",
    "protect armor",
};

const char *const text_weapon_names[WEAPON_SUBTYPE_COUNT] = {
    "mace",
    "long sword",
    "short bow",
    "arrow",
    "dagger",
    "two handed sword",
    "dart",
    "shuriken",
    "spear",
};

const char *const text_armor_names[ARMOR_SUBTYPE_COUNT] = {
    "leather armor",
    "ring mail",
    "studded leather",
    "scale mail",
    "chain mail",
    "splint mail",
    "banded mail",
    "plate mail",
};

const char *const text_ring_names[RING_SUBTYPE_COUNT] = {
    "protection",
    "add strength",
    "sustain strength",
    "searching",
    "see invisible",
    "adornment",
    "aggravate monster",
    "dexterity",
    "increase damage",
    "regeneration",
    "slow digestion",
    "teleportation",
    "stealth",
    "maintain armor",
};

const char *const text_stick_names[STICK_SUBTYPE_COUNT] = {
    "light",
    "invisibility",
    "lightning",
    "fire",
    "cold",
    "polymorph",
    "magic missile",
    "haste monster",
    "slow monster",
    "drain life",
    "nothing",
    "teleport away",
    "teleport to",
    "cancellation",
};

const char *const text_potion_colors[POTION_COLOR_COUNT] = {
    "amber", "aquamarine", "black", "blue", "brown", "clear", "crimson", "cyan", "ecru",
    "gold", "green", "grey", "magenta", "orange", "pink", "plaid", "purple", "red",
    "silver", "tan", "tangerine", "topaz", "turquoise", "vermilion", "violet", "white", "yellow",
};

const char *const text_ring_stones[RING_STONE_COUNT] = {
    "agate", "alexandrite", "amethyst", "carnelian", "diamond", "emerald", "germanium",
    "granite", "garnet", "jade", "kryptonite", "lapis lazuli", "moonstone", "obsidian",
    "onyx", "opal", "pearl", "peridot", "ruby", "sapphire", "stibotantalite",
    "tiger eye", "topaz", "turquoise", "taaffeite", "zircon",
};

const char *const text_stick_materials[STICK_MATERIAL_COUNT] = {
    // fémek (STICK_METAL_COUNT)
    "aluminum", "beryllium", "bone", "brass", "bronze", "copper", "electrum", "gold",
    "iron", "lead", "magnesium", "mercury", "nickel", "pewter", "platinum", "steel",
    "silver", "silicon", "tin", "titanium", "tungsten", "zinc",
    // fák (STICK_WOOD_COUNT)
    "avocado wood", "balsa", "bamboo", "banyan", "birch", "cedar", "cherry", "cinnibar",
    "cypress", "dogwood", "driftwood", "ebony", "elm", "eucalyptus", "fall", "hemlock",
    "holly", "ironwood", "kukui wood", "mahogany", "manzanita", "maple", "oaken",
    "persimmon wood", "pecan", "pine", "poplar", "redwood", "rosewood", "spruce", "teak",
    "walnut", "zebrawood",
};

const char *const text_scroll_syllables[SCROLL_SYLLABLE_COUNT] = {
    "a", "ab", "ag", "aks", "ala", "an", "app", "arg", "arze", "ash", "bek", "bie",
    "bit", "bjor", "blu", "bot", "bu", "byt", "comp", "con", "cos", "cre", "dalf", "dan",
    "den", "do", "e", "eep", "el", "eng", "er", "ere", "erk", "esh", "evs", "fa",
    "fid", "fri", "fu", "gan", "gar", "glen", "gop", "gre", "ha", "hyd", "i", "ing",
    "ip", "ish", "it", "ite", "iv", "jo", "kho", "kli", "klis", "la", "lech", "mar",
    "me", "mi", "mic", "mik", "mon", "mung", "mur", "nej", "nelg", "nep", "ner", "nes",
    "nih", "nin", "o", "od", "ood", "org", "orn", "ox", "oxy", "pay", "ple", "plu",
    "po", "pot", "prok", "re", "rea", "rhov", "ri", "ro", "rog", "rok", "rol", "sa",
    "san", "sat", "sef", "seh", "shu", "ski", "sna", "sne", "snik", "sno", "so", "sol",
    "sri", "sta", "sun", "ta", "tab", "tem", "ther", "ti", "tox", "trol", "tue", "turs",
    "u", "ulk", "um", "un", "uni", "ur", "val", "viv", "vly", "vom", "wah", "wed",
    "werg", "wex", "whon", "wun", "xo", "y", "yot", "yu", "zant", "zeb", "zim", "zok",
    "zon", "zum",
};

const char text_potion_kind[] = "potion";
const char text_scroll_kind[] = "scroll";
const char text_ring_kind[] = "ring";
const char text_wand_kind[] = "wand";
const char text_staff_kind[] = "staff";
const char text_of[] = " of ";
const char text_titled[] = " titled ";
const char text_charges[] = " charges";
const char text_protection[] = "protection ";
const char text_some_food[] = "some food";
const char text_rations_of_food[] = " rations of food";
const char text_fruit_name[] = "slime-mold";
const char text_gold_pieces[] = " Gold pieces";
const char text_amulet_name[] = "the Amulet of Yendor";
