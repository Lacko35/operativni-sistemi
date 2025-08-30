#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAX_LINE_SIZE 256
#define SHM_SIZE 40
#define SHM_KEY 10100
#define SEM1_KEY 10101
#define SEM2_KEY 10102
#define CHILD_EXIT -9999

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main(int argc, char* argv[]) {
    int shmID = shmget((key_t)SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);

    if(shmID == -1) {
        perror("Greska: neuspesno kreiranje pokazivaca na deljenu memoriju!\n");
        exit(1);
    }

    int sem1 = semget(SEM1_KEY, 1, IPC_CREAT | 0666);

    if(sem1 == -1) {
        perror("Greska: neuspesno kreiranje procesa sem1!\n");
        exit(1);
    }

    int sem2 = semget(SEM2_KEY, 1, IPC_CREAT | 0666);

    if(sem2 == -1) {
        perror("Greska: neuspesno kreiranje procesa sem1!\n");
        exit(1);
    }

    union semun semopts;
    struct sembuf semlock = {0, -1, 0};
    struct sembuf semunlock = {0, 1, 0};

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    pid_t child = fork();

    if(child == 0) {
        int* shm = (int*) shmat(shmID, NULL, 0);
        
        int current = 1;

        while(1) {
            semop(sem2, &semlock, 1);

            if(shm[0] == CHILD_EXIT) {
                break;
            }

            int suma = 0;

            for(int i = 0; i < 10; i++) {
                suma += shm[i];
            }

            printf("Suma brojeva u %d. redu fajla je: %d\n", current++, suma);
            fflush(stdout);

            semop(sem1, &semunlock, 1);
        }

        shmdt(shm);

        exit(0);
    }
    else if(child > 0) {
        int* shm = (int*) shmat(shmID, NULL, 0);

        FILE* f = fopen(FILE_NAME, "r");

        char buffer[MAX_LINE_SIZE];

        while(fgets(buffer, MAX_LINE_SIZE, f)) {
            int n = 0;

            semop(sem1, &semlock, 1);

            char* token = strtok(buffer, " \t\n");

            while(token != NULL & n < 10) {
                shm[n++] = atoi(token);

                token = strtok(NULL, " \t\n");
            }
            
            if(n != 10) {
                for(int i = n; i < 10; i++) {
                    shm[i] = 0;
                }
            }

            semop(sem2, &semunlock, 1);
        }

        fclose(f);

        semop(sem1, &semlock, 1);

        shm[0] = CHILD_EXIT;

        semop(sem2, &semunlock, 1);

        shmdt(shm);

        wait(NULL);

        shmctl(shmID, IPC_RMID, NULL);
        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);    
    }
    else {
        perror("Greska: neuspesno kreiranje procesa deteta!\n");
        exit(1);
    }

    return 0;
}