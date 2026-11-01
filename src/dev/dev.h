#ifndef DEV_H
#define DEV_H

#include <stdint.h>
#include <stdbool.h>

#ifdef DEVMODE
    void dev_fail(const char *msg, const char *file, int line); // hamis feltételnél: üzenet + forrásbeli hely, majd vissza a BASIC-be
    #define DEV_CHECK(cond, msg) do { if (!(cond)) dev_fail(msg, __FILE__, __LINE__); } while (0)

    bool dev_run_tests(void);           // öntesztek: mindegyik lefut, az eredmény a képernyőre kerül, igazat ad, ha mind sikerült
    void dev_charset_screen(void);      // minden jel normálisan és inverzen, térképminta, a képernyő széleinek próbája
    void dev_keyboard_screen(void);     // a billentyűmátrix és a módosítók élőben; RETURN-re kilép
    void dev_view_screen(void);         // 80 oszlopos minta a nézőablakban, három nézettel; RETURN-re kilép
#else
    #define DEV_CHECK(cond, msg) ((void)0)
#endif

#endif
