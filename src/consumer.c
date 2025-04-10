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
    for (int i = 0; i < 10; ++i) {
        sem_P(sem_fill);  // Ожидаем, пока в очереди не появится сообщение
        sem_P(sem_mutex);  // Блокируем очередь для других потребителей

        // Проверяем, если очередь пуста, то выходим из цикла
        if (q->free_space == QUEUE_SIZE) {
            sem_V(sem_mutex);
            break;  // Прерываем цикл, если очередь пуста
        }

        message msg = dequeue(q);
        int count = q->removed_messages;

        sem_V(sem_mutex);  // Освобождаем очередь

        uint16_t actual_hash = recalculate_hash(&msg);
        int is_valid = (actual_hash == msg.hash);

        printf("Consumed message %d: type=%d, size=%d, hash=%u — %s\n",
               count, msg.type, msg.size, msg.hash,
               is_valid ? "OK" : "CORRUPTED");
        fflush(stdout);

        sem_V(sem_empty);  // Уведомляем производителей, что место освободилось
        sleep(1);  // Задержка между потреблением сообщений
    }
}
