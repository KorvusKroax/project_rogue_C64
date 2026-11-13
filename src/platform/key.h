#ifndef KEY_H
#define KEY_H

#include <stdint.h>
#include <stdbool.h>

// a nem nyomtatható logikai billentyűk kódja; a CTRL + betű 1..26
#define KEY_RETURN  13
#define KEY_ESCAPE  27
#define KEY_DELETE  127
#define KEY_CTRL(letter)  ((letter) - 'a' + 1)   // a CTRL + kisbetű kódja

// az F-billentyűk kódja (a PETSCII-é); csak az enable_function_keys után jutnak el a játékhoz
#define KEY_F1  133
#define KEY_F3  134
#define KEY_F5  135
#define KEY_F7  136
#define KEY_F2  137
#define KEY_F4  138
#define KEY_F6  139
#define KEY_F8  140
#define is_function_key(code)  ((code) >= KEY_F1 && (code) <= KEY_F8)

#define KEY_COLUMNS    8            // a billentyűmátrix oszlopai
#define KEY_ROWS       8            // és sorai: egy oszlop egy bájt
#define KEY_MODIFIER_SHIFT  0x01    // bal vagy jobb SHIFT, SHIFT LOCK
#define KEY_MODIFIER_CTRL   0x02    // CTRL vagy C=

// a key_irq tölti: a key_matrix[oszlop] bitjei a sorok (1 = lenyomva), a módosítók nélkül
extern volatile uint8_t key_matrix[KEY_COLUMNS];
extern volatile uint8_t key_modifiers;

void key_init(void);
__interrupt void key_irq(void);     // képkockánként egyszer, a raszter-IRQ-ból
uint8_t read_key(void);             // ASCII kód, 0 ha nincs leütés
uint8_t wait_for_key(void);         // vár egy leütésre, és annak kódját adja
void set_key_repeat(bool enabled);  // a nyomva tartott billentyű ismétlése; csak a parancsra várakozás idejére kapcsolandó be
void enable_function_keys(void);    // az F-billentyűk kódot adnak; addig kód nélküliek, mint a kurzorbillentyűk

#endif
