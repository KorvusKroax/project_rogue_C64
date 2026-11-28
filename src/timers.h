#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

// a bájitalok időzítői (GameState.timers, a TIMER_ indexekkel, state.h) a kör AFTER fázisában egyet lépnek; amelyik 0-ra ér, annak
// a hatása véget ér, a TIMER_ sorrendben (4.1.1)
void tick_timers(void);

// az időzítő hátralévő köreihez spread(duration) kör adódik, így a nem futó elindul; a 16 bites számláló a legnagyobb értékénél megáll
void extend_timer(uint8_t timer, uint16_t duration);

// a hatás idő előtti vége: ha az időzítő fut, nullázódik, és a hatás úgy ér véget, mint lejáratkor (az üzenetével együtt)
void end_timer_early(uint8_t timer);

// a szöveg a hallucináció szerint (choose_str): hallucinálva a hallucinated_text, különben a normal_text
const char *choose_hallucinated_text(const char *normal_text, const char *hallucinated_text);

#endif
