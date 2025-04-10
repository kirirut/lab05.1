#include "semaphore_utils.h"

struct sembuf P = {0, -1, 0};
struct sembuf V = {0, 1, 0};

void sem_P(int semid) {
    semop(semid, &P, 1);
}

void sem_V(int semid) {
    semop(semid, &V, 1);
}
