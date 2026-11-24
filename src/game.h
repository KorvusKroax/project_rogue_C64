#ifndef GAME_H
#define GAME_H

void stop_running_and_repeating(void);  // a futás és a számprefixes ismétlés megszakítása (pl. sikeres keresés)
// időveszteség (4.2.3): egy üres kör a játékos akciója nélkül, a BEFORE és az AFTER fázissal, a gyűrűhatások nélkül; a játékos
// közben meghalhat (is_player_dead)
void waste_time(void);
// új játék: a játékos kezdőértékei és az első szint, majd a körök ismétlése a játékos haláláig;
// ideiglenesen (M10-01) utána a halál oka az üzenetsávban, és billentyűre vár
void run_game(void);

#endif
