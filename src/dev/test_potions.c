#include "tests.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../platform/key.h"
#include "../game.h"
#include "../item_names.h"
#include "../items.h"
#include "../map.h"
#include "../message.h"
#include "../monsters.h"
#include "../pack.h"
#include "../player.h"
#include "../potions.h"
#include "../rng.h"
#include "../rooms.h"
#include "../state.h"

// a játékos helye: a mellette álló szörny látszik
#define POTION_PLAYER_X  10
#define POTION_PLAYER_Y  (MAP_TOP_ROW + 2)

#define POTION_TEST_MONSTER  'Z'

// a próbák előtt: a kezdőértékű játékos az 1. mélységen, üres térképen, üres csomaggal, szörny nélkül; az üzenetsáv üres;
// minden bájitaltípus azonosítva, így az ivás után nincs kérdés, amelyre a tesztnek leütést kellene adnia
static void put_potion_scene(void)
{
    clear_map();
    clear_monsters();
    clear_item_table();
    game.dungeon_level = 1;
    init_player();
    game.player.x = POTION_PLAYER_X;
    game.player.y = POTION_PLAYER_Y;
    game.player.room = get_room_at(POTION_PLAYER_X, POTION_PLAYER_Y);
    memset(game.potion_knowledge, SUBTYPE_IDENTIFIED, sizeof(game.potion_knowledge));
    message_clear();
}

// a próbák után: a játékos kezdőértékekkel, nincs szörny és tárgy, a bájitaltípusok ismeretlenek, a névkészlet üres
static void clear_potion_scene(void)
{
    message_clear();
    clear_monsters();
    clear_item_table();
    clear_map();
    init_player();
    memset(game.potion_knowledge, SUBTYPE_UNKNOWN, sizeof(game.potion_knowledge));
    memset(game.name_pool, 0, sizeof(game.name_pool));
}

// egy új which bájital a csomagba; az eredmény a csomag tárgya, amelyben most van
static uint8_t add_potion_to_pack(uint8_t which)
{
    return add_item_to_pack(create_item(ITEM_TYPE_POTION, which));
}

// egy új which bájital megivása
static void drink_new_potion(uint8_t which)
{
    quaff_potion(add_potion_to_pack(which));
}

#define QUAFF_TEST_STACK_COUNT  3               // a kézben tartott bájitalköteg darabszáma

int test_quaff_item(uint16_t i)
{
    put_potion_scene();
    uint8_t food = add_item_to_pack(create_item(ITEM_TYPE_FOOD, FOOD_RATION));

    // a `*` lista csak a bájitalokat mutatja
    bool ok = !show_pack_list(PACK_FILTER_POTION) && does_message_bar_start_with("You don't have anything appropriate.");

    // a nem bájital a csomagban marad
    message_clear();
    quaff_potion(food);
    ok = ok && is_message_bar_text("Yuk! Why would you want to drink that?") && is_item_in_pack(food) && get_item(food)->count == 1;

    // a köteg a kezdőértékű játékosnak ártalmatlan bájitalból: a restore strength nem változtat semmin
    uint8_t potion = NO_ITEM;
    for (uint8_t count = 0; count < QUAFF_TEST_STACK_COUNT; count++) {
        potion = add_potion_to_pack(POTION_RESTORE_STRENGTH);
    }
    message_clear();
    ok = ok && show_pack_list(PACK_FILTER_POTION) && does_message_bar_start_with("b) ");

    // a kötegből egy darab fogy, és a kéz akkor is kiürül, ha a kötegből marad
    game.wielded_item = potion;
    uint8_t item_count = game.item_count;
    quaff_potion(potion);
    ok = ok && is_item_in_pack(potion) && get_item(potion)->count == QUAFF_TEST_STACK_COUNT - 1 && game.wielded_item == NO_ITEM;
    ok = ok && game.item_count == item_count;

    // az utolsó darab után a tárgy kikerül a csomagból, és a rekordja felszabadul
    for (uint8_t count = 1; count < QUAFF_TEST_STACK_COUNT; count++) {
        quaff_potion(potion);
    }
    ok = ok && !is_item_in_pack(potion) && game.item_count == item_count - 1;

    clear_potion_scene();
    return ok;
}

#define TIMER_BIT(timer)    (1 << (timer))
#define BLINDNESS_BIT       TIMER_BIT(TIMER_BLINDNESS)
#define HALLUCINATION_BIT   TIMER_BIT(TIMER_HALLUCINATION)
#define HASTE_BIT           TIMER_BIT(TIMER_HASTE)

