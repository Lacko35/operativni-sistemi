#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int int_counter = 0;
int alrm_counter = 0;
int prekid = 0;

void catch_int(int sigNum) {
    signal(SIGINT, catch_int);

    int_counter++;
    printf("\n[CTRL+Z] pritisnuto %d. puta\n", int_counter);
    fflush(stdout);
}

void catch_alrm(int sigNum) {
    signal(SIGALRM, catch_alrm);

    alrm_counter++;

    alarm(10);

    if(int_counter == 2) {
        printf("\nDoslo je do prekida programa!\n");
        exit(1);
    }
    
    if(alrm_counter == 3) {
        prekid = 1;
    }
}

int main() {
    signal(SIGALRM, catch_alrm);
    signal(SIGINT, catch_int);

    alarm(10);

    int odd_counter = 0;

    while(1) {
        int broj;
        printf("Unesite neki broj: ");
        scanf("%d", &broj);

        if(broj % 2 == 0) {
            odd_counter++;
        }

        if(prekid) {
            break;
        }
    }

    printf("Broj parnih unetih brojeva je: %d\n", odd_counter);
}