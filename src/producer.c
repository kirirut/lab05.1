#include "producer.h"

void producer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex) {
    unsigned int seed = time(NULL) ^ getpid();

    for (int i = 0; i < q->queue_size; ++i) {
        sem_P(sem_empty);  // Ожидаем, пока не будет место в очереди
        sem_P(sem_mutex);  // Блокируем очередь для других производителей

        message msg;
        generate_message(&msg, &seed);

        if (q->free_space == 0) {
            sem_V(sem_mutex); 
            sem_V(sem_empty); 
            break; 
        }

        enqueue(q, &msg);  
        
        int count = q->added_messages; // Считаем количество добавленных сообщений

        sem_V(sem_mutex);  // Освобождаем очередь

        printf("Produced message %d: type=%d, size=%d, hash=%u\n",
               count, msg.type, msg.size, msg.hash);
        fflush(stdout);

        sem_V(sem_fill);  // Уведомляем потребителей, что есть новое сообщение
        sleep(1);  
    }
}