#if TIMER_COUNT > 8
    #error "az időzítők bitjei nem férnek egy bájtba"
#endif

#define POTION_TEST_TIMER_TURNS  100            // a próba előtt futó időzítők ennyi körrel indulnak

// egy próba: az ivás előtt a running_timers időzítői futnak; az ivás után ez a szöveg áll az üzenetsávban, az ended_timers időzítői 0-k,
// a többi futó változatlan
struct PotionMessageTestCase {
    uint8_t which;
    uint8_t running_timers;                     // TIMER_BIT-ek
    uint8_t ended_timers;
    const char *text;
};

static const struct PotionMessageTestCase potion_message_test_cases[] = {
    { POTION_CONFUSION, 0, 0, "Wait, what's going on here. Huh? What? Who?" },
    { POTION_HALLUCINATION, 0, 0, "Oh, wow! Everything seems so cosmic!" },
    { POTION_POISON, 0, 0, "You feel very sick now." },
    { POTION_POISON, HALLUCINATION_BIT, HALLUCINATION_BIT, "You feel very sick now. Everything looks SO boring now." },
    // vakon a hallucináció csendben ér véget
    { POTION_POISON, HALLUCINATION_BIT | BLINDNESS_BIT, HALLUCINATION_BIT, "You feel very sick now." },
    { POTION_GAIN_STRENGTH, 0, 0, "You feel stronger, now. What bulging muscles!" },
    { POTION_SEE_INVISIBLE, 0, 0, "This potion tastes like slime-mold juice." },
    // az első üzenet két sorra törik, és a második már nem fér mellé a --More---val: a tesztben a sáv törlődik, így csak a második
    // marad; fordított sorrendben mindkettő elférne
    { POTION_SEE_INVISIBLE, BLINDNESS_BIT, BLINDNESS_BIT, "The veil of darkness lifts." },
    { POTION_HEALING, 0, 0, "You begin to feel better." },
    { POTION_HEALING, BLINDNESS_BIT, BLINDNESS_BIT, "The veil of darkness lifts. You begin to feel better." },
    // a healing a hallucinációt nem szünteti meg
    { POTION_HEALING, HALLUCINATION_BIT, 0, "You begin to feel better." },
    { POTION_MONSTER_DETECTION, 0, 0, "You have a strange feeling for a moment, then it passes." },
    { POTION_RAISE_LEVEL, 0, 0, "You suddenly feel much more skillful. Welcome to level 2." },
    { POTION_EXTRA_HEALING, 0, 0, "You begin to feel much better." },
    { POTION_EXTRA_HEALING, BLINDNESS_BIT, BLINDNESS_BIT, "The veil of darkness lifts. You begin to feel much better." },
    { POTION_EXTRA_HEALING, HALLUCINATION_BIT, HALLUCINATION_BIT, "Everything looks SO boring now. You begin to feel much better." },
    // a három üzenet nem fér a sávba: az első kettő után --More-- jön (a tesztben a sáv törlődik), így csak az utolsó marad; ha
    // a hallucináció a vakság előtt érne véget, csendes lenne, és a másik kettő elférne a sávban
    { POTION_EXTRA_HEALING, HALLUCINATION_BIT | BLINDNESS_BIT, HALLUCINATION_BIT | BLINDNESS_BIT, "You begin to feel much better." },
    { POTION_HASTE_SELF, 0, 0, "You feel yourself moving much faster." },
    { POTION_HASTE_SELF, HASTE_BIT, HASTE_BIT, "You faint from exhaustion." },
    { POTION_RESTORE_STRENGTH, 0, 0, "Hey, this tastes great. It makes you feel warm all over." },
    { POTION_BLINDNESS, 0, 0, "A cloak of darkness falls around you." },
    { POTION_LEVITATION, 0, 0, "You start to float in the air." },
    { POTION_MAGIC_DETECTION, 0, 0, "You have a strange feeling for a moment, then it passes." },

    // hallucinálva a hallucinált változatok
    { POTION_CONFUSION, HALLUCINATION_BIT, 0, "What a trippy feeling!" },
    { POTION_MONSTER_DETECTION, HALLUCINATION_BIT, 0, "You have a normal feeling for a moment, then it passes." },
    { POTION_MAGIC_DETECTION, HALLUCINATION_BIT, 0, "You have a normal feeling for a moment, then it passes." },
    { POTION_BLINDNESS, HALLUCINATION_BIT, 0, "Oh, bummer! Everything is dark! Help!" },
    { POTION_LEVITATION, HALLUCINATION_BIT, 0, "Oh, wow! You're floating in the air!" },
    { POTION_HEALING, HALLUCINATION_BIT | BLINDNESS_BIT, BLINDNESS_BIT, "Far out! Everything is all cosmic again. You begin to feel better." },
};

