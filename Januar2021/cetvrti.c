#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define BOTTOM_BORDER 150
#define TOP_BORDER 750
#define MAX_RECURSIVE_LEVEL 3

int filesCounter = 0;

void FindFiles(char absPath[], int lvl) {
    DIR* dp = opendir(absPath);

    if(dp == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statbuff;

    while((dirp = readdir(dp)) != NULL) {
        char obj[1024];
        strcpy(obj, absPath);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            if(statbuff.st_size > BOTTOM_BORDER && statbuff.st_size < TOP_BORDER) {
                printf("%s\n", dirp->d_name);
                filesCounter++;
            }
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, "..") != 0 && strcmp(dirp->d_name, ".") != 0) {
            if(lvl < MAX_RECURSIVE_LEVEL) {
                FindFiles(obj, lvl++);
            }
        }
        else {
            continue;
        }
    }

    if(!filesCounter) {
        printf("Nije pronadjen fajl koji ispunjava zadati uslov!\n");
    }

    closedir(dp);

    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nedovoljan broj ulaznih argumenata programa!\n");
        exit(EXIT_FAILURE);
    }

    FindFiles(argv[1], 0);

    return 0;
}