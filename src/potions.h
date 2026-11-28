#ifndef POTIONS_H
#define POTIONS_H

#include <stdint.h>

// a bájitalok hatásának ideje (8.2, 4.1.1); a spread(n) kört kapó időzítő, ha már fut, ennyivel hosszabbodik
#define CONFUSION_DURATION          20          // spread(20) (default: 20)
#define HALLUCINATION_DURATION      850         // spread(850) (default: 850)
#define SEE_INVISIBLE_DURATION      850         // spread(850) (default: 850)
#define BLINDNESS_DURATION          850         // spread(850) (default: 850)
#define LEVITATION_DURATION         30          // spread(30) (default: 30)
#define MONSTER_DETECTION_DURATION  20          // pontosan ennyi kör; ha már fut, nem változik (default: 20)
#define HASTE_DURATION_MIN          4           // rnd(HASTE_DURATION_RANGE) + HASTE_DURATION_MIN kör (default: 4)
#define HASTE_DURATION_RANGE        4           // default: 4
#define HASTE_FAINT_TURNS_RANGE     8           // gyorsítva újra megitatva rnd(HASTE_FAINT_TURNS_RANGE) kör ájulás, 0 körnél nincs ájulás (default: 8)

// a méreg rnd(POISON_STRENGTH_LOSS_RANGE) + POISON_STRENGTH_LOSS_MIN erőt vesz el
#define POISON_STRENGTH_LOSS_MIN    1           // default: 1
#define POISON_STRENGTH_LOSS_RANGE  3           // default: 3

// a gyógyítás szintenként egy kockával: a healing ennyi oldalúval, az extra healing ennyivel
#define HEALING_DIE_SIDES        4              // default: 4
#define EXTRA_HEALING_DIE_SIDES  8              // default: 8

// `q`: a csomag tárgyának megivása; nem bájitalra „yuk! Why would you want to drink that?”, és a tárgy marad; különben egy darab
// elfogy (use_up_pack_item), utána a bájital hatása (8.2) az üzeneteivel, a leírt sorrendben, végül az automatikus azonosítás, és ha
// a típus így sem ismert, és nincs neve, a kérdése (ask_type_name); a hívó számára mindig időbe kerül (J14)
void quaff_potion(uint8_t index);

#endif
