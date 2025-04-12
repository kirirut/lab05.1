#include "message.h"

void generate_message(message* msg, unsigned int* seed) {
    msg->type = 1;
    msg->size = rand_r(seed) % 256;  // значение в диапазоне [0, 255]
    
    int real_size = msg->size + 1;  // фактический размер сообщения
    int total_len = ((real_size + 3) / 4) * 4;  // выравнивание по 4 байта

    for (int i = 0; i < msg->size; ++i) {
        msg->data[i] = rand_r(seed) % 256;
    }

    msg->data[msg->size] = 0;  // дополнительный байт, можно обнулить

    for (int i = msg->size + 1; i < total_len; ++i) {
        msg->data[i] = 0;  // заполняем нулями до кратности 4
    }

    msg->hash = 0;

    uint16_t checksum = 0;
    checksum += msg->type;
    checksum += msg->size;
    for (int i = 0; i < real_size; ++i) {
        checksum += msg->data[i];  // считаем сумму по size + 1 байту
    }

    msg->hash = checksum;
}