// a próba futó időzítői az ivás után: a véget érők 0-k, a többi változatlan
static bool are_running_timers_right(const struct PotionMessageTestCase *test_case)
{
    for (uint8_t timer = 0; timer < TIMER_COUNT; timer++) {
        if (!(test_case->running_timers & TIMER_BIT(timer))) continue;
        uint16_t expected = (test_case->ended_timers & TIMER_BIT(timer)) ? 0 : POTION_TEST_TIMER_TURNS;
        if (game.timers[timer] != expected) return false;
    }
    return true;
}

int test_potion_messages(uint16_t i)
{
    put_potion_scene();
    bool ok = true;

    for (uint8_t index = 0; index < sizeof(potion_message_test_cases) / sizeof(potion_message_test_cases[0]); index++) {
        const struct PotionMessageTestCase *test_case = &potion_message_test_cases[index];
        init_player();
        for (uint8_t timer = 0; timer < TIMER_COUNT; timer++) {
            if (test_case->running_timers & TIMER_BIT(timer)) {
                game.timers[timer] = POTION_TEST_TIMER_TURNS;
            }
        }
        message_clear();
        drink_new_potion(test_case->which);
        ok = ok && is_message_bar_text(test_case->text) && are_running_timers_right(test_case);
    }

    // a szörnyérzékelés üzenete csak akkor jön, ha nincs nem látott szörny: a szomszédos szörny látszik, a láthatatlan nem
    init_player();
    create_monster(POTION_TEST_MONSTER, POTION_PLAYER_X + 1, POTION_PLAYER_Y);
    message_clear();
    drink_new_potion(POTION_MONSTER_DETECTION);
    ok = ok && is_message_bar_text("You have a strange feeling for a moment, then it passes.");
    game.monsters[0].flags |= MONSTER_INVISIBLE;
    message_clear();
    drink_new_potion(POTION_MONSTER_DETECTION);
    ok = ok && is_message_bar_empty();

    clear_potion_scene();
    return ok;
}

#define MAGIC_TEST_FLOOR_X  (POTION_PLAYER_X + 3)   // a padlón fekvő tárgy helye

// a magic detection megivása elnevezett típussal, így kérdés egyik esetben sincs: ha van mit mutatni, azonosítódik, a neve felszabadul,
// és nincs üzenet; különben a neve marad, és „you have a strange feeling for a moment, then it passes”; utána a típus újra azonosított
static bool is_magic_detection_right(bool is_shown)
{
    uint8_t name_index = store_name("x");
    game.potion_knowledge[POTION_MAGIC_DETECTION] = name_index;
    message_clear();
    drink_new_potion(POTION_MAGIC_DETECTION);

    bool ok;
    if (is_shown) {
        ok = game.potion_knowledge[POTION_MAGIC_DETECTION] == SUBTYPE_IDENTIFIED && is_message_bar_empty();
        ok = ok && store_name("y") == name_index;
    } else {
        ok = game.potion_knowledge[POTION_MAGIC_DETECTION] == name_index;
        ok = ok && is_message_bar_text("You have a strange feeling for a moment, then it passes.");
    }
    free_name(name_index);
    game.potion_knowledge[POTION_MAGIC_DETECTION] = SUBTYPE_IDENTIFIED;
    return ok;
}

int test_magic_detection(uint16_t i)
{
    put_potion_scene();

    // üres padlónál semmi, akkor sem, ha egy szörnynél mágikus tárgy van
    create_monster(POTION_TEST_MONSTER, POTION_PLAYER_X + 1, POTION_PLAYER_Y);
    uint8_t carried = create_item(ITEM_TYPE_POTION, POTION_HEALING);
    insert_item_after(&game.monsters[0].first_pack_item, NO_ITEM, carried);
    bool ok = is_magic_detection_right(false);

    // ha a padlón van tárgy, akár nem mágikus is (a létrehozott buzogány +0-s), a szörny mágikus tárgya számít
    uint8_t mace = create_item(ITEM_TYPE_WEAPON, WEAPON_MACE);
    put_item_on_floor(mace, MAGIC_TEST_FLOOR_X, POTION_PLAYER_Y);
    ok = ok && is_magic_detection_right(true);

    // a szörny tárgya nélkül a nem mágikus padló nem mutat semmit; a bónuszos fegyver mágikus
    free_item_list(&game.monsters[0].first_pack_item);
    ok = ok && is_magic_detection_right(false);
    get_item(mace)->properties.weapon.hit_bonus = 1;
    ok = ok && is_magic_detection_right(true);

    clear_potion_scene();
    return ok;
}

