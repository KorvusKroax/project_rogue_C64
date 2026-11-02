#ifndef DEV_H
#define DEV_H

#include <stdint.h>
#include <stdbool.h>

#ifdef DEVMODE
    void dev_fail(const char *message, const char *file, int line); // hamis feltételnél: üzenet + forrásbeli hely, majd vissza a BASIC-be
    #define DEV_CHECK(cond, message) do { if (!(cond)) dev_fail(message, __FILE__, __LINE__); } while (0)

    bool dev_run_tests(void);           // öntesztek: mindegyik lefut, az eredmény a képernyőre kerül, igazat ad, ha mind sikerült
    void dev_run_screens(void);         // seed, öntesztek és a tesztképernyők egymás után; a végén visszatér
#else
    #define DEV_CHECK(cond, message) ((void)0)
#endif

#endif
