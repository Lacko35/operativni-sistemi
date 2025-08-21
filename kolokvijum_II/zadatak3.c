#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

void catch_SIGUSR1(int sig) {
    if(sig == SIGUSR1) {
        FILE* f = fopen("brojevi2.dat", "a");

        fprintf(f, "%d ", rand() % 201);

        fclose(f);
    }
}

int main() {
    signal(SIGUSR1, catch_SIGUSR1);
    srand(time(NULL));

    pid_t pid = fork();

    if(pid < 0) {
        perror("Greska: neuspesno kreiranje novog procesa!\n");
        exit(EXIT_FAILURE);
    }

    if(pid == 0) {
        while(1) {
            pause();
        }
    }
    else {
        for(int i = 0; i < 10; i++) {
            kill(pid, SIGUSR1);

            sleep((rand() % 5) + 1);
        }

        kill(pid, SIGKILL);
        wait(NULL);
    }

    return 0;
}