#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define LINE_LENGTH 40
#define SHM_KEY 10100
#define SEM1_KEY 10101
#define SEM2_KEY 10102
#define ARRAY_MEMBERS 10

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main() {
    int sharedMemoryID = shmget((key_t)SHM_KEY, 10 * sizeof(int), IPC_CREAT | 0666);

    if(sharedMemoryID == -1) {
        perror("Greska: Neuspesno kreiranje deljene memorije!\n");
        exit(1);
    }

    int sem1ID = semget((key_t)SEM1_KEY, 1, IPC_CREAT | 0666);

    if(sem1ID == -1) {
        perror("Greska: Neuspesno kreiranje semafora!\n");
        exit(1);
    }

    int sem2ID = semget((key_t)SEM2_KEY, 1, IPC_CREAT | 0666);

    if(sem2ID == -1) {
        perror("Greska: Neuspesno kreiranje semafora!\n");
        exit(1);
    }

    union semun semopts;

    struct sembuf semLock = {0, -1, 0};
    struct sembuf semUnlock = {0, 1, 0};

    semopts.val = 1;
    semctl(sem1ID, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2ID, 0, SETVAL, semopts);

    pid_t child = fork();

    if(child == 0) {

    }
    else if(child > 0) {
        FILE* f = fopen("brojevi.txt", "r");
        int* shmem = (int*) shmat(sharedMemoryID, NULL, 0);

        char buffer[LINE_LENGTH];
        while(fgets(buffer, LINE_LENGTH, f)) {
            if(buffer[strlen(buffer) - 1] == '\n') {
                buffer[strcspn(buffer, "\n")] = '\0';
            }

            int current = 0;
            int i = 0;
            while(buffer[i] != '\0') {
                char num[3];

                num[0] = buffer[i];
                num[1] = buffer[i + 1];
                num[2] = '\0';

                shmem[current++] = atoi(num);

                i += 3;
            }

            if(current != 10) {
                for(int i = current; i < 10; i++) {
                    shmem[i] = 0;
                }
            }
        }
        fclose(f);

        shmdt(shmem);

        wait(NULL);

        shmctl();
    }
    else {
        perror("Greska: neuspesno kreiranje novog procesa!\n");
        exit(1);
    }

    return 0;
}