#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FILE_SIZE 1024

struct message {
    long type;
    int arr[256];
    int num;
};

pid_t child1, child2;
int iterations = 0;
int msgQueueID;

void child_handler(int sig) {
    if(sig == SIGUSR1) {
        FILE* f = fopen("binarni.txt", "w");

        for(int i = 0; i < FILE_SIZE / sizeof(int); i++) {
            fprintf(f, "%d\n", rand() % 100);
        }

        fflush(f);
        fclose(f);

        kill(getppid(), SIGUSR2);
    }
}

void child2_handler(int sig) {
    if(sig == SIGUSR1) {
        struct message msg;
        msgrcv(msgQueueID, &msg, 256 * sizeof(int), 1, 0);

        int oddNumbers = 0;

        for(int i = 0; i < msg.arr; i++) {
            if(msg.arr[i] % 2 == 0) {
                oddNumbers++;
            }
        }

        struct message snd;
        snd.type = 2;
        snd.num = oddNumbers;

        msgsnd(msgQueueID, &snd, sizeof(snd.num), 0);
    }
}

void parent_handler(int sig) {
    if(sig == SIGUSR2) {
        int oddNumbers = 0;
        iterations++;

        FILE* f = fopen("binarni.txt", "r");

        char buff[10];
        while(fgets(buff, 10, f) != 0) {
            int num = atoi(buff);

            if(num % 2 == 0) {
                oddNumbers++;
            }
        }

        printf("U %d. iteraciji je bilo %d parnih brojeva.\n", iterations, oddNumbers);
        fflush(stdout);

        if(iterations >= 5) {
            kill(child1, SIGKILL);

            exit(0);
        }

        fclose(f);
    }
}

int main() {
    srand(time(NULL));

    signal(SIGUSR2, parent_handler);

    child1 = fork();

    if(child1 == 0) {
        signal(SIGUSR1, child_handler);

        while(1) {
            pause();
        }
    }

    while(1) {
        sleep(5);
 
        kill(child1, SIGUSR1);
    }

    return 0;
}