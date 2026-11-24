#ifndef DISK_H
#define DISK_H

#include <stdbool.h>
#include <stdint.h>

// a lemezkezelés (10.7) olvasó része; egyszerre egy fájl lehet nyitva

uint8_t get_game_device(void);                                  // az egység, amelyről a játék betöltődött; ha az nem lemezegység, a 8-as
bool open_file_for_reading(const char *name, uint8_t device);   // a név PETSCII; a hiányzó fájl és egység csak az olvasáskor derül ki
uint16_t read_file(char *buffer, uint16_t size);                // legfeljebb size bájt, a betöltési címmel kezdve; kevesebb, ha a fájl véget ért, vagy hiba volt
bool is_file_end_reached(void);                                 // a legutóbbi read_file a fájl utolsó bájtjáig olvasott, hiba nélkül
void close_file(void);

#endif
