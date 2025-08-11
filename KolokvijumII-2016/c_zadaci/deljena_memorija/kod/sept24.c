#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define N 5

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

struct shared_memory {
    int sig;
    int arr[N];
    int suma;
};

int main() {
    int mem = shmget(10100, sizeof(struct shared_memory), IPC_CREAT | 0666);

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
        struct shared_memory* shm = (struct shared_memory*)shmat(mem, NULL, 0);

        while(1) {
            semop(sem2, &lock, 1);

            for(int i = 0; i < N; i++) {
                shm->suma += shm->arr[i];
            }

            if(shm->suma == 0) {
                shm->sig = 1;
                break;
            }
            else {
                shm->sig = 0;
            }

            semop(sem3, &unlock, 1);
        }

        shmdt(shm);
    }
    else {
        struct shared_memory* shm = (struct shared_memory*)shmat(mem, NULL, 0);

        while(1) {
            semop(sem1, &lock, 1);

            for(int i = 0; i < N; i++) {
                printf("Unesite [%d]. broj: ", i+1);
                scanf("%d", &shm->arr[i]);
            }

            semop(sem2, &unlock, 1);

            semop(sem3, &lock, 1);

            if(shm->sig) {
                break;
            }

            printf("Suma prosledjenih brojeva je: [%d]\n", shm->suma);

            semop(sem1, &unlock, 1);
        }

        shmdt(shm);

        wait(NULL);

        shmctl(mem, IPC_RMID, 0);
        semctl(sem1, 0, IPC_RMID, 0);
        semctl(sem2, 0, IPC_RMID, 0);
        semctl(sem3, 0, IPC_RMID, 0);
    }
}