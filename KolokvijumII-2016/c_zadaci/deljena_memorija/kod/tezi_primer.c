#include "../zaglavlje.h"

int main() {
    int shmid = shmget(SHM_KEY, sizeof(struct shared_memory) - sizeof(int), IPC_CREAT | 0666);
    int sem1 = semget(sem1, 1, IPC_CREAT | 0666);
    int sem2 = semget(sem2, 1, IPC_CREAT | 0666);

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun semopts;

    semopts.val = 1;
    semctl(sem1, 0, SETVAL, semopts);

    semopts.val = 0;
    semctl(sem2, 0, SETVAL, semopts);

    if(fork() != 0) {
        FILE* f = fopen("c_zadaci/datoteke/poruka.txt", "r");
        struct shared_memory* shm = (struct shared_memory*)shmat(shmid, NULL, 0);

        while(!feof(f)) {
            semop(sem1, &lock, 1);

            fgets(shm->text, LINE_LEN, f);
            shm->kraj = 0;

            semop(sem2, &unlock, 1);
        }

        fclose(f);

        semop(sem1, &lock, 1);

        shm->kraj = 1;

        semop(sem2, &unlock, 1);

        shmdt(shm);

        shmctl(shmid, IPC_RMID, NULL);
        semctl(sem1, 0, IPC_RMID, NULL);
        semctl(sem2, 0, IPC_RMID, NULL);
    }
    else {
        struct shared_memory* shm = (struct shared_memory*)shmat(shmid, NULL, 0);
        int rbr = 0;

        while(1) {
            semop(sem2, &lock, 1);

            if(shm->kraj) {
                break;
            }

            rbr++;
            printf("%d. recenica iz fajla je -> %s\n", rbr, shm->text);

            semop(sem1, &unlock, 1);
        }

        shmdt(shm);
    }

    return 0;
}