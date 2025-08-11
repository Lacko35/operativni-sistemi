#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define NUMBER_OF_REDINGS 10

union semun {
    int val;
    short* arr;
    struct semid_ds* buff;
};

struct shared_memory {
    int sig;
    int arr[NUMBER_OF_REDINGS];
};

int main() {
    int mem = shmget(10101, sizeof(struct shared_memory), IPC_CREAT | 0666);

    int sem1 = semget(10102, 1, IPC_CREAT | 0666);
    int sem2 = semget(10103, 1, IPC_CREAT | 0666);

    struct sembuf semlock = {0, -1, 0};
    struct sembuf semunlock = {0, 1, 0};

    union semun semopts;

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    if(fork() == 0) {
        struct shared_memory* shm = (struct shared_memory*) shmat(mem, NULL, 0);

        while(1) {
            semop(sem2, &semlock, 1);

            if(shm->sig) {
                break;
            }

            int suma = 0;
            for(int i = 0; i < NUMBER_OF_REDINGS; i++) {
                suma += shm->arr[i];
            }
            printf("Suma procitanih brojeva: [%d]\n", suma);

            semop(sem1, &semunlock, 1);
        }

        shmdt(shm);
    }
    else {
        struct shared_memory* shm = (struct shared_memory*) shmat(mem, NULL, 0);

        FILE* f = fopen("c_zadaci/deljena_memorija/datoteke/brojevi.txt", "r");

        while(!feof(f)) {
            int numReading = 0;

            semop(sem1, &semlock, 1);

            shm->sig = 0;

            for(int i = 0; i < NUMBER_OF_REDINGS; i++) {
                fscanf(f, "%d ", &shm->arr[i]);
                numReading++;
            }

            if(numReading != NUMBER_OF_REDINGS) {
                for(int i = numReading; i < NUMBER_OF_REDINGS; i++) {
                    shm->arr[i] = 0;
                }
            }

            semop(sem2, &semunlock, 1);
        }

        fclose(f);

        semop(sem1, &semlock, 1);
        shm->sig = 1;
        semop(sem2, &semunlock, 1);

        shmdt(shm);

        wait(NULL);

        shmctl(mem, IPC_RMID, 0);
        semctl(sem1, 0, IPC_RMID, 0);
        semctl(sem2, 0, IPC_RMID, 0);
    }

    return 0;
}