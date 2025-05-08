#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* consumer(void* arg) {
    Queue* queue = (Queue*)arg;

    while (1) {
        sem_wait(&queue->full);
        pthread_mutex_lock(&queue->mutex);

        Message msg;
        memcpy(&msg, &queue->messages[queue->head], sizeof(Message));
        queue->head = (queue->head + 1) % queue->capacity;
        queue->removed_count++;
        queue->free_space++;
        int removed = queue->removed_count;

        pthread_mutex_unlock(&queue->mutex);
        sem_post(&queue->empty);

        uint16_t computed_hash = compute_hash(&msg);
        printf("Consumed message %d: type=%u, size=%u, hash=%u, check=%s\n",
               removed, msg.type, msg.size, msg.hash,
               (computed_hash == msg.hash) ? "OK" : "ERROR");

        struct timespec ts = {2, 0}; // 2 seconds delay
        nanosleep(&ts, NULL);
    }
    return NULL;
}