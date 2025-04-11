#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdlib.h>
#include "message_queue.h"
#include "producer.h"
#include "consumer.h"
#include "semaphore_utils.h"
#include "globals.h"
int sem_empty, sem_fill, sem_mutex;
pthread_t *producer_threads;
pthread_t *consumer_threads;
int producer_count = 0;
int consumer_count = 0;

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