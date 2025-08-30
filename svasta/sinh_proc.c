#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
    int val;
    unsigned short* arr;
    struct semid_ds* buff;
};

int main() {
    key_t sem1 = semget((key_t)10101, 1, IPC_CREAT | 0666);
    
    if(sem1 == -1) {
        perror("GRESKA: semafor sem1 se nije kreirao!\n");
        exit(EXIT_FAILURE);
    }
    
    key_t sem2 = semget((key_t)10102, 1, IPC_CREAT | 0666);

    if(sem2 == -1) {
        perror("GRESKA: semafor sem2 se nije kreirao!\n");
        exit(EXIT_FAILURE);
    }

    union semun semopts;
    struct sembuf semLock = {0, -1, 0};
    struct sembuf semUnlock = {0, 1, 0};

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    pid_t pid = fork();

    if(pid > 0) {
        char letter = 'a';

        for(int i = 0; i < 26; i++) {
            FILE* f = fopen("slova.dat", "a");

            semop(sem1, &semLock, 1);

            fprintf(f, "%c\n", (char)(letter + i));
            fclose(f);

            semop(sem2, &semUnlock, 1);
        }

        wait(NULL);

        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
    }
    else if(pid == 0) {
        char letter = 'A';

        for(int i = 0; i < 26; i++) {
            FILE* f = fopen("slova.dat", "a");

            semop(sem2, &semLock, 1);

            fprintf(f, "%c\n", (char)(letter + i));
            fclose(f);

            semop(sem1, &semUnlock, 1);
        }
    }
    else {
        perror("GRESKA: neuspesno kreiranje procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}