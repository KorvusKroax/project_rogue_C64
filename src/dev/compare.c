// keretprogram a két fordító összevetéséhez: a szintgeneráló fájlokat változtatás nélkül fordítja (compare.bat);
// a játék platformrétege nincs benne, a képernyőt és a CIA-kat közvetlenül írja, a gyári memóriatérképpel és karakterkészlettel

#include <stdint.h>
#include <stdbool.h>

#include "../level.h"
#include "../rng.h"
#include "../state.h"
#include "../view.h"

// a fordítóspecifikus rész csak ide kerülhet
#if defined(__OSCAR64C__)
    #define COMPILER_NAME  "OSCAR64"
#elif defined(__mos__)
    #define COMPILER_NAME  "LLVM-MOS"
#else
    #error "ismeretlen fordító"
#endif

// a szintek ugyanazok, mint a szint-önteszt első COMPARE_LEVEL_COUNT szintje (test_level.c):
// az i. szint magja COMPARE_FIRST_SEED + i, a mélységek sorban körbe mennek
#define COMPARE_LEVEL_COUNT  100
#define COMPARE_FIRST_SEED   1
#define COMPARE_DEPTH_COUNT  4
static const uint8_t compare_depths[COMPARE_DEPTH_COUNT] = { 1, 10, 26, 40 };

// minden mélységre jut legalább egy szint, különben az átlagnál nullával osztanánk
#if COMPARE_LEVEL_COUNT < COMPARE_DEPTH_COUNT
    #error "a COMPARE_LEVEL_COUNT legalább a COMPARE_DEPTH_COUNT legyen"
#endif

// PAL: 312 sor, soronként 63 ciklus
#define PAL_CYCLES_PER_LINE   63
#define PAL_LINES_PER_FRAME   312
#define PAL_CYCLES_PER_FRAME  ((uint32_t)PAL_CYCLES_PER_LINE * PAL_LINES_PER_FRAME)     // 19 656

#define VIDEO_MEMORY  ((volatile uint8_t *)0x0400)     // a gyári képernyőmemória
#define VIDEO_COLUMNS  40
#define VIDEO_ROWS     25

#define VIC_MEMORY_SETUP       (*(volatile uint8_t *)0xd018)
#define VIC_UPPERCASE_CHARSET  0x15             // képernyő: $0400, karakterkészlet: a ROM nagybetűs fele (az llvm-mos indítókódja a kisbetűsre vált)

// a gyári nagybetűs karakterkészlet képernyőkódjai
#define SCREEN_CODE_VERTICAL_LINE  0x5d         // a '|' helyett: a készletben nincs függőleges vonal jel
#define SCREEN_CODE_UNKNOWN        0x3f         // '?': amire nincs képernyőkód

// CIA-regiszterek
#define CIA1_INTERRUPT_CONTROL  (*(volatile uint8_t *)0xdc0d)
#define CIA2_TIMER_A_LOW        (*(volatile uint8_t *)0xdd04)
#define CIA2_TIMER_A_HIGH       (*(volatile uint8_t *)0xdd05)
#define CIA2_TIMER_B_LOW        (*(volatile uint8_t *)0xdd06)
#define CIA2_TIMER_B_HIGH       (*(volatile uint8_t *)0xdd07)
#define CIA2_INTERRUPT_CONTROL  (*(volatile uint8_t *)0xdd0d)
#define CIA2_CONTROL_A          (*(volatile uint8_t *)0xdd0e)
#define CIA2_CONTROL_B          (*(volatile uint8_t *)0xdd0f)

#define CIA_INTERRUPTS_OFF         0x7f         // minden megszakításforrás ki
#define CIA_TIMER_A_INTERRUPT_ON   0x81         // az A időzítő megszakítása be (a KERNAL IRQ-ja)
#define CIA_TIMER_STOP             0x00
#define CIA_TIMER_A_START          0x11         // betöltés a kezdőértékből és indítás; a rendszerórajelet számolja, körbefut
#define CIA_TIMER_B_START          0x51         // betöltés és indítás; az A időzítő átfordulásait számolja

// a szintgenerálás a látott térképet törli és a nézőablakot állítja: a keretprogramban nincs képernyőréteg, ezek üres csonkok
void clear_seen_map(void)
{
}

void center_view_window(uint8_t map_x)
{
    (void)map_x;
}

// a CIA2 két időzítője 32 bites ciklusszámlálóként: az A ciklusonként lefelé számol $FFFF-től,
// a B minden átfordulásánál lép egyet lefelé, szintén $FFFF-től
static void start_cycle_counter(void)
{
    CIA2_CONTROL_A = CIA_TIMER_STOP;
    CIA2_CONTROL_B = CIA_TIMER_STOP;

    CIA2_TIMER_A_LOW = 0xff;
    CIA2_TIMER_A_HIGH = 0xff;
    CIA2_TIMER_B_LOW = 0xff;
    CIA2_TIMER_B_HIGH = 0xff;

    CIA2_CONTROL_B = CIA_TIMER_B_START;
    CIA2_CONTROL_A = CIA_TIMER_A_START;
}

