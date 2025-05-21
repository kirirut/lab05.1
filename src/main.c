#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t running = 1;

void stop_running(int sig) {
    running = 0;
}

int main() {
    Queue queue;
    init_queue(&queue, 10);

    pthread_t producers[MAX_THREADS];
    pthread_t consumers[MAX_THREADS];
    int num_producers = 0;
    int num_consumers = 0;

    signal(SIGINT, stop_running); // Ctrl+C обработчик

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    printf("Message queue created. Press:\n");
    printf("  'p' - create producer\n");
    printf("  'c' - create consumer\n");
    printf("  's' - show queue status\n");
    printf("  '+' - +1 place\n");
    printf("  '-' - -1 place\n");
    printf("  'q' - quit\n");

    int ch;
    while (running) {
        ch = getchar();
        if (ch == EOF || ch == '\n') {
            struct timespec ts = {0, 10000000};
            nanosleep(&ts, NULL);
            continue;
        }

        if (ch == 'p' && num_producers < MAX_THREADS) {
            pthread_create(&producers[num_producers++], NULL, producer, &queue);
        } else if (ch == 'c' && num_consumers < MAX_THREADS) {
            pthread_create(&consumers[num_consumers++], NULL, consumer, &queue);
        } else if (ch == 's') {
            pthread_mutex_lock(&queue.mutex);
            int capacity = queue.capacity;
            int occupied = capacity - queue.free_space;
            int free_space = queue.free_space;
            int added = queue.added_count;
            int removed = queue.removed_count;
            pthread_mutex_unlock(&queue.mutex);
            printf("Queue size: %d, occupied: %d, free: %d, added: %d, removed: %d, producers: %d, consumers: %d\n",
                   capacity, occupied, free_space, added, removed, num_producers, num_consumers);
        } else if (ch == '+') {
            resize_queue(&queue, queue.capacity + 1);
        } else if (ch == '-') {
            if (queue.capacity > 1) resize_queue(&queue, queue.capacity - 1);
        } else if (ch == 'q') {
            running = 0;
        }
    }

    // Дожидаемся завершения потоков
    for (int i = 0; i < num_producers; i++) pthread_join(producers[i], NULL);
    for (int i = 0; i < num_consumers; i++) pthread_join(consumers[i], NULL);

    pthread_mutex_destroy(&queue.mutex);
    sem_destroy(&queue.empty);
    sem_destroy(&queue.full);
    free(queue.messages);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}
