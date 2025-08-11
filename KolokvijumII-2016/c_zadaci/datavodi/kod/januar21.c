#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_OF_ITERATIONS 5

int main() {
    int fd1[2];
    int fd2[2];

    if(pipe(fd1) < 0) {
        printf("Doslo je do greske prilikom kreiranja datavoda [fd1]!\n");
        exit(EXIT_FAILURE);
    }

    if(pipe(fd2) < 0) {
        printf("Doslo je do greske prilikom kreiranja datavoda [fd2]!\n");
        exit(EXIT_FAILURE);
    }

    int process = fork();

    if(process > 0) {
        close(fd1[0]);
        close(fd2[1]);

        int brojac = 0;

        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            int broj = rand() % 100;
            write(fd1[1], &broj, sizeof(broj));

            int procitani_broj;
            read(fd2[0], &procitani_broj, sizeof(procitani_broj));

            if(procitani_broj != 0) {
                printf("Procitani broj je; [%d]\n", procitani_broj);
                brojac++;
            }
            else {
                printf("Broj nije bio paran!\n");
            }
        }

        printf("Broj prenetih parnih brojeva je: %d", brojac);

        wait(NULL);

        close(fd1[1]);
        close(fd2[0]);
    }
    else if(process == 0) {
        close(fd1[1]);
        close(fd2[0]);

        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            int broj;
            read(fd1[0], &broj, sizeof(broj));

            if(broj % 2 == 0) {
                write(fd2[1], &broj, sizeof(broj));
            }
            else {
                broj = 0;
                write(fd2[1], &broj, sizeof(broj));
            }
        }

        close(fd1[0]);
        close(fd2[1]);
    }
    else {
        printf("Doslo je do greske prilikom kreiranja novog procesa sa ID-em: [%d]!\n", (int)process);
        exit(EXIT_FAILURE);
    }

    return 0;
}