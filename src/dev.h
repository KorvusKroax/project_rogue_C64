#ifndef DEV_H
#define DEV_H

#include <stdint.h>

#ifdef DEVMODE

// hamis feltételnél: üzenet + forrásbeli hely, majd vissza a BASIC-be
void dev_fail(const char *msg, const char *file, int line);

#define DEV_CHECK(cond, msg) \
    do { if (!(cond)) dev_fail(msg, __FILE__, __LINE__); } while (0)

#else

#define DEV_CHECK(cond, msg) ((void)0)

#endif

#endif
