//imamo 2 procesa, roditelja i njegovo dete. Procesi komuniciraju tako da roditelj generise nasumicno
//1024 celih brojeva i upisuje ih u prenos.dat datoteku i nakon toga salje sigusr1 signal da obavesti
//svoje dete o tome. Proces dete, cita iz fajla i na standardnom izlazu prikazuje te brojeve a nakon toga
//salje sigusr2 svom roditelju kako bi ga obavestio o tome. Postupak se ponavlja 4096 puta

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#define FILE_MEMBERS 5
#define ITERATIONS 10
#define SOURCE_FILE "prenos.dat"

pid_t child, parent;
int currentIteration = 0;

void parent_handler(int sig) {
    if(sig == SIGUSR2) {
        currentIteration++;

        if(currentIteration >= ITERATIONS) {
            kill(child, SIGTERM);

            exit(0);
        }

        FILE* f = fopen(SOURCE_FILE, "wb");

        for(int i = 0; i < FILE_MEMBERS; i++) {
            int num = rand() % 100;
            fwrite(&num, sizeof(int), 1, f);
        }

        fclose(f);

        kill(child, SIGUSR1);
    }
}

void child_handler(int sig) {
    if(sig == SIGUSR1) {
        FILE* file = fopen(SOURCE_FILE, "rb");

        for(int i = 0; i < FILE_MEMBERS; i++) {
            int num;
            fread(&num, sizeof(int), 1, file);

            printf("\t%d. broj u ovoj iteraciji je: %d\n", i+1, num);
        }

        printf("\n");

        fflush(stdout);

        fclose(file);

        kill(parent, SIGUSR2);
    }
}

int main() {
    srand(time(NULL));

    parent = getpid();

    child = fork();

    if(child == 0) {
        child = getpid();

        signal(SIGUSR1, child_handler);

        while(1) {
            pause();
        }
    }
    else {
        signal(SIGUSR2, parent_handler);

        FILE* f = fopen(SOURCE_FILE, "wb");

        for(int i = 0; i < FILE_MEMBERS; i++) {
            int num = rand() % 100;
            fwrite(&num, sizeof(int), 1, f);
        }

        fclose(f);

        kill(child, SIGUSR1);

        while(1) {
            pause();
        }
    }

    return 0;
}