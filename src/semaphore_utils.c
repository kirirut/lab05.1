#include "semaphore_utils.h"

struct sembuf P = {0, -1, 0};
struct sembuf V = {0, 1, 0};

void sem_P(int semid) {
    semop(semid, &P, 1);
}

void sem_V(int semid) {
    semop(semid, &V, 1);
}
void init_semaphores() {
    sem_empty = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    sem_fill = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    sem_mutex = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

    semctl(sem_empty, 0, SETVAL, QUEUE_SIZE);
    semctl(sem_fill, 0, SETVAL, 0);
    semctl(sem_mutex, 0, SETVAL, 1);
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
    producer_threads = realloc(producer_threads, (producer_count + 1) * sizeof(pthread_t));
    consumer_threads = realloc(consumer_threads, (consumer_count + 1) * sizeof(pthread_t));
    if (producer_threads == NULL || consumer_threads == NULL) {
        fprintf(stderr, "Memory allocation for threads failed\n");
        exit(1);
    }
}