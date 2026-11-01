#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>

bool disk_exists(const char *name);
bool disk_save(const char *name, const void *data, uint16_t size);
bool disk_load(const char *name, void *data, uint16_t size);

#endif
