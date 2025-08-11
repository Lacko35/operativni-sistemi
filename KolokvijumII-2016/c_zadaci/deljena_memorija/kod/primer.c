#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define SHM_KEY 10101
#define SEM1_KEY 10102
#define SEM2_KEY 10103
#define ITERATIONS 5

union semun {
    int val;
    short* arr;
    struct semid_ds* buff;
};

int main() {
    int shmid = shmget(SHM_KEY, ITERATIONS * sizeof(int), IPC_CREAT | 0666);

    int sem1 = semget(SEM1_KEY, 1, IPC_CREAT | 0666);
    int sem2 = semget(SEM2_KEY, 1, IPC_CREAT | 0666);

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun semopts;
    
    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    if(fork() != 0) {
        int* shm = (int*)shmat(shmid, NULL, 0);

        semop(sem1, &lock, 1);

        for(int i = 0; i < ITERATIONS; i++) {
            int broj = (rand() % 1000) + 1;

            shm[i] = broj;
        }

        semop(sem2, &unlock, 1);

        shmdt(shm);

        shmctl(shmid, IPC_RMID, NULL);
        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
    }
    else {
        int* shm = (int*)shmat(shmid, NULL, 0);

        semop(sem1, &lock, 1);

        for(int i = 0; i < ITERATIONS; i++) {
            int broj = shm[i];

            printf("\t%d. broj prosledjen je: [%d]\n", i+1, broj);
        }

        semop(sem2, &unlock, 1);

        shmdt(shm);        
    }

    return 0;
}