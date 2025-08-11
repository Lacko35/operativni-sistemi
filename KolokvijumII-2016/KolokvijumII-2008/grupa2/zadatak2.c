#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define N 10

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main(int argc, char* argv[]) {
    int mem = shmget(10101, N * sizeof(int), IPC_CREAT | 0666);
    
    int sem1 = semget(10102, 1, IPC_CREAT | 0666);
    int sem2 = semget(10103, 1, IPC_CREAT | 0666);

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun semopts;

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    if(fork() == 0) {
        int* shm = (int*)shmat(mem, NULL, 0);

        semop(sem2, &lock, 1);

        for(int i = 0; i < N; i++) {
            printf("%d ", shm[i]);
        }
        
        semop(sem1, &unlock, 1);

        exit(0);
    }
    else {
        int* shm = (int*)shmat(mem, NULL, 0);

        semop(sem1, &lock, 1);

        for(int i = 0; i < N; i++) {
            shm[i] = rand() % 100;
        }

        semop(sem2, &unlock, 1);

        shmdt(shm);

        wait(NULL);

        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
        shmctl(mem, IPC_RMID, NULL);
    }

    return 0;
}