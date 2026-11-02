#ifndef KEY_H
#define KEY_H

#include <stdint.h>

// a nem nyomtatható logikai billentyűk kódja; a CTRL + betű 1..26
#define KEY_RETURN  13
#define KEY_ESCAPE  27
#define KEY_DELETE  127

#define KEY_COLUMNS    8
#define KEY_MODIFIER_SHIFT  0x01 // bal vagy jobb SHIFT, SHIFT LOCK
#define KEY_MODIFIER_CTRL   0x02 // CTRL vagy C=

// a key_irq tölti: a key_matrix[oszlop] bitjei a sorok (1 = lenyomva), a módosítók nélkül
extern volatile uint8_t key_matrix[KEY_COLUMNS];
extern volatile uint8_t key_modifiers;

void key_init(void);
__interrupt void key_irq(void);  // képkockánként egyszer, a raszter-IRQ-ból
uint8_t read_key(void);           // ASCII kód, 0 ha nincs leütés
uint8_t wait_for_key(void);       // vár egy leütésre, és annak kódját adja
void wait_for_keys_released(void);    // a még lenyomott billentyűt a KERNAL kilépés után a BASIC-be írná

#ifdef DEVMODE
    // a mátrixhely (oszlop * 8 + sor) kódja a módosítókkal, 0 ha nincs; az önteszthez
    uint8_t key_decode(uint8_t position, uint8_t modifiers);
#endif

#endif
