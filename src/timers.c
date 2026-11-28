#include "timers.h"

#include <stdint.h>

#include "message.h"
#include "rng.h"
#include "state.h"
#include "texts.h"

// méretre fordítva: a körönkénti ketyegés csak hét számláló, a lejárat és a hosszabbítás ritka; itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

// a hatás vége, amikor az időzítője lejár
static void end_timed_effect(uint8_t timer)
{
    switch (timer) {
        case TIMER_CONFUSION:
            message(text_less_confused);
            break;
        case TIMER_HALLUCINATION:
            // vakon csak a hatás ér véget; ide jön: a látható tárgyak és szörnyek visszarajzolása a valódi jelükkel
            if (game.timers[TIMER_BLINDNESS] == 0) {
                message(text_looks_boring);
            }
            break;
        case TIMER_BLINDNESS:
            // ide jön: a képernyő frissítése
            message(text_darkness_lifts);
            break;
        case TIMER_LEVITATION:
            message(text_float_to_ground);
            break;
        case TIMER_HASTE:
            message(text_slowing_down);
            break;
        // ide jön: a see invisible végén a látható láthatatlan szörnyek eltűnése, ha nincs see invisible gyűrű; a monster detection
        // végén a nem látott szörnyek eltűnése
    }
}

void tick_timers(void)
{
    for (uint8_t timer = 0; timer < TIMER_COUNT; timer++) {
        if (game.timers[timer] > 0) {
            game.timers[timer]--;
            if (game.timers[timer] == 0) {
                end_timed_effect(timer);
            }
        }
    }
}

void extend_timer(uint8_t timer, uint16_t duration)
{
    uint16_t turns = spread(duration);
    uint16_t sum = game.timers[timer] + turns;
    game.timers[timer] = sum < turns ? UINT16_MAX : sum;
}

void end_timer_early(uint8_t timer)
{
    if (game.timers[timer] == 0) return;
    game.timers[timer] = 0;
    end_timed_effect(timer);
}

#pragma optimize(pop)
