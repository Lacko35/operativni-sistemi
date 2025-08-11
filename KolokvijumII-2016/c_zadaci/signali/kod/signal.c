#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FAJL "prenos.dat"
#define BROJ_ITERACIJA 4096
#define DUZINA_NIZA 1024

int sig_ = 0;

void signal_handler(int sig) {
    sig_ = 1;
}

void parent_process(pid_t child_pid) {
    FILE* fajl;
    int brojevi[DUZINA_NIZA];

    for(int i = 0; BROJ_ITERACIJA; i++) {
        for(int j = 0; j < DUZINA_NIZA; j++) {
            brojevi[j] = rand() % 1000;
        }

        fajl = fopen(FAJL, "wb");
        fwrite(brojevi, sizeof(int), DUZINA_NIZA, fajl);
        fclose(fajl);

        kill(child_pid, SIGUSR1);

        while(!sig_) {
            pause();
        }
        sig_ = 0;
    }
}

void child_process(pid_t parent_process) {
    FILE* f;
    int brojevi[DUZINA_NIZA];

    for(int i = 0; i < BROJ_ITERACIJA; i++) {
        while(!sig_) {
            pause();
        }
        sig_ = 0;

        f = fopen(FAJL, "rb");
        fread(brojevi, sizeof(int), DUZINA_NIZA, f);
        fclose(f);

        for(int j = 0; j < DUZINA_NIZA; j++) {
            if(j < 10) {
                printf("%d. clan niza je: [%d] ", j+1, brojevi[j]);
            }
        }
        printf("\n");

        kill(parent_process, SIGUSR2);
    }
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    pid_t pid = fork();

    if(pid > 0) {
        parent_process(pid);
        wait(NULL);
    }
    else {
        child_process(getppid());
    }

    return 0;
}