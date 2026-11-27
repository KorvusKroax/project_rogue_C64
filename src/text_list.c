#include "text_list.h"

#include <stddef.h>

#include "platform/key.h"
#include "platform/screen.h"
#include "message.h"
#include "state.h"
#include "texts.h"
#include "view.h"

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

// clear módban egy oldal ennyi sor; a képernyő utolsó sora a TEXT_PRESS_SPACE-é
#define CLEAR_PAGE_ROW_COUNT  (SCREEN_HEIGHT - 1)

// overwrite módban a sorok előtt egy üres oszlop
#define WINDOW_MARGIN  1

// a folytatósorban a behúzás után is marad hely
#if TEXT_LIST_CONTINUATION_INDENT >= SCREEN_WIDTH
    #error "a folytatósor behúzása nem fér el a képernyő szélességében"
#endif

// a törés szóköze nem kerül a következő sor elejére
static const char *skip_break_space(const char *text)
{
    return *text == ' ' ? text + 1 : text;
}

// a tétel text-tel kezdődő sorának hossza, a folytatósor behúzása nélkül; a következő sor a skip_break_space(text + hossz)-nál kezdődik
static uint8_t calculate_entry_row_length(const char *text, bool is_continuation)
{
    uint8_t width = is_continuation ? SCREEN_WIDTH - TEXT_LIST_CONTINUATION_INDENT : SCREEN_WIDTH;
    return calculate_message_chars_fitting_row(text, width);
}

static void add_row_to_size(struct TextListSize *size, uint8_t length)
{
    if (size->row_count < SCREEN_HEIGHT) {
        size->row_count++;
    }
    if (length > size->width) {
        size->width = length;
    }
}

void calculate_text_list_size(TextListEntryFunction get_entry, struct TextListSize *size)
{
    size->entry_count = 0;
    size->row_count = 0;
    size->width = TEXT_PRESS_SPACE_LENGTH;

    const char *text = get_entry(0);
    while (text != NULL) {
        bool is_continuation = false;
        do {
            uint8_t length = calculate_entry_row_length(text, is_continuation);
            add_row_to_size(size, is_continuation ? TEXT_LIST_CONTINUATION_INDENT + length : length);
            text = skip_break_space(text + length);
            is_continuation = true;
        } while (*text);

        size->entry_count++;
        text = get_entry(size->entry_count);
    }
}

bool does_text_list_fit_window(const struct TextListSize *size)
{
    return size->width + WINDOW_MARGIN <= SCREEN_WIDTH && size->row_count + 1 <= SCREEN_HEIGHT;
}

// a y. sor a left. oszloptól a sor végéig üres, és a szöveg a text_column. oszloptól
static void draw_list_row(uint8_t left, uint8_t text_column, uint8_t y, const char *text, uint8_t length)
{
    set_screen_cursor(left, y);
    clear_to_row_end();
    for (uint8_t i = 0; i < length; i++) {
        put_screen_character(text_column + i, y, text[i]);
    }
}

static void show_press_space(uint8_t left, uint8_t margin, uint8_t y)
{
    draw_list_row(left, left + margin, y, TEXT_PRESS_SPACE, TEXT_PRESS_SPACE_LENGTH);
    while (wait_for_key() != ' ') {}
}

// a tételek sorai a 0. sortól, a left. oszloptól a sor végéig, a szöveg margin oszloppal beljebb;
// ha a sorok elérik a képernyő utolsó sorát, ott TEXT_PRESS_SPACE, szóközre a képernyő törlődik, és a lista a 0. sortól folytatódik
static void draw_list_entries(TextListEntryFunction get_entry, uint8_t entry_count, uint8_t left, uint8_t margin)
{
    uint8_t y = 0;
    for (uint8_t index = 0; index < entry_count; index++) {
        const char *text = get_entry(index);
        bool is_continuation = false;
        do {
            if (y == CLEAR_PAGE_ROW_COUNT) {
                show_press_space(left, margin, y);
                screen_clear();
                y = 0;
            }
            // az üres tétel (elválasztó) nem kerül az oldal elejére; az első tétel nem üres
            if (y == 0 && !*text) break;

            uint8_t length = calculate_entry_row_length(text, is_continuation);
            uint8_t text_column = left + margin;
            if (is_continuation) {
                text_column += TEXT_LIST_CONTINUATION_INDENT;
            }
            draw_list_row(left, text_column, y, text, length);
            text = skip_break_space(text + length);
            y++;
            is_continuation = true;
        } while (*text);
    }
}

// tételenként egy üzenet: az első lecseréli a sáv szövegét, a többi előtt --More--, mert a listasorok nem fűződnek össze (message.h);
// ennél a --More---nál ESC-re a lista véget ér, és a sáv üres; az üres tétel kimarad
static void show_list_in_message_bar(TextListEntryFunction get_entry, uint8_t entry_count)
{
    message_clear();
    for (uint8_t index = 0; index < entry_count; index++) {
        const char *text = get_entry(index);
        if (*text && !message_list_row(text, true)) return;
    }
}

void show_text_list(TextListEntryFunction get_entry)
{
    struct TextListSize size;
    calculate_text_list_size(get_entry, &size);

    if (size.entry_count == 1) {
        message_list_row(get_entry(0), false);
        return;
    }
    if (options.inventory_style == INVENTORY_SLOW) {
        show_list_in_message_bar(get_entry, size.entry_count);
        return;
    }

    open_temporary_window();
    if (options.inventory_style == INVENTORY_OVERWRITE && does_text_list_fit_window(&size)) {
        // a jobb felső sarokban: a leghosszabb sor a képernyő jobb széléig ér
        uint8_t left = SCREEN_WIDTH - WINDOW_MARGIN - size.width;
        draw_list_entries(get_entry, size.entry_count, left, WINDOW_MARGIN);
        show_press_space(left, WINDOW_MARGIN, size.row_count);
    } else {
        screen_clear();
        draw_list_entries(get_entry, size.entry_count, 0, 0);
        show_press_space(0, 0, CLEAR_PAGE_ROW_COUNT);
    }
    close_temporary_window();
}

#pragma optimize(pop)
