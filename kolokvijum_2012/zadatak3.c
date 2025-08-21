#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define N 4

int combination[N] = {1, 1, 0, 1};
int count = 0;

void catch_C() {
    signal(SIGINT, catch_C);

    if(combination[count] == 1) {
        count++;

        if(count == N) {
            printf(" Validna kombinacija, izlazimo iz programa!\n");
            exit(0);
        }
    }
    else {
        printf(" Los redosled tastera, restartuje se kombinacija\n");
        count = 0;
    }
}

void catch_Z() {
    signal(SIGTSTP, catch_Z);

    if(combination[count] == 0) {
        count++;

        if(count == N) {
            printf(" Validna kombinacija, izlazimo iz programa!\n");
            exit(0);
        }
    }
    else {
        printf(" Los redosled tastera, restartuje se kombinacija\n");
        count = 0;
    }
}

int main() {
    signal(SIGINT, catch_C);
    signal(SIGTSTP, catch_Z);

    while(1) {
        pause();
    }

    return 0;
}