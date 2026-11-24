#include "disk.h"

#include <c64/kernalio.h>

#define CURRENT_DEVICE     (*(volatile uint8_t *)0x00ba)    // a KERNAL legutóbb használt egysége: induláskor az, amelyről a játék betöltődött
#define FIRST_DISK_DEVICE  8
#define READ_FILE_NUMBER   2                                // a KERNAL fájlszáma
#define READ_CHANNEL       0                                // a 0. csatornán a PRG fájl a betöltési címével együtt olvasható

uint8_t get_game_device(void)
{
    uint8_t device = CURRENT_DEVICE;
    return device < FIRST_DISK_DEVICE ? FIRST_DISK_DEVICE : device;
}

bool open_file_for_reading(const char *name, uint8_t device)
{
    krnio_setnam(name);
    return krnio_open(READ_FILE_NUMBER, device, READ_CHANNEL);
}

uint16_t read_file(char *buffer, uint16_t size)
{
    int count = krnio_read(READ_FILE_NUMBER, buffer, size);
    return count < 0 ? 0 : count;
}

bool is_file_end_reached(void)
{
    return krnio_pstatus[READ_FILE_NUMBER] == KRNIO_EOF;
}

void close_file(void)
{
    krnio_close(READ_FILE_NUMBER);
}
