#include "producer.h"
#include "globals.h"
void producer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex) {
    unsigned int seed = time(NULL) ^ getpid();
    while(run) {
        sem_P(sem_empty);  
        sem_P(sem_mutex);  

        
        if (q->shrink_requested && q->free_space >= (q->queue_size - q->new_size)) {
            resize_queue(q, q->new_size); 
            q->shrink_requested = 0;    
        }

      
        if (q->free_space == 0) {
            sem_V(sem_mutex); 
            sem_V(sem_empty); 
            break; 
        }

        message msg;
        generate_message(&msg, &seed);
        enqueue(q, &msg);  

        int count = q->added_messages; 
        sem_V(sem_mutex); 

       
        printf("Produced message %d: type=%d, size=%d, hash=%u\n",
               count, msg.type, msg.size, msg.hash);
        fflush(stdout);

       
        sem_V(sem_fill);  
        sleep(3); 
    }
}
