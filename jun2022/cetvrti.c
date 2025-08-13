#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define KILOBYTE 1024
#define MAX_DEPTH 3

void removeFiles(char path[], int depth) {
    DIR* dp = opendir(path);

    if(dp == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statbuff;

    while((dirp = readdir(dp)) != NULL) {
        char obj[512];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            if(strstr(dirp->d_name, "log") != NULL && statbuff.st_size > 10 * KILOBYTE) {
                if(fork() == 0) {
                    execlp("rm", "rm", obj, NULL);
                }
                else {
                    wait(NULL);
                }
            }
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            if(depth <= MAX_DEPTH) {
                removeFiles(obj, depth++);
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
        perror("Broj argumenata komandne linije nije zadovoljavajuc!\n");
        exit(EXIT_FAILURE);
    }

    removeFiles(argv[1], 0);

    return 0;
}