#ifndef DEV_H
#define DEV_H

#include <stdint.h>
#include <stdbool.h>

#ifdef DEVMODE
    void dev_fail(const char *message, const char *file, int line); // hamis feltételnél: üzenet + forrásbeli hely, majd a program megáll
    #define DEV_CHECK(cond, message) do { if (!(cond)) dev_fail(message, __FILE__, __LINE__); } while (0)

    bool dev_run_tests(void);           // öntesztek: mindegyik lefut, az eredmény a képernyőre kerül, igazat ad, ha mind sikerült
    void put_failed_test_names(const char *prefix);   // az utolsó tesztfutás hibás tesztjei soronként, a prefix után
    bool dev_run_level_tests(void);     // szint-önteszt: sok véletlen szint rögzített magokkal; hosszú, VICE-ben warp módban
    void dev_run_screens(void);         // seed, gyors öntesztek és alattuk menü: a szint-önteszt és a tesztképernyők billentyűre, ESC-re vissza; RETURN-re visszatér
    bool dev_execute_command(uint8_t code); // fejlesztői parancs játék közben, F-billentyűre: F1 új szint, F3 ugrás mélységre, F5 teljes térkép, F7 a jump_to_run_end, F8 a follow_passages be- és kikapcsolása; igaz, ha időt vitt (egyik sem)

    // a teljes térkép (F5): a MAP jele, a titkos folyosó-mező inverz folyosóként, a titkos ajtó inverz ajtóként
    uint8_t calculate_full_map_character(uint8_t x, uint8_t y);
    bool is_full_map_shown(void);
    const uint8_t *build_full_map_row(uint8_t y);   // a teljes térkép y. sorának a nézőablakba eső része, a játékos jelével; a következő hívásig érvényes

    // a csak decimális jegyekből álló szöveg értéke a *number-be; hamis, ha üres, más karakter is van benne, vagy max_value-nál nagyobb (ekkor a *number nem változik)
    bool convert_text_to_number(const char *text, uint32_t max_value, uint32_t *number);
#endif

#endif
