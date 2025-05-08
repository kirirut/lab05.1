#define _POSIX_C_SOURCE 200809L
#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_DATA_SIZE 256
#define MAX_THREADS 100

typedef struct {
    uint8_t type;
    uint16_t hash;
    uint8_t size;
    uint8_t data[MAX_DATA_SIZE];
} Message;

typedef struct {
    Message* messages;
    int capacity;
    int head;
    int tail;
    int added_count;
    int removed_count;
    int free_space;
    pthread_mutex_t mutex;
    sem_t empty;
    sem_t full;
} Queue;

uint16_t compute_hash(const Message* msg);
void init_queue(Queue* queue, int size);
void resize_queue(Queue* queue, int new_size);
void* producer(void* arg);
void* consumer(void* arg);

#endif