// a kérdés leütései
#define NAMING_KEY_ESCAPE  "\033"
#define NAMING_KEY_RETURN  "\r"

// egy új which bájital megivása a keys leütéseivel; igaz, ha a leütések mind elfogytak, vagyis jött a kérdés, és válaszolt rá
static bool drink_new_potion_with_keys(uint8_t which, const char *keys)
{
    set_test_keys(keys);
    drink_new_potion(which);
    bool ok = read_key() == 0;
    set_test_keys(NULL);
    return ok;
}

// iváskor azonosítódik-e a típus, az üres padlós jelenetben, hallucináció nélkül (8.2)
static bool is_identified_by_drinking(uint8_t which)
{
    return which != POTION_SEE_INVISIBLE && which != POTION_MONSTER_DETECTION && which != POTION_MAGIC_DETECTION
        && which != POTION_RESTORE_STRENGTH;
}

int test_potion_identification(uint16_t i)
{
    put_potion_scene();
    memset(game.potion_knowledge, SUBTYPE_UNKNOWN, sizeof(game.potion_knowledge));
    bool ok = true;

    // ami iváskor azonosítódik, azt nem kérdezi; a többinél jön a kérdés, és ESC-re a típus név nélkül marad
    for (uint8_t which = 0; which < POTION_SUBTYPE_COUNT; which++) {
        init_player();
        bool is_asked = drink_new_potion_with_keys(which, NAMING_KEY_ESCAPE);
        if (is_identified_by_drinking(which)) {
            ok = ok && !is_asked && game.potion_knowledge[which] == SUBTYPE_IDENTIFIED;
        } else {
            ok = ok && is_asked && game.potion_knowledge[which] == SUBTYPE_UNKNOWN;
        }
    }

    // hallucinálva a confusion nem azonosítódik
    game.potion_knowledge[POTION_CONFUSION] = SUBTYPE_UNKNOWN;
    init_player();
    game.timers[TIMER_HALLUCINATION] = POTION_TEST_TIMER_TURNS;
    ok = ok && drink_new_potion_with_keys(POTION_CONFUSION, NAMING_KEY_ESCAPE) && game.potion_knowledge[POTION_CONFUSION] == SUBTYPE_UNKNOWN;
    init_player();

    // a beírt szöveg a típus neve lesz, utána az elnevezett típus nem kérdez; üres beírásra nincs név
    ok = ok && drink_new_potion_with_keys(POTION_SEE_INVISIBLE, "ab" NAMING_KEY_RETURN);
    uint8_t name_index = game.potion_knowledge[POTION_SEE_INVISIBLE];
    ok = ok && name_index != NO_NAME && name_index != SUBTYPE_IDENTIFIED && strcmp(get_name(name_index), "ab") == 0;
    ok = ok && !drink_new_potion_with_keys(POTION_SEE_INVISIBLE, NAMING_KEY_ESCAPE);
    ok = ok && game.potion_knowledge[POTION_SEE_INVISIBLE] == name_index;
    ok = ok && drink_new_potion_with_keys(POTION_RESTORE_STRENGTH, NAMING_KEY_RETURN);
    ok = ok && game.potion_knowledge[POTION_RESTORE_STRENGTH] == SUBTYPE_UNKNOWN;

    // az ivással azonosított típus neve felszabadul
    uint8_t healing_name_index = store_name("x");
    game.potion_knowledge[POTION_HEALING] = healing_name_index;
    ok = ok && !drink_new_potion_with_keys(POTION_HEALING, NAMING_KEY_ESCAPE);
    ok = ok && game.potion_knowledge[POTION_HEALING] == SUBTYPE_IDENTIFIED && store_name("y") == healing_name_index;

    // betelt névkészletnél a kérdés helyett az üzenet jön (a „strange feeling” mellé nem fér, ezért új sávban)
    while (store_name("f") != NO_NAME) {
    }
    message_clear();
    ok = ok && !drink_new_potion_with_keys(POTION_MONSTER_DETECTION, NAMING_KEY_ESCAPE);
    ok = ok && game.potion_knowledge[POTION_MONSTER_DETECTION] == SUBTYPE_UNKNOWN;
    ok = ok && is_message_bar_text("You have no room for more names. Remove a name from something first.");

    clear_potion_scene();
    return ok;
}