// a start_cycle_counter óta eltelt ciklusok, a két hívás saját idejével együtt
static uint32_t stop_cycle_counter(void)
{
    CIA2_CONTROL_A = CIA_TIMER_STOP;            // az A megáll, így a B sem lép tovább

    uint32_t remaining = ((uint32_t)CIA2_TIMER_B_HIGH << 24) | ((uint32_t)CIA2_TIMER_B_LOW << 16)
                       | ((uint32_t)CIA2_TIMER_A_HIGH << 8) | CIA2_TIMER_A_LOW;
    return 0xffffffffUL - remaining;
}

static uint8_t convert_ascii_to_screen_code(uint8_t character)
{
    if (character >= 0x20 && character <= 0x3f) return character;
    if (character >= 0x40 && character <= 0x5f) return character - 0x40;
    if (character >= 'a' && character <= 'z') return character - 0x60;
    if (character == '|') return SCREEN_CODE_VERTICAL_LINE;
    return SCREEN_CODE_UNKNOWN;
}

static void put_compare_character(uint8_t x, uint8_t y, uint8_t character)
{
    VIDEO_MEMORY[y * VIDEO_COLUMNS + x] = convert_ascii_to_screen_code(character);
}

// a kiírt szöveg utáni első oszlopot adja
static uint8_t put_compare_text(uint8_t x, uint8_t y, const char *text)
{
    while (*text) {
        put_compare_character(x, y, (uint8_t)*text);
        x++;
        text++;
    }
    return x;
}

// a kiírt szám utáni első oszlopot adja
static uint8_t put_compare_number(uint8_t x, uint8_t y, uint32_t number)
{
    char digits[11];
    uint8_t digit_count = 0;

    do {
        digits[digit_count] = (char)('0' + number % 10);
        digit_count++;
        number /= 10;
    } while (number != 0);

    while (digit_count > 0) {
        digit_count--;
        put_compare_character(x, y, (uint8_t)digits[digit_count]);
        x++;
    }
    return x;
}

// a ciklusszám képkockában, két tizedesjegyre kerekítve; a cycles * 100 miatt legfeljebb kb. 42 millió ciklusig (kb. 2000 képkocka) pontos
static void put_compare_frames(uint8_t x, uint8_t y, uint32_t cycles)
{
    uint32_t hundredths = (cycles * 100 + PAL_CYCLES_PER_FRAME / 2) / PAL_CYCLES_PER_FRAME;
    uint8_t fraction = (uint8_t)(hundredths % 100);

    x = put_compare_number(x, y, hundredths / 100);
    put_compare_character(x, y, '.');
    put_compare_character(x + 1, y, (uint8_t)('0' + fraction / 10));
    put_compare_character(x + 2, y, (uint8_t)('0' + fraction % 10));
}

static void clear_compare_screen(void)
{
    VIC_MEMORY_SETUP = VIC_UPPERCASE_CHARSET;

    for (uint16_t i = 0; i < VIDEO_COLUMNS * VIDEO_ROWS; i++) {
        VIDEO_MEMORY[i] = convert_ascii_to_screen_code(' ');
    }
}

// a szám nyolc hexadecimális jeggyel
static void put_compare_hex(uint8_t x, uint8_t y, uint32_t number)
{
    for (int8_t shift = 28; shift >= 0; shift -= 4) {
        uint8_t digit = (uint8_t)((number >> shift) & 0x0f);
        put_compare_character(x, y, digit < 10 ? '0' + digit : 'A' + digit - 10);
        x++;
    }
}

// a teljes szint ujjlenyomata, ugyanúgy, mint a szint-önteszt calculate_level_checksum-ja (test_level.c):
// a MAP két rétege, a szobák és a játékos helye, összegzéssel (Fletcher-féle két összeg)
static uint16_t calculate_level_checksum(void)
{
    uint8_t sum = 0;
    uint8_t sum_of_sums = 0;
    const uint8_t *bytes = &game.map_characters[0][0];

    for (uint16_t i = 0; i < sizeof(game.map_characters); i++) {
        sum += bytes[i];
        sum_of_sums += sum;
    }
    bytes = &game.map_flags[0][0];
    for (uint16_t i = 0; i < sizeof(game.map_flags); i++) {
        sum += bytes[i];
        sum_of_sums += sum;
    }
    bytes = (const uint8_t *)game.rooms;
    for (uint16_t i = 0; i < sizeof(game.rooms); i++) {
        sum += bytes[i];
        sum_of_sums += sum;
    }
    sum += game.player.x;
    sum_of_sums += sum;
    sum += game.player.y;
    sum_of_sums += sum;

    return ((uint16_t)sum_of_sums << 8) | sum;
}

