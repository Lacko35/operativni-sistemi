#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 10

int main() {
    int fd1[2];
    int fd2[2];

    if(pipe(fd1) < 0) {
        perror("Doslo je do greske prilikom kreiranja datavoda 'fd1'\n");
        exit(EXIT_FAILURE);
    }

    if(pipe(fd2) < 0) {
        perror("Doslo je do greske prilikom kreiranja datavoda 'fd2'\n");
        exit(EXIT_FAILURE);
    }

    pid_t proccess = fork();

    if(proccess > 0) {
        close(fd1[0]);
        close(fd2[1]);

        for(int i = 0; i < N; i++) {
            int broj = (rand() % 100) + 200;

            write(fd1[1], &broj, sizeof(int));
            read(fd2[0], &broj, sizeof(int));

            if(broj != 0) {
                printf("Modifikovani broj koji je proces dete vratio roditelju: [%d]\n", broj);
            }
            else {
                printf("Nije doslo do modifikacije\n");
            }
        }

        wait(NULL);

        close(fd1[1]);
        close(fd2[0]);        
    }
    else if(proccess == 0) {
        close(fd1[1]);
        close(fd2[0]);

        for(int i = 0; i < N; i++) {

            int broj;
            read(fd1[0], &broj, sizeof(int));
            printf("Broj koji je proces roditelj poslao detetu: [%d]\n", broj);

            if(broj % 3 == 0) {
                broj += 25;
                write(fd2[1], &broj, sizeof(int));
            }
            else {
                broj = 0;
                write(fd2[1], &broj, sizeof(int));
            }
        }

        close(fd1[0]);
        close(fd2[1]);
    }
    else {
        perror("Doslo je do greske prilikom kreiranja procesa\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}