#include "rng.h"

#include "state.h"

// RN: seed = seed * RN_MULTIPLIER + RN_INCREMENT (mod 2^32), a véletlenszám a seed felső 16 bitje
#define RN_MULTIPLIER  11109                    // default: 11109 (0x2B65)
#define RN_INCREMENT   13849                    // default: 13849 (0x3619)

// a szorzás bájtonkénti táblákkal megy, ezért mindkét konstans legfeljebb 16 bites
#if RN_MULTIPLIER > 0xFFFF || RN_INCREMENT > 0xFFFF
    #error "az RN_MULTIPLIER és az RN_INCREMENT legfeljebb 16 bites lehet"
#endif

#define RN_MULTIPLIER_LOW_BYTE   (RN_MULTIPLIER & 0xFF)     // default: 0x65
#define RN_MULTIPLIER_HIGH_BYTE  (RN_MULTIPLIER >> 8)       // default: 0x2B
#define RN_INCREMENT_LOW_BYTE    (RN_INCREMENT & 0xFF)      // default: 0x19
#define RN_INCREMENT_HIGH_BYTE   (RN_INCREMENT >> 8)        // default: 0x36

// i * RN_MULTIPLIER_LOW_BYTE és i * RN_MULTIPLIER_HIGH_BYTE alsó és felső bájtja, minden i bájtra (4 × 256 bájt)
static uint8_t low_byte_product_low[256];
static uint8_t low_byte_product_high[256];
static uint8_t high_byte_product_low[256];
static uint8_t high_byte_product_high[256];

void rng_init(void)
{
    for (uint16_t i = 0; i < 256; i++) {
        uint16_t low_byte_product = i * RN_MULTIPLIER_LOW_BYTE;
        uint16_t high_byte_product = i * RN_MULTIPLIER_HIGH_BYTE;

        low_byte_product_low[i] = (uint8_t)low_byte_product;
        low_byte_product_high[i] = (uint8_t)(low_byte_product >> 8);
        high_byte_product_low[i] = (uint8_t)high_byte_product;
        high_byte_product_high[i] = (uint8_t)(high_byte_product >> 8);
    }
}

// a szorzat bájtoszloponként, mint az írásbeli szorzásnál: a seed i. bájtja a szorzó alsó bájtjával az i. oszlopba,
// a felső bájtjával az i + 1. oszlopba kerül; a 3. oszlop fölötti rész elveszik (mod 2^32)
uint16_t RN(void)
{
    uint8_t *seed = (uint8_t *)&game.seed;     // a legalsó bájt elöl
    uint8_t seed0 = seed[0];
    uint8_t seed1 = seed[1];
    uint8_t seed2 = seed[2];
    uint8_t seed3 = seed[3];

    // oszloponként 16 bites összeg: a felső bájtja az átvitel a következő oszlopba
    uint16_t column0 = low_byte_product_low[seed0] + RN_INCREMENT_LOW_BYTE;
    uint16_t column1 = (column0 >> 8) + RN_INCREMENT_HIGH_BYTE
                     + low_byte_product_high[seed0] + low_byte_product_low[seed1]
                     + high_byte_product_low[seed0];
    uint16_t column2 = (column1 >> 8)
                     + low_byte_product_high[seed1] + low_byte_product_low[seed2]
                     + high_byte_product_high[seed0] + high_byte_product_low[seed1];
    uint8_t column3 = (uint8_t)(column2 >> 8)
                    + low_byte_product_high[seed2] + low_byte_product_low[seed3]
                    + high_byte_product_high[seed1] + high_byte_product_low[seed2];

    seed[0] = (uint8_t)column0;
    seed[1] = (uint8_t)column1;
    seed[2] = (uint8_t)column2;
    seed[3] = column3;

    return ((uint16_t)column3 << 8) | (uint8_t)column2;
}

uint16_t rnd(uint16_t n)
{
    return n == 0 ? 0 : RN() % n;
}

uint16_t ROLL(uint16_t count, uint16_t sides)
{
    uint16_t sum = 0;
    for (uint16_t i = 0; i < count; i++) {
        sum += rnd(sides) + 1;
    }

    return sum;
}

uint16_t spread(uint16_t n)
{
    return n - n / 20 + rnd(n / 10);
}
