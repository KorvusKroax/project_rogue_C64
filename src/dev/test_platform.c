#include "tests.h"

#include <stdbool.h>
#include <string.h>
#include <c64/cia.h>
#include <c64/memmap.h>

#include "../platform/platform.h"
#include "../texts.h"

#define COPY_TEST_BUFFER_SIZE               16
#define PROCESSOR_STATUS_INTERRUPT_DISABLE  0x04    // az állapotregiszter I bitje

#define MEMORY_CONFIGURATION  (*(volatile __memmap uint8_t *)0x01)
#define KERNAL_NMI_VECTOR     (*(void **)0x0318)
#define RAM_NMI_VECTOR        (*(void * volatile *)0xfffa)

#define CIA_INTERRUPT_TIMER_A     0x01
#define CIA_INTERRUPT_ENABLE      0x80  // az icr írásakor a megadott forrásokat engedélyezi, enélkül tiltja
#define CIA_INTERRUPT_ALL         0x7f
#define CIA_TIMER_START_ONE_SHOT  0x19  // indítás, egyszeri lefutás, a kezdőérték betöltése

#define NMI_TEST_TEXT_LENGTH   120      // a másolása kb. 2500 ciklus (karakterenként kb. 21)
#define NMI_TEST_TIMER_CYCLES  500      // ennyi ciklus után kér NMI-t a CIA2 időzítője: a másolás közben

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
    uint8_t memory_configuration = MEMORY_CONFIGURATION;

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
        get_text_from_table(table, 1) == text &&
        // a játék szövegei és táblái is
        is_copied(buffer, copy_text(buffer, text_you, sizeof(buffer) - 1), "you") &&
        is_copied(buffer, copy_text_from_table(buffer, text_monster_names, 'Z' - 'A', sizeof(buffer) - 1), "zombie") &&
        is_copied(buffer, copy_text(buffer, get_text_from_table(text_monster_names, 'Y' - 'A'), sizeof(buffer) - 1), "yeti") &&
        // a $01 visszaáll
        MEMORY_CONFIGURATION == memory_configuration;
}

// a KERNAL ROM alatti RAM-ból
static void *read_ram_nmi_vector(void)
{
    __asm {
        php
        sei
    }
    uint8_t memory_configuration = MEMORY_CONFIGURATION;
    MEMORY_CONFIGURATION = MMAP_RAM;

    void *vector = RAM_NMI_VECTOR;

    MEMORY_CONFIGURATION = memory_configuration;
    __asm {
        plp
    }
    return vector;
}

// rossz vektornál a gép elszáll, és a tesztprogram nem ér véget
int test_nmi_during_copy(uint16_t i)
{
    static char text[NMI_TEST_TEXT_LENGTH + 1];
    static char buffer[NMI_TEST_TEXT_LENGTH + 1];

    if (read_ram_nmi_vector() != KERNAL_NMI_VECTOR) {
        return false;
    }

    memset(text, 'a', NMI_TEST_TEXT_LENGTH);
    text[NMI_TEST_TEXT_LENGTH] = 0;
    uint8_t timer_control = cia2.cra;

    __asm { sei }                       // a raszter-IRQ ne késleltesse a másolás kezdetét
    cia2.icr = CIA_INTERRUPT_ALL;
    (void)(cia2.icr);                   // a függő kérés törlése
    cia2.ta = NMI_TEST_TIMER_CYCLES;
    cia2.icr = CIA_INTERRUPT_ENABLE | CIA_INTERRUPT_TIMER_A;
    cia2.cra = CIA_TIMER_START_ONE_SHOT;
    copy_text(buffer, text, NMI_TEST_TEXT_LENGTH);
    uint8_t interrupts = cia2.icr;      // az olvasás törli a kérést, és az NMI-vonal felenged
    cia2.icr = CIA_INTERRUPT_ALL;
    cia2.cra = timer_control;
    __asm { cli }

    return (interrupts & CIA_INTERRUPT_TIMER_A) && strcmp(buffer, text) == 0;
}
