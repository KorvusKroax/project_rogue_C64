#include "tests.h"

#include <stdbool.h>
#include <stdint.h>

#include "../game.h"
#include "../message.h"
#include "../monsters.h"
#include "../player.h"
#include "../state.h"

// egy próba: két időzítő ennyi körrel indul, és egy üres kör után ez a szöveg áll az üzenetsávban (üres szöveg: nincs üzenet)
struct TimerTestCase {
    uint8_t first_timer;
    uint8_t first_turns;
    uint8_t second_timer;
    uint8_t second_turns;
    const char *text;
};

static const struct TimerTestCase timer_test_cases[] = {
    // az egy körben lejárók üzenetei a TIMER_ sorrendben jönnek; a confusion végén a hallucináció még fut
    { TIMER_CONFUSION, 1, TIMER_HALLUCINATION, 1, "You feel less trippy now. Everything looks SO boring now." },
    { TIMER_CONFUSION, 1, TIMER_BLINDNESS, 1, "You feel less confused now. The veil of darkness lifts." },
    { TIMER_HALLUCINATION, 1, TIMER_LEVITATION, 1, "Everything looks SO boring now. You float gently to the ground." },
    { TIMER_BLINDNESS, 1, TIMER_LEVITATION, 1, "The veil of darkness lifts. You float gently to the ground." },
    { TIMER_LEVITATION, 1, TIMER_HASTE, 1, "You float gently to the ground. You feel yourself slowing down." },

    // vakon a hallucináció üzenet nélkül ér véget, akkor is, ha a vakság ugyanabban a körben jár le
    { TIMER_HALLUCINATION, 1, TIMER_BLINDNESS, 1, "The veil of darkness lifts." },
    { TIMER_HALLUCINATION, 1, TIMER_BLINDNESS, 2, "" },

    // hallucinálva a confusion, a vakság és a lebegés vége a hallucinált szöveggel jön, a gyorsításé ugyanaz
    { TIMER_CONFUSION, 1, TIMER_HALLUCINATION, 2, "You feel less trippy now." },
    { TIMER_BLINDNESS, 1, TIMER_HALLUCINATION, 2, "Far out! Everything is all cosmic again." },
    { TIMER_LEVITATION, 1, TIMER_HALLUCINATION, 2, "Bummer! You've hit the ground." },
    { TIMER_HASTE, 1, TIMER_HALLUCINATION, 2, "You feel yourself slowing down." },

    // a see invisible és a monster detection vége nem ad üzenetet
    { TIMER_SEE_INVISIBLE, 1, TIMER_MONSTER_DETECTION, 1, "" },

    // a még futó időzítő csak egyet lép
    { TIMER_HASTE, 2, TIMER_CONFUSION, 3, "" },
};

// a próba után a két időzítő egyet lépett, a többi 0 maradt
static bool are_timers_ticked(const struct TimerTestCase *test_case)
{
    for (uint8_t timer = 0; timer < TIMER_COUNT; timer++) {
        uint16_t expected = 0;
        if (timer == test_case->first_timer) {
            expected = test_case->first_turns - 1;
        } else if (timer == test_case->second_timer) {
            expected = test_case->second_turns - 1;
        }
        if (game.timers[timer] != expected) return false;
    }
    return true;
}

int test_timer_expiry(uint16_t i)
{
    bool ok = true;
    clear_monsters();

    for (uint8_t index = 0; index < sizeof(timer_test_cases) / sizeof(timer_test_cases[0]); index++) {
        const struct TimerTestCase *test_case = &timer_test_cases[index];
        init_player();
        message_clear();
        game.timers[test_case->first_timer] = test_case->first_turns;
        game.timers[test_case->second_timer] = test_case->second_turns;

        waste_time();
        ok = ok && is_message_bar_text(test_case->text) && are_timers_ticked(test_case);
    }

    message_clear();
    init_player();
    return ok;
}
