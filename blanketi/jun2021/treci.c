#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

pid_t child;
int current = 0;

#define N 10
#define iterations 10

void child_handler(int sig) {
    if(sig == SIGUSR2) {
        FILE* f = fopen("./januar2021/prenos.dat", "wb");

        for(int i = 0; i < N; i++) {
            int num = rand() % 100;
            fwrite(&num, sizeof(int), 1, f);
        }

        fclose(f);

        kill(getppid(), SIGUSR1);      
    }
}

void parent_handler(int sig) {
    if(sig == SIGUSR1) {
        FILE* f = fopen("./januar2021/prenos.dat", "rb");

        for(int i = 0; i < N; i++) {
            int num;
            fread(&num, sizeof(int), 1, f);

            printf("%d. broj u fajlu je: %d\n", i+1, num);
            fflush(stdout);
        }

        fclose(f);

        current++;

        if(current >= iterations) {
            kill(child, SIGKILL);

            exit(0);
        }
    }
}

int main() {
    signal(SIGUSR1, parent_handler);

    child = fork();

    if(child == 0) {
        srand(getppid() ^ time(NULL));

        child = getpid();
        signal(SIGUSR2, child_handler);

        while(1) {
            pause();
        }
    }
    else if(child > 0) {
        while(1) {
            kill(child, SIGUSR2);

            pause();
        }
    }
    else {
        perror("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(1);
    }

    return 0;
}