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
    q->shrink_requested = 0;
    q->new_size=queue_size;
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
    printf("________________________________________\n");
    printf("Queue state:\n");
    printf("Head: %d, Tail: %d\n", q->head, q->tail);
    printf("Added messages: %d, Removed messages: %d\n", q->added_messages, q->removed_messages);
    printf("Free space: %d\n", q->free_space);
    printf("Old queue size: %d\n", q->queue_size);
    printf("New queue size: %d\n", q->new_size);
    printf("________________________________________\n");
}
void destroy_queue(message_queue* q) {
    if (q->buffer != NULL) {
        free(q->buffer);
        q->buffer = NULL;  
    }
}

void request_shrink_queue(message_queue *q, int new_size) {
    if (new_size < q->queue_size && new_size >= (q->queue_size - q->free_space)) {
        q->shrink_requested = 1;
        q->new_size = new_size;
        printf("Shrink to %d requested\n", new_size);
    } else {
        printf("Cannot shrink: too many elements in queue\n");
    }
}
void resize_queue(message_queue *q, int new_size) {
    message *new_buffer = malloc(sizeof(message) * new_size);
    if (!new_buffer) {
        fprintf(stderr, "Failed to allocate memory for new queue\n");
        return;
    }

    int count = 0;
    for (int i = 0; i < q->queue_size - q->free_space; ++i) {
        new_buffer[i] = q->buffer[(q->head + i) % q->queue_size];
        count++;
    }

    free(q->buffer);
    q->buffer = new_buffer;
    q->head = 0;
    q->tail = count % new_size;
    q->free_space = new_size - count;
    q->queue_size = new_size;

    // Пересоздание семафоров:
    semctl(sem_empty, 0, IPC_RMID);
    semctl(sem_fill, 0, IPC_RMID);
    semctl(sem_mutex, 0, IPC_RMID);

    sem_empty = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    sem_fill = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    sem_mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);

    semctl(sem_empty, 0, SETVAL, q->free_space);
    semctl(sem_fill, 0, SETVAL, count);
    semctl(sem_mutex, 0, SETVAL, 1);

    printf("Queue resized to %d\n", new_size);
}
