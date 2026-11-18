#ifndef DEV_H
#define DEV_H

#include <stdint.h>
#include <stdbool.h>

#ifdef DEVMODE
    void dev_fail(const char *message, const char *file, int line); // hamis feltételnél: üzenet + forrásbeli hely, majd a program megáll
    #define DEV_CHECK(cond, message) do { if (!(cond)) dev_fail(message, __FILE__, __LINE__); } while (0)

    void dev_run_start_screen(void);    // a seed kiírása, S-re kézi megadása; RETURN-re visszatér
    bool dev_execute_command(uint8_t code); // fejlesztői parancs játék közben, F-billentyűre: F1 új szint, F2 szörny a játékos mellé betű alapján, F3 ugrás mélységre, F4 a szint összes szörnyének üldözése, F5 teljes térkép, F6 a szint összes szörnyének lassítása ki és be, F7 a jump_to_run_end, F8 a follow_passages be- és kikapcsolása; igaz, ha időt vitt (egyik sem)

    // a teljes térkép (F5): a MAP jele, a titkos folyosó-mező inverz folyosóként, a titkos ajtó inverz ajtóként, a szörnyek a betűjükkel, az ébren lévők inverzen
    bool is_full_map_shown(void);
    const uint8_t *build_full_map_row(uint8_t y);   // a teljes térkép y. sorának a nézőablakba eső része, a játékos jelével; a következő hívásig érvényes

    // a csak decimális jegyekből álló szöveg értéke a *number-be; hamis, ha üres, más karakter is van benne, vagy max_value-nál nagyobb (ekkor a *number nem változik)
    bool convert_text_to_number(const char *text, uint32_t max_value, uint32_t *number);
#endif

#endif
