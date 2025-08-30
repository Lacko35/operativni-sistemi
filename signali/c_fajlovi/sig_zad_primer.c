#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void signal_handler(int sig) {
    if(sig == SIGUSR1) {
        printf("[Roditelj] primio SIGUSR1 od strane deteta\n");
    }
}

int main() {
    signal(SIGUSR1, signal_handler);

    pid_t procc = fork();

    if(procc < 0) {
        perror("Doslo je do greske prilikom kreiranja procesa!");
        exit(1);
    }

    if(procc == 0) {
        printf("PID deteta: [%d]\n", getpid());

        sleep(2);

        kill(getppid(), SIGUSR1);

        exit(0);
    }
    else {
        printf("PID roditelja: [%d], PID deteta: [%d]\n", getpid(), procc);

        pause();

        wait(NULL);

        printf("Proces roditelj izlazi, proces dete zavrsio!\n");
    }

    return 0;
}