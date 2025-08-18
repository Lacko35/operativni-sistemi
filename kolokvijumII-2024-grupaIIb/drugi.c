#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 10101
#define SHM_SIZE 100
#define SEM1_KEY 10102
#define SEM2_KEY 10103

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main() {
    int shm = shmget((key_t), SHM_SIZE, IPC_CREAT | 0666);

    if(shm == -1) {
        perror("Doslo je do greske prilikom kreiranja/ucitavanja deljene memorije!\n");
        exit(1);
    }

    int sem1 = semget((key_t)SEM1_KEY, 1, IPC_CREAT | 0666);

    if(sem1 == -1) {
        perror("Doslo je do greske prilikom kreiranja semafora!\n");
        exit(1);
    }

    int sem2 = semget((key_t)SEM2_KEY, 1, IPC_CREAT | 0666);

    if(sem2 == -1) {
        perror("Doslo je do greske prilikom kreiranja semafora!\n");
        exit(1);
    }

    union semun semopts;
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);    

    pid_t pid = fork();

    if(pid == 0) {
        int* memory = (int*) shmat(shm, NULL, 0);

        semop(sem2, &lock, 1);

        for(int i = 0; i < 5; i++) {
            int max = shm[i][0];
            for(int j = 1; j < 5; j++) {
                if(max < shm[i][j]) {
                    max = shm[i][j];
                }
            }

            printf("Maximalni element %d. vrste je: %d\n", max);
        }

        semop(sem1, &unlock, 1);

        shmdt(memory);

        exit(0);
    }
    else if(pid > 0) {
        int* memory = (int*) shmat(shm, NULL, 0);

        semop(sem1, &lock, 1);

        for(int i = 0; i < 5; i++) {
            for(int j = 0; j < 5; j++) {
                shm[i][j] = ((rand() % 100) + 1);
            }
        }

        semop(sem2, &unlock, 1);

        shmdt(memory);

        wait(NULL);

        shmctl(shm, IPC_RMID, NULL);
        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
    }
    else {
        perror("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }

    return 0;
}