#include "message.h"

void generate_message(message* msg, unsigned int* seed) {
    msg->type = 1;
    msg->size = rand_r(seed) % 256;

    for (int i = 0; i < msg->size; ++i) {
        msg->data[i] = rand_r(seed) % 256;
    }

    int total_len = ((msg->size + 3) / 4) * 4;
    for (int i = msg->size; i < total_len; ++i) {
        msg->data[i] = 0;
    }

    msg->hash = 0;
    uint16_t checksum = 0;
    checksum += msg->type;
    checksum += msg->size;
    for (int i = 0; i < msg->size; ++i) {
        checksum += msg->data[i];
    }

    msg->hash = checksum;
}