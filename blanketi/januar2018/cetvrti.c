#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define KILOBYTE 1024
#define ABSOLUTE_PATH_LENGTH 256

void findFiles(char absolutePath[]) {
    DIR* dp = opendir(absolutePath);

    if(dp == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statbuff;

    while((dirp = readdir(dp)) != NULL) {
        char obj[512];
        strcpy(obj, absolutePath);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode) && statbuff.st_size > 20 * KILOBYTE && strlen(absolutePath) < ABSOLUTE_PATH_LENGTH) {
            printf("%s\n", dirp->d_name);
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(".", dirp->d_name) != 0 && strcmp("..", dirp->d_name) != 0) {
            findFiles(obj);
        }
        else {
            continue;
        }
    }

    closedir(dp);
}

int main() {
    findFiles("/");

    return 0;
}