// a bájital, amelynek ideje ismételt iváskor összeadódik, az időzítője és az ideje
struct PotionDurationTestCase {
    uint8_t which;
    uint8_t timer;
    uint16_t duration;
};

static const struct PotionDurationTestCase potion_duration_test_cases[] = {
    { POTION_CONFUSION, TIMER_CONFUSION, CONFUSION_DURATION },
    { POTION_HALLUCINATION, TIMER_HALLUCINATION, HALLUCINATION_DURATION },
    { POTION_SEE_INVISIBLE, TIMER_SEE_INVISIBLE, SEE_INVISIBLE_DURATION },
    { POTION_BLINDNESS, TIMER_BLINDNESS, BLINDNESS_DURATION },
    { POTION_LEVITATION, TIMER_LEVITATION, LEVITATION_DURATION },
};

// az ivás után az időzítő a before-nál a spread(duration) tartományába eső körrel több
static bool is_timer_extended(uint8_t timer, uint16_t before, uint16_t duration)
{
    uint16_t added = game.timers[timer] - before;
    uint16_t added_min = duration - duration / 20;
    return added >= added_min && added < added_min + duration / 10;
}

int test_potion_durations(uint16_t i)
{
    put_potion_scene();
    bool ok = true;

    // ismételt iváskor az idő összeadódik
    for (uint8_t index = 0; index < sizeof(potion_duration_test_cases) / sizeof(potion_duration_test_cases[0]); index++) {
        const struct PotionDurationTestCase *test_case = &potion_duration_test_cases[index];
        init_player();
        drink_new_potion(test_case->which);
        uint16_t first_turns = game.timers[test_case->timer];
        ok = ok && is_timer_extended(test_case->timer, 0, test_case->duration);
        drink_new_potion(test_case->which);
        ok = ok && is_timer_extended(test_case->timer, first_turns, test_case->duration);
    }

    // a monster detection ideje rögzített, és ha már fut, nem változik
    init_player();
    drink_new_potion(POTION_MONSTER_DETECTION);
    ok = ok && game.timers[TIMER_MONSTER_DETECTION] == MONSTER_DETECTION_DURATION;
    game.timers[TIMER_MONSTER_DETECTION] = 1;
    drink_new_potion(POTION_MONSTER_DETECTION);
    ok = ok && game.timers[TIMER_MONSTER_DETECTION] == 1;

    // a poison és az extra healing a hallucináció időzítőjét is törli, így az utána megivott hallucináció a teljes idejéig tart (J35)
    init_player();
    drink_new_potion(POTION_HALLUCINATION);
    drink_new_potion(POTION_POISON);
    ok = ok && game.timers[TIMER_HALLUCINATION] == 0;
    drink_new_potion(POTION_HALLUCINATION);
    ok = ok && is_timer_extended(TIMER_HALLUCINATION, 0, HALLUCINATION_DURATION);
    drink_new_potion(POTION_EXTRA_HEALING);
    ok = ok && game.timers[TIMER_HALLUCINATION] == 0;

    // a gyorsítás ideje; gyorsítva újra megitatva a gyorsítás véget ér, a játékos elájulhat, és utána nem jön „you feel yourself
    // slowing down”
    init_player();
    drink_new_potion(POTION_HASTE_SELF);
    uint16_t haste_turns = game.timers[TIMER_HASTE];
    ok = ok && haste_turns >= HASTE_DURATION_MIN && haste_turns < HASTE_DURATION_MIN + HASTE_DURATION_RANGE && game.turns_unable_to_act == 0;
    drink_new_potion(POTION_HASTE_SELF);
    ok = ok && game.timers[TIMER_HASTE] == 0 && game.turns_unable_to_act < HASTE_FAINT_TURNS_RANGE;
    message_clear();
    for (uint16_t turn = 0; turn < haste_turns; turn++) {
        waste_time();
    }
    ok = ok && is_message_bar_empty();

    clear_potion_scene();
    return ok;
}

#define POTION_TEST_MAX_HIT_POINTS  100         // a gyógyítás próbáiban a max. HP

