#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>

void nadjiNajvecuRegularnu(char* putanja, int nivo, int* velicinaNajvece, char* najveca) {
    DIR* dp;
    if((dp = opendir(putanja)) == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma -> izlazimo iz programa\n");
        exit(EXIT_FAILURE);
    }

    struct stat statbuff;
    struct dirent* dirp;

    while((dirp = readdir(dp)) != NULL) {
        char obj[512];
        strcpy(obj, putanja);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            perror("Greska prilikom prikupljanja informacija o datom clanu direktorijuma\n");
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            if(statbuff.st_size > *velicinaNajvece) {
                *velicinaNajvece = statbuff.st_size;
                strcpy(najveca, obj);
            }
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(".", dirp->d_name) != 0 
            && strcmp("..", dirp->d_name) != 0 && nivo < 5) 
        {
            nadjiNajvecuRegularnu(obj, nivo + 1, velicinaNajvece, najveca);
        }
        else {
            continue;
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nedovoljan broj argumenata komandne linije->izlazimo iz programa");
        exit(EXIT_FAILURE);
    }
    
    int velicinaNajvece = 0;
    char najveca[512] = "";
    nadjiNajvecuRegularnu(argv[1], 0, &velicinaNajvece, najveca);

    printf("Najveca regularna datoteka je: | %s |\n", najveca);

    return 0;
}