// a táblázat oszlopai
#define DEPTH_COLUMN    0
#define AVERAGE_COLUMN  8
#define MAXIMUM_COLUMN  20
#define RESULT_ROW      3                       // az első mélység sora; a mélységek alatt egy üres sor, majd az összesítés
#define CHECKSUM_ROW    (RESULT_ROW + COMPARE_DEPTH_COUNT + 3)

// mélységenként a generate_level ideje ciklusban: összeg, darab, a leglassabb
static uint32_t depth_cycle_sums[COMPARE_DEPTH_COUNT];
static uint16_t depth_level_counts[COMPARE_DEPTH_COUNT];
static uint32_t depth_cycle_maximums[COMPARE_DEPTH_COUNT];

// a szintek ujjlenyomatai sorban összefűzve (Fletcher-féle két összeg, 16 biten): ha a két fordítónál egyezik, szintenként is egyeznek
static uint16_t level_checksum_sum;
static uint16_t level_checksum_sum_of_sums;

// az átlag és a leglassabb a sor felirata mellé
static void put_compare_result_row(uint8_t y, uint32_t cycle_sum, uint16_t level_count, uint32_t cycle_maximum)
{
    put_compare_frames(AVERAGE_COLUMN, y, cycle_sum / level_count);
    put_compare_frames(MAXIMUM_COLUMN, y, cycle_maximum);
}

static void measure_levels(void)
{
    // a mérés idejére a KERNAL IRQ-ja (CIA1) és a CIA2 NMI-je ki
    CIA1_INTERRUPT_CONTROL = CIA_INTERRUPTS_OFF;
    (void)CIA1_INTERRUPT_CONTROL;               // a függő megszakítás törlése
    CIA2_INTERRUPT_CONTROL = CIA_INTERRUPTS_OFF;

    // a számláló indításának és leállításának saját ideje, ezt minden mérésből levonjuk
    start_cycle_counter();
    uint32_t counter_overhead = stop_cycle_counter();

    for (uint16_t i = 0; i < COMPARE_LEVEL_COUNT; i++) {
        uint8_t depth_index = i % COMPARE_DEPTH_COUNT;

        uint8_t x = put_compare_text(0, 1, "LEVEL ");
        put_compare_number(x, 1, i + 1);

        game.seed = COMPARE_FIRST_SEED + i;
        game.dungeon_level = compare_depths[depth_index];

        start_cycle_counter();
        generate_level();
        uint32_t cycles = stop_cycle_counter() - counter_overhead;

        level_checksum_sum += calculate_level_checksum();
        level_checksum_sum_of_sums += level_checksum_sum;

        depth_cycle_sums[depth_index] += cycles;
        depth_level_counts[depth_index]++;
        if (cycles > depth_cycle_maximums[depth_index]) {
            depth_cycle_maximums[depth_index] = cycles;
        }
    }

    CIA1_INTERRUPT_CONTROL = CIA_TIMER_A_INTERRUPT_ON;
}

static void put_compare_results(void)
{
    put_compare_text(DEPTH_COLUMN, RESULT_ROW - 1, "DEPTH");
    put_compare_text(AVERAGE_COLUMN, RESULT_ROW - 1, "AVG FRAMES");
    put_compare_text(MAXIMUM_COLUMN, RESULT_ROW - 1, "MAX FRAMES");

    uint32_t all_cycle_sum = 0;
    uint16_t all_level_count = 0;
    uint32_t all_cycle_maximum = 0;

    for (uint8_t d = 0; d < COMPARE_DEPTH_COUNT; d++) {
        put_compare_number(DEPTH_COLUMN, RESULT_ROW + d, compare_depths[d]);
        put_compare_result_row(RESULT_ROW + d, depth_cycle_sums[d], depth_level_counts[d], depth_cycle_maximums[d]);

        all_cycle_sum += depth_cycle_sums[d];
        all_level_count += depth_level_counts[d];
        if (depth_cycle_maximums[d] > all_cycle_maximum) {
            all_cycle_maximum = depth_cycle_maximums[d];
        }
    }

    put_compare_text(DEPTH_COLUMN, RESULT_ROW + COMPARE_DEPTH_COUNT + 1, "ALL");
    put_compare_result_row(RESULT_ROW + COMPARE_DEPTH_COUNT + 1, all_cycle_sum, all_level_count, all_cycle_maximum);

    uint8_t x = put_compare_text(DEPTH_COLUMN, CHECKSUM_ROW, "CHECKSUM ");
    put_compare_hex(x, CHECKSUM_ROW, ((uint32_t)level_checksum_sum_of_sums << 16) | level_checksum_sum);
}

int main(void)
{
    rng_init();

    clear_compare_screen();
    put_compare_text(0, 0, COMPILER_NAME);

    measure_levels();
    put_compare_results();

    // megállás: nincs visszatérés a BASIC-be
    while (true) {}
}
