#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define LINE_LENGTH 80

union semun {
    int val;
    unsigned short* array;
    struct semid_ds* buff;
};

struct shared_memory {
    char fname[30];
    char line[LINE_LENGTH];
    int linesNum;
};

int main(int argc, char* argv[]) {
    int shm_id = shmget(10101, sizeof(struct shared_memory), IPC_CREAT | 0666);
    
    if(shm_id == -1) {
        perror("GRESKA: neuspesno kreiranje/ucitavanje deljene memorije!\n");
        return 1;
    }

    int sem1 = semget(10102, 1, IPC_CREAT | 0666);

    if(sem1 == -1) {
        perror("GRESKA: neuspesno kreiranje semafora!\n");
        return 1;
    }

    int sem2 = semget(10103, 1, IPC_CREAT | 0666);

    if(sem2 == -1) {
        perror("GRESKA: neuspesno kreiranje semafora!\n");
        return 1;
    }

    union semun semopts;
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    pid_t chld_pid = fork();

    if(chld_pid < 0) {
        perror("GRESKA: neuspesno kreiranje novog procesa!\n");
        return 1;
    }
    else if(chld_pid == 0) {
        struct shared_memory* shm = (struct shared_memory*)shmat(shm_id, NULL, 0);

        semop(sem2, &lock, 1);

        FILE* f = fopen(shm->fname, "r");

        if(f == NULL) {
            perror("GRESKA: neuspesno otvaranje fajla!\n");
            return 1;
        }

        while(shm->linesNum > 0) {
            char line[LINE_LENGTH];

            fgets(line, LINE_LENGTH, f);

            line[0] = tolower(line[0]);
            for(int i = 1; i < strlen(line); i++) {
                line[i] = toupper(line[i]);
            }

            printf("linija: %s\n", line);
            fflush(stdout);

            shm->linesNum -= 1;
        }

        fclose(f);

        semop(sem1, &unlock, 1);

        shmdt(shm);
    }
    else {
        struct shared_memory* shm = (struct shared_memory*)shmat(shm_id, NULL, 0);

        semop(sem1, &lock, 1);

        strcpy(shm->fname, argv[1]);
        shm->linesNum = atoi(argv[2]);

        semop(sem2, &unlock, 1);

        shmdt(shm);

        wait(NULL);
    
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
    }

    return 0;
}