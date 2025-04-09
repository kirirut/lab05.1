#pragma once
#include <stdint.h>
#include <string.h>

#define MAX_DATA_SIZE 256
#define QUEUE_SIZE 10

typedef struct {
    uint8_t type;           // 1 байт
    uint16_t hash;          // 2 байта
    uint8_t size;           // 1 байт
    uint8_t data[((MAX_DATA_SIZE + 3) / 4) * 4];  // кратно 4
} message;
