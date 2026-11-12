// keretprogram a két fordító összevetéséhez: a szintgeneráló fájlokat változtatás nélkül fordítja (compare.bat);
// a játék platformrétege nincs benne, a képernyőt közvetlenül írja, a gyári memóriatérképpel és karakterkészlettel

#include <stdint.h>
#include <stdbool.h>

#include "../level.h"
#include "../map.h"
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

#define COMPARE_SEED   1                        // a generált szint magja
#define COMPARE_DEPTH  1                        // és mélysége

#define VIDEO_MEMORY  ((volatile uint8_t *)0x0400)     // a gyári képernyőmemória
#define VIDEO_COLUMNS  40
#define VIDEO_ROWS     25

#define VIC_MEMORY_SETUP       (*(volatile uint8_t *)0xd018)
#define VIC_UPPERCASE_CHARSET  0x15             // képernyő: $0400, karakterkészlet: a ROM nagybetűs fele (az llvm-mos indítókódja a kisbetűsre vált)

// a gyári nagybetűs karakterkészlet képernyőkódjai
#define SCREEN_CODE_VERTICAL_LINE  0x5d         // a '|' helyett: a készletben nincs függőleges vonal jel
#define SCREEN_CODE_UNKNOWN        0x3f         // '?': amire nincs képernyőkód

// a szintgenerálás a látott térképet törli és a nézőablakot állítja: a keretprogramban nincs képernyőréteg, ezek üres csonkok
void clear_seen_map(void)
{
}

void center_view_window(uint8_t map_x)
{
    (void)map_x;
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

static void clear_compare_screen(void)
{
    VIC_MEMORY_SETUP = VIC_UPPERCASE_CHARSET;

    for (uint16_t i = 0; i < VIDEO_COLUMNS * VIDEO_ROWS; i++) {
        VIDEO_MEMORY[i] = convert_ascii_to_screen_code(' ');
    }
}

// a térkép bal fele (az első VIDEO_COLUMNS oszlop) a térkép saját soraiba, a játékossal
static void draw_map_left_half(void)
{
    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < VIDEO_COLUMNS; x++) {
            put_compare_character(x, y, get_map_character(x, y));
        }
    }
    if (game.player.x < VIDEO_COLUMNS) {
        put_compare_character(game.player.x, game.player.y, CHAR_PLAYER);
    }
}

int main(void)
{
    rng_init();

    game.seed = COMPARE_SEED;
    game.dungeon_level = COMPARE_DEPTH;
    generate_level();

    clear_compare_screen();

    uint8_t x = put_compare_text(0, 0, COMPILER_NAME " SEED ");
    x = put_compare_number(x, 0, COMPARE_SEED);
    x = put_compare_text(x, 0, " DEPTH ");
    put_compare_number(x, 0, COMPARE_DEPTH);

    draw_map_left_half();

    // megállás: nincs visszatérés a BASIC-be
    while (true) {}
}
