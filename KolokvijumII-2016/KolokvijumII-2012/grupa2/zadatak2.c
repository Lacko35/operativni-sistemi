#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define SHM_KEY 10100
#define SEM1_KEY 10101
#define SEM2_KEY 10102
#define NUMBER_OF_ITERATIONS 100

struct shared_memory {
    int num1;
    int num2;
    int previousSum;
};

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main() {
    int memid = shmget(SHM_KEY, sizeof(struct shared_memory), IPC_CREAT | 0666);

    int sem1 = semget(SEM1_KEY, 1, IPC_CREAT | 0666);
    int sem2 = semget(SEM2_KEY, 1, IPC_CREAT | 0666);

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun semopts;

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    if(fork() == 0) {
        struct shared_memory* shm = (struct shared_memory*)shmat(memid, NULL, 0);
        shm->previousSum = 0;
        
        for(int i = 0; i < NUMBER_OF_ITERATIONS; i++) {
            semop(sem2, &lock, 1);
                printf("Prosledjeni brojevi su: %d | %d\n", shm->num1, shm->num2);

                printf("Njihova suma je: %d\n", shm->num1 + shm->num2);

                shm->previousSum += shm->num1 + shm->num2;

                printf("Suma prethodnih: %d\n", shm->previousSum);

                printf("-----------------------------------------------------------\n");
            semop(sem1, &unlock, 1);
        }
    }
    else {
        struct shared_memory* shm = (struct shared_memory*)shmat(memid, NULL, 0);

        for(int i = 0; i < NUMBER_OF_ITERATIONS; i++) {
            semop(sem1, &lock, 1);
                shm->num1 = rand() % 100;

                shm->num2 = rand() % 100;
            semop(sem2, &unlock, 1);
        }

        wait(NULL);

        shmctl(memid, IPC_RMID, NULL);
        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
    }

    return 0;
}