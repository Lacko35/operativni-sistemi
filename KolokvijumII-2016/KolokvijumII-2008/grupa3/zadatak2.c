#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/stat.h>

void findSmallest(char* path, char* smallest, int* size) {
    DIR* dp;

    if((dp = opendir(path)) == NULL) {
        perror("Doslo je do greske prilikom otvaranja datoteke, putanja nije ispravna!");
        exit(1);
    }

    struct stat statbuff;

    struct dirent* dirp;
    while((dirp = readdir(dp)) != NULL) {
        char obj[1024];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            printf("Doslo je do greske prilikom prikupljanja informacija o ovom objektu");
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            if(strstr(dirp->d_name , ".txt") != NULL) {
                if(statbuff.st_size > *size) {
                    *size = statbuff.st_size;
                    strcpy(smallest, obj);
                }
            }
        }
        else {
            continue;
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Invalid number of command-line arguments!");
        exit(1);
    }

    char smallest[1024];
    strcpy(smallest, " ");
    int* size = 0;

    findSmallest(argv[1], smallest, size);

    printf("Najmanja .txt datoteka u okviru direktorijuma je: %s", smallest);
}