#include "producer.h"

void producer(message_queue* q, int sem_empty, int sem_fill, int sem_mutex) {
    unsigned int seed = time(NULL) ^ getpid();

    for (int i = 0; i < 10; ++i) {
        sem_P(sem_empty);  // Ожидаем, пока не освободится место
        sem_P(sem_mutex);  // Блокируем очередь для других производителей

        message msg;
        generate_message(&msg, &seed);

        // Проверяем, есть ли место в очереди
        if (q->free_space == 0) {
            sem_V(sem_mutex);  // Освобождаем mutex, если очередь полна
            sem_V(sem_empty);  // Уведомляем потребителей, что место освободилось
            break;  // Прерываем цикл, если очередь полна
        }

        enqueue(q, &msg);  // Добавляем сообщение в очередь
        int count = q->added_messages;

        sem_V(sem_mutex);  // Освобождаем mutex

        printf("Produced message %d: type=%d, size=%d, hash=%u\n",
               count, msg.type, msg.size, msg.hash);
        fflush(stdout);

        sem_V(sem_fill);  // Уведомляем потребителей, что новое сообщение добавлено
        sleep(1);  // Задержка между производством сообщений
    }
}
