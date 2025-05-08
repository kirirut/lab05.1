#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_queue(Queue* queue, int size) {
    queue->messages = malloc(size * sizeof(Message));
    queue->capacity = size;
    queue->head = 0;
    queue->tail = 0;
    queue->added_count = 0;
    queue->removed_count = 0;
    queue->free_space = size;
    pthread_mutex_init(&queue->mutex, NULL);
    sem_init(&queue->empty, 0, size);
    sem_init(&queue->full, 0, 0);
}

void resize_queue(Queue* queue, int new_size) {
    pthread_mutex_lock(&queue->mutex);

    int occupied = queue->capacity - queue->free_space;
    if (new_size < occupied) {
        printf("Cannot shrink queue below occupied slots (%d)\n", occupied);
        pthread_mutex_unlock(&queue->mutex);
        return;
    }

    Message* new_messages = realloc(queue->messages, new_size * sizeof(Message));
    if (!new_messages) {
        perror("realloc failed");
        pthread_mutex_unlock(&queue->mutex);
        return;
    }
    queue->messages = new_messages;

    if (new_size < queue->capacity) {
        if (queue->head > queue->tail) {
            int shift = queue->capacity - queue->head;
            memmove(&queue->messages[new_size - shift], &queue->messages[queue->head], shift * sizeof(Message));
            queue->head = new_size - shift;
        }
        queue->tail = queue->tail % new_size;
    }
    int old_capacity = queue->capacity;
    queue->capacity = new_size;
    queue->free_space = new_size - occupied;

    pthread_mutex_unlock(&queue->mutex);

    sem_init(&queue->empty, 0, queue->free_space);
    sem_init(&queue->full, 0, occupied);
    printf("Queue resized from %d to %d\n", old_capacity, new_size);
}