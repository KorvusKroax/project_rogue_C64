#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <stdbool.h>

#define MESSAGE_ROW        0    // az üzenetsáv első képernyősora (default: 0)
#define MESSAGE_ROW_COUNT  2    // az üzenetsáv sorai; a második sortól a térkép felső sorait takarja (default: 1)

// a sávba, az eddigi szöveg után; ha nem fér el, --More--, és szóközre vár; a tárgylista-sor (pl. "a) a mace") nem fűződik
// össze mással: ha a sávban szöveg van, előtte, és ha utána üzenet vagy kérdés jön, utána is --More--
void message(const char *text);
void message_format(const char *text, const char *first_text, const char *second_text, int16_t number);    // mint a message, a \1, \2 helyén a két szöveg, a \3 helyén a szám; a nem használt szöveg lehet NULL
void message_format_without_recall(const char *text, const char *first_text, const char *second_text, int16_t number);    // mint a message_format, de a CTRL-P nem hozza vissza
// mint a message, de ha előtte --More-- kell, ott ESC is leüthető: ekkor a szöveg elmarad, a sáv üres, és az eredmény hamis
// (a slow lista megszakítása, text_list.h)
bool message_unless_escaped(const char *text);
void repeat_last_message(void);                 // az utolsó üzenet (a kérdések nem) újra a sávba; ha még nem volt, üres
void message_prompt(const char *text);          // kérdés a sávba, záró pont nélkül; a képernyő kurzora a szöveg végén marad; a válasszal látszik, amíg a következő üzenet vagy törlés le nem váltja
void message_prompt_format(const char *text, const char *first_text);  // mint a message_prompt, a \1 helyén a szöveg
void message_reserve_input_space(uint8_t length);   // a kurzortól length karakter helyét lefoglalja a sávban, legfeljebb a sáv utolsó soráig; az újonnan lefoglalt sorok üresek
// mint a message_prompt, utána message_reserve_input_space; az eddigi szöveghez csak akkor fűződik, ha mögötte a length hosszú mező is elfér,
// különben --More-- után a sáv elején kezdődik
void message_prompt_with_input_space(const char *text, uint8_t input_length);
void message_clear(void);                       // a sáv törlése; a második sortól visszajön a térkép
uint8_t wait_for_key_and_clear_message(void);   // vár egy leütésre, törli a sávot, és a leütés kódját adja

const char *prepare_message_text(const char *text, const char *first_text, const char *second_text, int16_t number, bool add_end_character);    // helyőrzők kitöltése, nagybetűs kezdés és igény szerint záró pont; a tárgylista-sor változatlan; az eredmény a következő hívásig él
uint8_t calculate_message_chars_fitting_row(const char *text, uint8_t width);   // ennyi karakter kerül a szövegből egy width széles sorba; nem rajzol
uint16_t calculate_message_chars_fitting_bar(const char *text);                 // ennyi karakter fér a sávba úgy, hogy a --More-- is elférjen; nem rajzol

#ifdef DEVMODE
    // a tesztprogramoknak: kikapcsolva a --More-- nem vár szóközre, a teli sáv törlődik, és az üzenetek mennek tovább (alapból bekapcsolva)
    void set_message_more_wait(bool enabled);
#endif

#endif
