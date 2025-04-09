#pragma once
#include "message.h"

typedef struct {
    int head;
    int tail;
    int added_messages;
    int removed_messages;
    int free_space;

    message buffer[QUEUE_SIZE]; 
} message_queue;

void init_queue(message_queue* q);
void enqueue(message_queue* q, const message* msg);
message dequeue(message_queue* q);
