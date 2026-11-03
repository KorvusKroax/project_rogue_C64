#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <stdbool.h>

#define MESSAGE_ROW  0      // az üzenetsáv első képernyősora; a második a MESSAGE_ROW + 1

void message(const char *text);                 // a sávba, az eddigi szöveg után; ha nem fér el, --More--, és szóközre vár
void message_prompt(const char *text);          // kérdés a sávba, záró pont nélkül; a képernyő kurzora a szöveg végén marad
void message_reserve_input_space(uint8_t length);   // ha a kurzortól length karakter nem fér a sorba, lefoglalja és törli a második sort
void message_finish_prompt(void);               // a kérdés a válasszal látszik, amíg a következő üzenet vagy törlés le nem váltja
void message_clear(void);                       // a sáv törlése; a második sor helyén visszajön a térkép
uint8_t wait_for_key_and_clear_message(void);   // vár egy leütésre, törli a sávot, és a leütés kódját adja

const char *prepare_message_text(const char *text, bool add_end_character);     // nagybetűs kezdés és igény szerint záró pont; a tárgylista-sor változatlan; az eredmény a következő hívásig él
uint8_t calculate_message_chars_fitting_row(const char *text, uint8_t width);   // ennyi karakter kerül a szövegből egy width széles sorba; nem rajzol
uint8_t calculate_message_chars_fitting_bar(const char *text);                  // ennyi karakter fér a sávba úgy, hogy a --More-- is elférjen; nem rajzol

#endif
