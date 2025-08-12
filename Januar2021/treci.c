#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>

#define FILE_NAME "prenos.dat"
#define FILE_SIZE 1024
#define ITERATIONS 4096

pid_t pid1, pid2;
int iterationsCounter = 0;

void generator() {
    FILE* f = fopen(FILE_NAME, "wb");

    if(iterationsCounter >= ITERATIONS) {
        kill(pid2, SIGTERM);
        exit(0);
    }

    int numbers[FILE_SIZE];
    for(int i = 0; i < FILE_SIZE; i++) {
        numbers[i] = rand() % 1000;
    }

    fwrite(numbers, sizeof(int), FILE_SIZE, f);
    fclose(f);

    iterationsCounter++;
    kill(pid2, SIGUSR1);
}

void potrosac() {
    FILE* f = fopen(FILE_NAME, "rb");

    int numbers[FILE_SIZE];
    fread(numbers, sizeof(int), FILE_SIZE, f);
    fclose(f);

    for(int i = 0; i < FILE_SIZE; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    kill(pid1, SIGUSR2);
}

int main() {
    srand(time(NULL));

    pid1 = getpid();
    pid2 = fork();

    if(pid2 == 0) {
        pid2 = getpid();

        signal(SIGUSR1, potrosac);

        while(1) {
            pause();
        }
    }
    else {
        signal(SIGUSR2, generator);

        generator();

        while(1) {
            pause();
        }
    }

    return 0;
}