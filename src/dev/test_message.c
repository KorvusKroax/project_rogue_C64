#include "tests.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../platform/screen.h"
#include "../message.h"

// a próbaszöveg 'a' betűs szavakból áll, egy-egy szóközzel; a szavak hossza a képernyő és a sáv méretéből jön
#define WIDTH       SCREEN_WIDTH                        // egy teljes sor
#define LAST_WIDTH  (SCREEN_WIDTH - MORE_TEXT_LENGTH)   // a sáv utolsó sora, a --More-- helye nélkül
#define HALF_LAST   (LAST_WIDTH / 2 + 1)                // két ekkora szó nem fér egy utolsó sorba

#define MAX_WORDS   3

// full_rows: a szavak elé a sáv első MESSAGE_ROW_COUNT - 1 sorát pontosan kitöltő szavak kerülnek
struct LengthCase {
    bool full_rows;
    uint8_t words[MAX_WORDS];       // 0: nincs több szó
    uint8_t expected;               // a full_rows sorai nélkül
};

#define FULL_ROWS_LENGTH  ((MESSAGE_ROW_COUNT - 1) * (WIDTH + 1))

static char test_text[FULL_ROWS_LENGTH + MAX_WORDS * (WIDTH + 2) + 1];

static void add_test_word(uint16_t *length, uint8_t word_length)
{
    if (*length) {
        test_text[(*length)++] = ' ';
    }
    for (uint8_t i = 0; i < word_length; i++) {
        test_text[(*length)++] = 'a';
    }
}

static const char *build_test_text(const struct LengthCase *length_case)
{
    uint16_t length = 0;

    if (length_case->full_rows) {
        for (uint8_t row = 0; row < MESSAGE_ROW_COUNT - 1; row++) {
            add_test_word(&length, WIDTH);
        }
    }
    for (uint8_t w = 0; w < MAX_WORDS && length_case->words[w]; w++) {
        add_test_word(&length, length_case->words[w]);
    }

    test_text[length] = 0;
    return test_text;
}

static const struct LengthCase row_cases[] = {
    { false, { 0 }, 0 },
    { false, { 5, 4 }, 10 },

    // pontosan kitölti a sort
    { false, { WIDTH }, WIDTH },
    { false, { WIDTH, 1 }, WIDTH },

    // az utolsó kiférő szóköznél törik
    { false, { WIDTH - 3, 5 }, WIDTH - 3 },
    { false, { 1, WIDTH }, 1 },

    // a sornál hosszabb szó keményen törik
    { false, { WIDTH + 1 }, WIDTH },
};

static const struct LengthCase bar_cases[] = {
    { false, { 5, 4 }, 10 },

    // az utolsó sorban a --More-- helye is megvan
    { true, { LAST_WIDTH }, LAST_WIDTH },
    { true, { LAST_WIDTH + 1 }, LAST_WIDTH },
    { true, { HALF_LAST, HALF_LAST }, HALF_LAST },

    // ami a sávnál több, a következő oldalra marad
    { true, { WIDTH, WIDTH }, LAST_WIDTH },
};

struct TextCase {
    const char *text, *expected;
};

static const struct TextCase text_cases[] = {
    { "", "" },
    { "you miss the bat", "You miss the bat." },

    // már van a végén írásjel
    { "wait! That's a xeroc!", "Wait! That's a xeroc!" },
    { "really quit?", "Really quit?" },
    { "the end.", "The end." },

    // nem betűvel kezdődik
    { "'x' is not a valid item", "'x' is not a valid item." },

    // tárgylista-sor
    { "a) a mace", "a) a mace" },
};

struct FormatCase {
    const char *text, *first_text, *second_text;
    int16_t number;
    const char *expected;
};

static const struct FormatCase format_cases[] = {
    { "\1 hits you", "the bat", NULL, 0, "The bat hits you." },
    { "you find \3 gold pieces", NULL, NULL, 25, "You find 25 gold pieces." },
    { "\1 \2 \3", "a", "b", -3, "A b -3." },

    // a helyőrzőből lett tárgylista-sor
    { "\1) \2", "c", "a mace", 0, "c) a mace" },
};

// a fordító a literálokat ASCII-ban hagyja, nem alakítja PETSCII-re
static bool are_literals_ascii(void)
{
    static const char sample[] = "aA";
    return sample[0] == 0x61 && sample[1] == 0x41;
}

int test_prepare_message_text(uint16_t i)
{
    if (!are_literals_ascii()) return false;

    for (uint8_t c = 0; c < sizeof(text_cases) / sizeof(text_cases[0]); c++) {
        const struct TextCase *text_case = &text_cases[c];
        if (strcmp(prepare_message_text(text_case->text, NULL, NULL, 0, true), text_case->expected) != 0) return false;
    }

    for (uint8_t c = 0; c < sizeof(format_cases) / sizeof(format_cases[0]); c++) {
        const struct FormatCase *format_case = &format_cases[c];
        const char *prepared = prepare_message_text(format_case->text, format_case->first_text, format_case->second_text, format_case->number, true);
        if (strcmp(prepared, format_case->expected) != 0) return false;
    }

    // a kérdés nem kap záró pontot
    return strcmp(prepare_message_text("file name: ", NULL, NULL, 0, false), "File name: ") == 0;
}

int test_message_chars_fitting_row(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(row_cases) / sizeof(row_cases[0]); c++) {
        const struct LengthCase *row_case = &row_cases[c];
        if (calculate_message_chars_fitting_row(build_test_text(row_case), WIDTH) != row_case->expected) return false;
    }

    return true;
}

int test_message_chars_fitting_bar(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(bar_cases) / sizeof(bar_cases[0]); c++) {
        const struct LengthCase *bar_case = &bar_cases[c];
        uint16_t expected = bar_case->expected + (bar_case->full_rows ? FULL_ROWS_LENGTH : 0);
        if (calculate_message_chars_fitting_bar(build_test_text(bar_case)) != expected) return false;
    }

    return true;
}
