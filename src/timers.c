#include "timers.h"

#include <stdint.h>

#include "message.h"
#include "rng.h"
#include "state.h"
#include "texts.h"

// méretre fordítva: a körönkénti ketyegés csak hét számláló, a lejárat és a hosszabbítás ritka; itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

const char *choose_hallucinated_text(const char *normal_text, const char *hallucinated_text)
{
    return game.timers[TIMER_HALLUCINATION] > 0 ? hallucinated_text : normal_text;
}

// a hatás vége, amikor az időzítője lejár; az egy körben lejáró hallucináció a sorrendben előtte állóknál még fut
static void end_timed_effect(uint8_t timer)
{
    switch (timer) {
        case TIMER_CONFUSION:
            message(choose_hallucinated_text(text_less_confused, text_less_trippy));
            break;
        case TIMER_HALLUCINATION:
            // vakon csak a hatás ér véget; ide jön: a látható tárgyak és szörnyek visszarajzolása a valódi jelükkel
            if (game.timers[TIMER_BLINDNESS] == 0) {
                message(text_looks_boring);
            }
            break;
        case TIMER_BLINDNESS:
            // ide jön: a képernyő frissítése
            message(choose_hallucinated_text(text_darkness_lifts, text_cosmic_again));
            break;
        case TIMER_LEVITATION:
            message(choose_hallucinated_text(text_float_to_ground, text_hit_the_ground));
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
