#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int fd[2];

    if(pipe(fd) < 0) {
        printf("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[2]);

    int process = fork();

    if(process > 0) {
        close(fd[0]);

        FILE* f = fopen(argv[1], "r");
        char line[N];

        while(!feof(f)) {
            fgets(line, N, f);

            write(fd[1], line, strlen(line) + 1);
        }

        fclose(f);

        wait(NULL);

        close(fd[1]);
    }
    else if(process == 0) {
        close(fd[1]);

        FILE* f = fopen(argv[3], "w");
        char line[N];

        while(1) {
            ssize_t procitano = read(fd[0], line, N);

            if(procitano == 0) {
                break;
            }

            fputs(line, f);
        }

        fclose(f);

        close(fd[0]);
    }
    else {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}