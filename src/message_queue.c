#include "message_queue.h"


void init_queue(message_queue* q) {
    q->head = 0;
    q->tail = 0;
    q->added_messages = 0;
    q->removed_messages = 0;
    q->free_space = QUEUE_SIZE;
}

void enqueue(message_queue* q, const message* msg) {
    if (q->free_space <= 0) return;

    memcpy(&q->buffer[q->tail], msg, sizeof(message));
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->added_messages++;
    q->free_space--;
    }


message dequeue(message_queue* q) {
    message msg = {0};

    if (q->free_space >= QUEUE_SIZE) return msg; // пусто

    memcpy(&msg, &q->buffer[q->head], sizeof(message));
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->removed_messages++;
    q->free_space++;

    return msg;
}
