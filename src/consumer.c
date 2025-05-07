#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include "consumer.h"

uint16_t recalculate_hash(const message* msg) {
    uint16_t checksum = 0;
    checksum += msg->type;
    checksum += msg->size;
    for (int i = 0; i < msg->size; ++i) {
        checksum += msg->data[i];
    }
    return checksum;
}

void consumer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex) {
    while (q->free_space != q->queue_size) {
        sem_P(sem_fill);  
        sem_P(sem_mutex);  
        if (q->free_space == q->queue_size) {
            sem_V(sem_mutex);
            break;  
        }
        message msg = dequeue(q);
        int count = q->removed_messages;  
        if (q->shrink_requested && q->free_space >= (q->queue_size - q->new_size)) {
            resize_queue(q, q->new_size);
            q->shrink_requested = 0;
        }
        sem_V(sem_mutex); 
        uint16_t actual_hash = recalculate_hash(&msg);
        int is_valid = (actual_hash == msg.hash);
        printf("Consumed message %d: type=%d, size=%d, hash=%u — %s\n",
               count, msg.type, msg.size, msg.hash,
               is_valid ? "OK" : "CORRUPTED");
        fflush(stdout);
        sem_V(sem_empty); 
        sleep(3);  
    }
}
