#ifndef TIMERS_H
#define TIMERS_H

// a bájitalok időzítői (GameState.timers, a TIMER_ indexekkel, state.h) a kör AFTER fázisában egyet lépnek; amelyik 0-ra ér, annak
// a hatása véget ér, a TIMER_ sorrendben (4.1.1)
void tick_timers(void);

#endif
