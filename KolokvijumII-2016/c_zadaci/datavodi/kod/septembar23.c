#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define KOLICINA 200

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("Nedovoljan broj argumenata komandne linije!\n");
        exit(EXIT_FAILURE);
    }

    int fd[2];

    if(pipe(fd) < 0) {
        printf("Doslo je do greske prilikom kreiranja datavoda [fd]!\n");
        exit(EXIT_FAILURE);
    }

    pid_t proces = fork();

    if(proces > 0) {
        close(fd[0]);

        FILE* f = fopen(argv[1], "rb");

        if(!f) {
            printf("Doslo je do greske prilikom otvaranja fajla [%s]!\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        char podaci[KOLICINA];

        while(!feof(f)) {
            fread(podaci, sizeof(char), KOLICINA, f);

            write(fd[1], podaci, sizeof(podaci));
        }

        fclose(f);

        wait(NULL);

        close(fd[1]);
    }
    else if(proces == 0) {
        close(fd[1]);

        FILE* f = fopen(argv[2], "wb");

        if(!f) {
            printf("Doslo je do greske prilikom otvaranja fajla [%s]!\n", argv[2]);
            exit(EXIT_FAILURE);
        }        

        char datas[KOLICINA];

        while((read(fd[1], datas, KOLICINA)) == KOLICINA) {
            for(int i = 0; i < KOLICINA; i++) {
                int br = (rand() % 255) + 1;
                datas[i] ^= br;

                fwrite(&br, sizeof(int), 1, f);
            }
            fwrite(datas, sizeof(char), KOLICINA, f);
        }
        
        fclose(f);

        close(fd[0]);
    }
    else {
        printf("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}