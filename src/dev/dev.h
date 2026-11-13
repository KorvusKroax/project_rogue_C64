#ifndef DEV_H
#define DEV_H

#include <stdint.h>
#include <stdbool.h>

#ifdef DEVMODE
    void dev_fail(const char *message, const char *file, int line); // hamis feltételnél: üzenet + forrásbeli hely, majd a program megáll
    #define DEV_CHECK(cond, message) do { if (!(cond)) dev_fail(message, __FILE__, __LINE__); } while (0)

    bool dev_run_tests(void);           // öntesztek: mindegyik lefut, az eredmény a képernyőre kerül, igazat ad, ha mind sikerült
    bool dev_run_level_tests(void);     // szint-önteszt: sok véletlen szint rögzített magokkal; hosszú, VICE-ben warp módban
    void dev_run_screens(void);         // seed, gyors öntesztek és alattuk menü: a szint-önteszt és a tesztképernyők billentyűre, ESC-re vissza; RETURN-re visszatér
    bool dev_execute_command(uint8_t code); // fejlesztői parancs játék közben, F-billentyűre: F1 új szint, F3 ugrás mélységre; igaz, ha időt vitt (egyik sem)

    // a csak decimális jegyekből álló szöveg értéke a *number-be; hamis, ha üres, más karakter is van benne, vagy max_value-nál nagyobb (ekkor a *number nem változik)
    bool convert_text_to_number(const char *text, uint32_t max_value, uint32_t *number);
#endif

#endif
