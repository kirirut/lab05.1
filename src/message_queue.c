#include "message_queue.h"
#include "semaphore_utils.h"
// Инициализация очереди
void init_queue(message_queue* q, int queue_size) {
    // Проверка на корректность размера
    if (queue_size <= 0) {
        fprintf(stderr, "Queue size must be greater than 0.\n");
        exit(1);
    }

    q->head = 0;
    q->tail = 0;
    q->added_messages = 0;
    q->removed_messages = 0;
    q->free_space = queue_size;
    q->queue_size=queue_size;
    // Выделяем память для массива сообщений
    q->buffer = (message*)malloc(queue_size * sizeof(message));
    if (q->buffer == NULL) {
        // Обработка ошибки выделения памяти
        fprintf(stderr, "Error allocating memory for the message queue.\n");
        exit(1);
    }

    printf("Queue initialized with size %d\n", queue_size);
}

// Очередь (добавление сообщения)
void enqueue(message_queue* q, const message* msg) {
    // Захватываем мьютекс
    sem_P(sem_mutex);

    // Если очередь полна, ждем освобождения места
    if (q->free_space == 0) {
        sem_V(sem_empty);  // Поставим семафор в состоянии ожидания, пока не появится место
        sem_P(sem_mutex);  // Захватываем мьютекс снова
    }

    // Добавляем сообщение в очередь
    q->buffer[q->tail] = *msg;
    q->added_messages++;
    q->tail = (q->tail + 1) % q->queue_size;
    q->free_space--;

    // Увеличиваем количество занятых мест
    sem_V(sem_fill);

    // Освобождаем мьютекс
    sem_V(sem_mutex);
}

message dequeue(message_queue* q) {
    // Захватываем мьютекс
    sem_P(sem_mutex);

    // Если очередь пуста, ждем появления новых сообщений
    if (q->free_space == q->queue_size) {
        sem_V(sem_fill);  // Ожидаем заполнения очереди
        sem_P(sem_mutex);  // Захватываем мьютекс снова
    }

    // Извлекаем сообщение из очереди
    message msg = q->buffer[q->head];
    q->head = (q->head + 1) % q->queue_size;
    q->free_space++;
    q->removed_messages++;

    // Уменьшаем количество свободных мест
    sem_V(sem_empty);

    // Освобождаем мьютекс
    sem_V(sem_mutex);

    return msg;
}

// Печать состояния очереди (для отладки)
void print_queue_state(message_queue* q) {
    printf("Queue state:\n");
    printf("Head: %d, Tail: %d\n", q->head, q->tail);
    printf("Added messages: %d, Removed messages: %d\n", q->added_messages, q->removed_messages);
    printf("Free space: %d\n", q->free_space);
}
void destroy_queue(message_queue* q) {
    if (q) {
        free(q->buffer);
        free(q);
    }
}