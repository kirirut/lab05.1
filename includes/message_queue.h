#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct {
    int head;
    int tail;
    int added_messages;
    int removed_messages;
    int free_space;
    int queue_size;
    message *buffer;

    int shrink_requested;
    int new_size;
} message_queue;


void init_queue(message_queue* q, int queue_size);
void enqueue(message_queue* q, const message* msg);
message dequeue(message_queue* q);
void print_queue_state(message_queue* q);
void destroy_queue(message_queue* q);
void request_shrink_queue(message_queue *q, int new_size);
void resize_queue(message_queue *q, int new_size);


#endif 