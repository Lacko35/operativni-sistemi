#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main() {
    int shmid = shmget(10101, sizeof(int), IPC_CREAT | 0666);

    int sem1 = semget(10101, 1, IPC_CREAT | 0666);
    int sem2 = semget(10102, 1, IPC_CREAT | 0666);

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun semopts;

    semopts.val = 0;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 1;
    semctl(sem2, 0, SETVAL, semopts);
    
    if(fork() == 0) {
        int* shm = (int*) shmat(shmid, NULL, 0);

        for(int i = 0; i < 10; i++) {
            semop(sem2, &lock, 1);
                shm[0] = getpid();
            semop(sem1, &unlock, 1);
        }

        shmdt(shm);
    }
    else {
        int* shm = (int*) shmat(shmid, NULL, 0);

        for(int i = 0; i < 10; i++) {

            semop(sem1, &lock, 1);
                if(getpid() == (shm[0] + 1)) {
                    printf("PID procesa roditelja [%d] je za 1 veci od PID procesa deteta [%d]\n", getpid(), shm[0]);
                    printf("--------------------------------------------------------------------------------------------\n");
                }
                else {
                    printf("PID procesa roditelja [%d] nije za 1 veci od PID procesa deteta [%d]\n", getpid(), shm[0]);
                    printf("---------------------------------------------------------------------------------------------\n");
                }
            semop(sem2, &unlock, 1);
        }

        shmdt(shm);

        wait(NULL);

        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}