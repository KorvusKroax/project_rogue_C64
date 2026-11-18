#ifndef GAME_H
#define GAME_H

void stop_running_and_repeating(void);  // a futás és a számprefixes ismétlés megszakítása (pl. sikeres keresés)
// új játék: a játékos kezdőértékei és az első szint, majd a körök ismétlése a játékos haláláig;
// ideiglenesen (M10-01) utána a halál oka az üzenetsávban, és billentyűre vár
void run_game(void);

#endif
