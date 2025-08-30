#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>

#define SHM_SIZE 1024

char* shared_memory = NULL;
int shmid;
pid_t pid;

void child_handler(int sig) {
    if(sig == SIGUSR1) {
        printf("\t[Dete]: Primljen signal od strane roditelja, pocinjemo sa obradom podataka!\n");

        for(int i = 0; shared_memory[i] != '\0'; i++) {
            shared_memory[i] = toupper(shared_memory[i]);
        }

        printf("\t[Dete]: Obrada zavrsena. Salje se signal roditelju.\n");

        kill(getppid(), SIGUSR2);
    }
}

void parent_handler(int sig) {
    if(sig == SIGUSR2) {
        printf("\tObradjeni sadrzaj deljene memorije: [%s].\n", shared_memory);
        printf("------------------------------------------------------------------------------------\n");
    }
}

int main() {
    shmid = shmget(10101, SHM_SIZE, IPC_CREAT | 0666);

    if(shmid == -1) {
        perror("Doslo je do greske. shmget() failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = (char*)shmat(shmid, NULL, 0);

    if(shared_memory == ((char*)(-1))) {
        perror("Doslo je do greske. shmat() failed");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, child_handler);
    signal(SIGUSR2, parent_handler);

    pid = fork();

    if(pid < 0) {
        perror("Doslo je do greske. fork() failed");
        exit(EXIT_FAILURE);        
    }

    if(pid == 0) {
        while(1) {
            pause();
        }
    }
    else {
        while(1) {
            printf("Unesite neki tekst ili 'kraj' za prekid programa: ");
            fflush(stdout);
            fgets(shared_memory, SHM_SIZE, stdin);

            printf("------------------------------------------------------------------------------------\n");
            fflush(stdout);

            shared_memory[strcspn(shared_memory, "\n")] = '\0';

            if(strcmp(shared_memory, "kraj") == 0) {
                kill(pid, SIGKILL);
                break;
            }

            kill(pid, SIGUSR1);

            pause();
        }

        shmdt(shared_memory);
        shmctl(shmid, IPC_RMID, NULL);

        printf("\t[Roditelj] zatvaranje programa.\n");
    }

    return 0;
}