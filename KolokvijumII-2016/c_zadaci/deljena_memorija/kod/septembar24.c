#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define NUM_OF_NUMBER 5

union semun {
    int val;
    short* arr;
    struct semid_ds* buff; 
};

int main() {
    int shm_ref = shmget(10100, IPC_CREAT | 0666);

    int sem1 = semget(10101, 1, IPC_CREAT | 0666);
    int sem2 = semget(10102, 1, IPC_CREAT | 0666);
    int sem3 = semget(10103, 1, IPC_CREAT | 0666);

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun semopts;

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem3, 0, SETVAL, semopts);

    if(fork() == 0) {
        //proces dete
        int* shmem = (int*) shmat(shm_ref, NULL, 0);

        semop(sem2, &lock, 1);

    }
    else {
        //proces roditelj
        int* shmem = (int*) shmat(shm_ref, NULL, 0);
        int suma = 0;
        do {
            semop(sem1, &lock, 1);
            for(int i = 0; i < NUM_OF_NUMBER; i++) {
                int broj;
                printf("Unesite broj: ");
                scanf("%d", &broj);
                suma += broj;

                shmem[i] = broj;
            }
            semop(sem2, &unlock, 1);
        }
        while(suma != 0);
        
        shmdt(shmem);

        wait(NULL);

        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
        semctl(sem3, 0, IPC_RMID, NULL);
        shmctl(shm_ref, IPC_RMID, NULL);
    }

    return 0;
}