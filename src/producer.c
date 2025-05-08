#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void* producer(void* arg) {
    Queue* queue = (Queue*)arg;
    unsigned int seed = time(NULL) ^ pthread_self();

    while (1) {
        Message msg;
        msg.type = rand_r(&seed) % 256;
        msg.size = rand_r(&seed) % 256;
        for (int i = 0; i < msg.size; i++) {
            msg.data[i] = rand_r(&seed) % 256;
        }
        msg.hash = compute_hash(&msg);

        sem_wait(&queue->empty);
        pthread_mutex_lock(&queue->mutex);

        memcpy(&queue->messages[queue->tail], &msg, sizeof(Message));
        queue->tail = (queue->tail + 1) % queue->capacity;
        queue->added_count++;
        queue->free_space--;
        int added = queue->added_count;

        pthread_mutex_unlock(&queue->mutex);
        sem_post(&queue->full);

        printf("Produced message %d: type=%u, size=%u, hash=%u\n", 
               added, msg.type, msg.size, msg.hash);
        struct timespec ts = {2, 0}; // 2 seconds delay
        nanosleep(&ts, NULL);
    }
    return NULL;
}