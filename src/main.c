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
int run=1;
int main() {
    int queue_size;
    printf("Enter the size of the message queue: ");
    if (scanf("%d", &queue_size) != 1 || queue_size <= 0) {
        fprintf(stderr, "Invalid queue size.\n");
        exit(1);
    }

    message_queue q;
    init_queue(&q, queue_size); 
    init_semaphores(&q);

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
    printf("  '+' - +1 place\n");
    printf("  '-' - -1 place\n");
    printf("  'q' - quit\n");

    char command;

    while (run) {
        command = getchar();
        if (command == '\n') continue;

        if (command == 'p') {
            expand_thread_arrays();
            pthread_create(&producer_threads[producer_count], NULL, (void*)producer, &q);
            producer_count++;
        }
        else if (command == 'c') {
            expand_thread_arrays();
            pthread_create(&consumer_threads[consumer_count], NULL, (void*)consumer, &q);
            consumer_count++;
        }
        else if (command == 's') {
            print_queue_state(&q);
        }
        else if (command == 'q') {
            run = 0;
            break;
        }
        else if (command == '+') {
            int new_size = q.queue_size + 1;
            resize_queue(&q, new_size);
        }
        else if (command == '-') {
            if (q.queue_size > q.free_space + 1) {
                int new_size = q.queue_size - 1;
                request_shrink_queue(&q, new_size);
            } else {
                printf("Cannot shrink: too many elements in queue\n");
            }
        }
    }

    wait_for_threads();

    free(producer_threads);
    free(consumer_threads);

    semctl(sem_empty, 0, IPC_RMID);
    semctl(sem_fill, 0, IPC_RMID);
    semctl(sem_mutex, 0, IPC_RMID);
    destroy_queue(&q);

    return 0;
}