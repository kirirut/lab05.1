#ifndef SEMAPHORE_UTILS_H
#define SEMAPHORE_UTILS_H

#include <sys/sem.h>

extern struct sembuf P;
extern struct sembuf V;


void sem_P(int semid);
void sem_V(int semid);

#endif // SEMAPHORE_UTILS_H
