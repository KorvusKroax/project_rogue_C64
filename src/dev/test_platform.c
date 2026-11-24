#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../platform/platform.h"
#include "../texts.h"

#define COPY_TEST_BUFFER_SIZE               16
#define PROCESSOR_STATUS_INTERRUPT_DISABLE  0x04    // az állapotregiszter I bitje

static volatile uint8_t processor_status;

static void read_processor_status(void)
{
    __asm {
        php
        pla
        sta processor_status
    }
}

// a buffer a várt szöveg, és az end a lezáró nullájára mutat
static bool is_copied(const char *buffer, const char *end, const char *expected)
{
    return strcmp(buffer, expected) == 0 && end == buffer + strlen(expected);
}

int test_copy_text(uint16_t i)
{
    static const char text[] = "hello";
    static const char *const table[] = { "alpha", text };
    char buffer[COPY_TEST_BUFFER_SIZE];
    uint8_t memory_configuration = *(volatile uint8_t *)0x01;

    // a megszakítás tiltása a hívás után ugyanaz, mint előtte
    __asm { sei }
    copy_text(buffer, text, sizeof(buffer) - 1);
    read_processor_status();
    bool is_disabled_kept = (processor_status & PROCESSOR_STATUS_INTERRUPT_DISABLE) != 0;
    __asm { cli }
    copy_text(buffer, text, sizeof(buffer) - 1);
    read_processor_status();
    bool is_enabled_kept = (processor_status & PROCESSOR_STATUS_INTERRUPT_DISABLE) == 0;

    // a max_length után a lezáró nulla, mögötte a buffer érintetlen
    memset(buffer, 'x', sizeof(buffer));
    bool is_shortened = is_copied(buffer, copy_text(buffer, text, 3), "hel") && buffer[4] == 'x';

    return
        is_disabled_kept &&
        is_enabled_kept &&
        is_shortened &&
        is_copied(buffer, copy_text(buffer, text, sizeof(buffer) - 1), "hello") &&
        is_copied(buffer, copy_text(buffer, text, 5), "hello") &&
        is_copied(buffer, copy_text(buffer, text, 0), "") &&
        is_copied(buffer, copy_text(buffer, "", sizeof(buffer) - 1), "") &&
        is_copied(buffer, copy_text_from_table(buffer, table, 0, sizeof(buffer) - 1), "alpha") &&
        is_copied(buffer, copy_text_from_table(buffer, table, 1, 2), "he") &&
        // a játék szövegei és táblái is
        is_copied(buffer, copy_text(buffer, text_you, sizeof(buffer) - 1), "you") &&
        is_copied(buffer, copy_text_from_table(buffer, text_monster_names, 'Z' - 'A', sizeof(buffer) - 1), "zombie") &&
        // a $01 visszaáll
        *(volatile uint8_t *)0x01 == memory_configuration;
}