// a gyógyítás egy próbája a level szinten, a max. HP alatt missing ponttal: a HP és a max. HP a dobás szerint (a dobást ugyanazzal
// a seeddel előre kiszámolja); a HP a dobással nő, ha így a max. HP fölé ér, a max. HP eggyel, can_gain_two mellett, ha a max. HP
// + szint + 1-et is meghaladta, kettővel nő, és a HP annyi lesz
static bool is_healing_right(uint8_t which, uint8_t die_sides, bool can_gain_two, uint8_t level, int16_t missing)
{
    game.player.experience_level = level;
    game.player.max_hit_points = POTION_TEST_MAX_HIT_POINTS;
    game.player.hit_points = POTION_TEST_MAX_HIT_POINTS - missing;
    uint8_t potion = add_potion_to_pack(which);

    uint32_t seed = game.seed;
    int16_t hit_points = game.player.hit_points + ROLL(level, die_sides);
    game.seed = seed;
    int16_t max_hit_points = POTION_TEST_MAX_HIT_POINTS;
    if (hit_points > max_hit_points) {
        if (can_gain_two && hit_points > max_hit_points + level + 1) {
            max_hit_points++;
        }
        max_hit_points++;
        hit_points = max_hit_points;
    }

    quaff_potion(potion);
    return game.player.hit_points == hit_points && game.player.max_hit_points == max_hit_points;
}

#define HEALING_TEST_MISSING_STEP  7            // a próbánként változó hiány lépése
#define HEALING_TEST_MISSING_MAX   30           // a hiány legfeljebb ennyi − 1

int test_potion_values(uint16_t i)
{
    put_potion_scene();
    game.seed = i + 1;

    // poison: 1..3 erő vész el, a legnagyobb erő marad
    drink_new_potion(POTION_POISON);
    uint8_t loss = INITIAL_STRENGTH - game.player.strength;
    bool ok = loss >= POISON_STRENGTH_LOSS_MIN && loss < POISON_STRENGTH_LOSS_MIN + POISON_STRENGTH_LOSS_RANGE;
    ok = ok && game.max_strength == INITIAL_STRENGTH;

    // restore strength: az erő a legnagyobb erő; gain strength: mindkettő eggyel nő; utána a restore strength nem változtat
    drink_new_potion(POTION_RESTORE_STRENGTH);
    ok = ok && game.player.strength == INITIAL_STRENGTH;
    drink_new_potion(POTION_GAIN_STRENGTH);
    ok = ok && game.player.strength == INITIAL_STRENGTH + 1 && game.max_strength == INITIAL_STRENGTH + 1;
    drink_new_potion(POTION_RESTORE_STRENGTH);
    ok = ok && game.player.strength == INITIAL_STRENGTH + 1;

    // a gyógyítás próbánként más szinten és más hiánnyal: hol a max. HP alatt marad, hol fölé ér
    uint8_t level = i % EXPERIENCE_LEVEL_MAX + 1;
    int16_t missing = (i * HEALING_TEST_MISSING_STEP) % HEALING_TEST_MISSING_MAX;
    ok = ok && is_healing_right(POTION_HEALING, HEALING_DIE_SIDES, false, level, missing);
    ok = ok && is_healing_right(POTION_EXTRA_HEALING, EXTRA_HEALING_DIE_SIDES, true, level, missing);

    // raise level: az XP a következő szint küszöbe + 1, és a szint eggyel nő, a 20. szintről is
    init_player();
    drink_new_potion(POTION_RAISE_LEVEL);
    ok = ok && game.player.experience == experience_thresholds[0] + 1 && game.player.experience_level == 2;
    game.player.experience = experience_thresholds[EXPERIENCE_THRESHOLD_COUNT - 2];
    game.player.experience_level = EXPERIENCE_LEVEL_MAX - 1;
    drink_new_potion(POTION_RAISE_LEVEL);
    ok = ok && game.player.experience == experience_thresholds[EXPERIENCE_THRESHOLD_COUNT - 1] + 1;
    ok = ok && game.player.experience_level == EXPERIENCE_LEVEL_MAX;

    // a 21. szinten csak az üzenet jön, a szint és az XP nem változik (J36)
    uint32_t experience = game.player.experience;
    message_clear();
    drink_new_potion(POTION_RAISE_LEVEL);
    ok = ok && game.player.experience == experience && game.player.experience_level == EXPERIENCE_LEVEL_MAX;
    ok = ok && is_message_bar_text("You suddenly feel much more skillful.");

    clear_potion_scene();
    return ok;
}
