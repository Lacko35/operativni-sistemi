#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>

union semun {
    int val;
};

int main() {
    int shmid = shmget(10101, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    int sem1 = semget(10101, 1, IPC_CREAT | 0666); // dete -> roditelj
    int sem2 = semget(10102, 1, IPC_CREAT | 0666); // roditelj -> dete

    union semun semopts;

    semopts.val = 0;
    semctl(sem1, 0, SETVAL, semopts); // dete postavlja kada je gotovo

    semopts.val = 1;
    semctl(sem2, 0, SETVAL, semopts); // roditelj dozvoljava detetu da pise

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    int* shm = (int*) shmat(shmid, NULL, 0);

    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Dete
            semop(sem2, &lock, 1); // Ceka dozvolu od roditelja
                shm[0] = getpid(); // Upisuje svoj PID
            semop(sem1, &unlock, 1); // Obavestava roditelja
            shmdt(shm);
            exit(0);
        }
        else {
            // Roditelj
            semop(sem1, &lock, 1); // Ceka da dete upise PID
                int pid_dete = shm[0];
                if (pid_dete == getpid() + 1) {
                    printf("PID procesa deteta [%d] je za 1 veci od PID-a roditelja [%d]\n", pid_dete, getpid());
                } else {
                    printf("PID procesa deteta [%d] NIJE za 1 veci od PID-a roditelja [%d]\n", pid_dete, getpid());
                }
                printf("--------------------------------------------------------------------------------\n");
            semop(sem2, &unlock, 1); // Daje dozvolu za sledeće dete

            wait(NULL); // Ceka dete da zavrsi
        }
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(sem1, 0, IPC_RMID);
    semctl(sem2, 0, IPC_RMID);

    return 0;
}