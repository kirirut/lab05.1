#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdlib.h>
#include "message_queue.h"
#include "producer.h"
#include "consumer.h"
#include "semaphore_utils.h"

// Глобальные переменные для семафоров
int sem_empty, sem_fill, sem_mutex;

// Структуры для хранения потоков

pthread_t *producer_threads;
pthread_t *consumer_threads;
int producer_count = 0;
int consumer_count = 0;

void init_semaphores() {
    sem_empty = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    sem_fill = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    sem_mutex = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

    semctl(sem_empty, 0, SETVAL, QUEUE_SIZE);
    semctl(sem_fill, 0, SETVAL, 0);
    semctl(sem_mutex, 0, SETVAL, 1);
}

void print_queue_state(message_queue* q) {
    printf("Queue state: Size = %d, Used = %d, Free = %d\n",
           QUEUE_SIZE, QUEUE_SIZE - q->free_space, q->free_space);
    printf("Added messages: %d\n", q->added_messages);
    printf("Removed messages: %d\n", q->removed_messages);
}

void wait_for_threads() {
    // Ожидание завершения всех потоков
    for (int i = 0; i < producer_count; ++i) {
        pthread_join(producer_threads[i], NULL);
    }
    for (int i = 0; i < consumer_count; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }
}
void expand_thread_arrays() {
    // Увеличиваем размер массивов для потоков
    producer_threads = realloc(producer_threads, (producer_count + 1) * sizeof(pthread_t));
    consumer_threads = realloc(consumer_threads, (consumer_count + 1) * sizeof(pthread_t));
    if (producer_threads == NULL || consumer_threads == NULL) {
        fprintf(stderr, "Memory allocation for threads failed\n");
        exit(1);
    }
}

int main() {
    message_queue q;
    init_queue(&q);

    // Инициализация семафоров
    init_semaphores();

    // Изначально выделяем память для одного потока
    producer_threads = malloc(sizeof(pthread_t));
    consumer_threads = malloc(sizeof(pthread_t));

    if (producer_threads == NULL || consumer_threads == NULL) {
        fprintf(stderr, "Memory allocation for threads failed\n");
        exit(1);
    }

    printf("Message queue created. Press:\n");
    printf("  'p' - create producer\n");
    printf("  'c' - create consumer\n");
    printf("  's' - show queue status\n");
    printf("  'q' - quit\n");

    char command;

    while (1) {
        command = getchar();
        if (command == '\n') continue;

        if (command == 'p') {
            // Расширяем массив потоков, если необходимо
            expand_thread_arrays();
            // Создание нового производителя
            pthread_create(&producer_threads[producer_count], NULL, (void*)producer, &q);
            producer_count++;
        }
        else if (command == 'c') {
            // Расширяем массив потоков, если необходимо
            expand_thread_arrays();
            // Создание нового потребителя
            pthread_create(&consumer_threads[consumer_count], NULL, (void*)consumer, &q);
            consumer_count++;
        }
        else if (command == 's') {
            // Вывод состояния очереди
            print_queue_state(&q);
        }
        else if (command == 'q') {
            break;
        }
    }

    wait_for_threads();

    // Освобождение памяти под потоки
    free(producer_threads);
    free(consumer_threads);

    // Удаление семафоров
    semctl(sem_empty, 0, IPC_RMID);
    semctl(sem_fill, 0, IPC_RMID);
    semctl(sem_mutex, 0, IPC_RMID);

    return 0;
}