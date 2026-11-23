#ifndef TEXT_LIST_H
#define TEXT_LIST_H

#include <stdbool.h>
#include <stdint.h>

// 40 oszlopos listák (csomaglista, * lista, felfedezések; 4.2.6): a 40 karakternél hosszabb tétel szóhatáron törik,
// a folytatósor ennyi szóközzel beljebb kezdődik (az "a) " alá)
#define TEXT_LIST_CONTINUATION_INDENT  3

// a lista index. tétele (0-tól), pl. "a) a mace"; az üres tétel üres sor (a felfedezések listái között); NULL, ha nincs több;
// az eredmény a következő hívásig él
typedef const char *(*TextListEntryFunction)(uint8_t index);

struct TextListSize {
    uint8_t entry_count;                        // a tételek száma
    uint8_t row_count;                          // a tördelt sorok száma, legfeljebb SCREEN_HEIGHT: ennél több már nem fér el
    uint8_t width;                              // a leghosszabb sor, legalább a TEXT_PRESS_SPACE hossza
};

void calculate_text_list_size(TextListEntryFunction get_entry, struct TextListSize *size);

// overwrite módban a képernyőre fér: balra egy üres oszlop, alul a TEXT_PRESS_SPACE sora
bool does_text_list_fit_window(const struct TextListSize *size);

// a lista megjelenítése az inven opció szerint; legalább egy tétel kell, és egyetlen tétel üzenetként jelenik meg:
// - clear: külön teljes képernyőn, a 0. sortól; ami nem fér a 0..23. sorba, a következő oldalra kerül;
//   a 24. sorban TEXT_PRESS_SPACE, és szóközre vár
// - overwrite: a térkép fölött, a jobb felső sarokban; az utolsó sora a TEXT_PRESS_SPACE; ha nem fér el, clear módban
// - slow: tételenként az üzenetsávban, közöttük --More--; ott ESC-re a lista véget ér; az üres tétel kimarad
// a clear és az overwrite mód ideiglenes ablak (view.h): a végén a képernyő visszaáll
void show_text_list(TextListEntryFunction get_entry);

#endif
