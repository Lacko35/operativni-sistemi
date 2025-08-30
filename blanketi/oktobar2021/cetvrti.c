#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define NUMBER_OF_FILES 5
#define PATH_LENGTH 1024

char files[NUMBER_OF_FILES][PATH_LENGTH];
int sizes[NUMBER_OF_FILES];

void deleteFiles(char path[]) {
    DIR* dp = opendir(path);

    if(dp == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statbuff;

    int current = 0;

    while((dirp = readdir(dp)) != NULL) {
        char obj[PATH_LENGTH];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);
    
        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            strcpy(files[current], obj);
            sizes[current] = (int)statbuff.st_size;

            current++;
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            deleteFiles(obj);
        }
        else {
            continue;
        }
    }

    for(int i = 0; i < current - 1; i++) {
        for(int j = i + 1; j < current; j++) {
            if(sizes[i] < sizes[j]) {
                char tmp[PATH_LENGTH];
                strcpy(tmp, files[i]);
                strcpy(files[i], files[j]);
                strcpy(files[j], tmp);
            }
        }
    }

    int i = 0;
    while(i < NUMBER_OF_FILES) {
        if(fork() == 0) {
            execlp("rm", "rm", files[i], NULL);
        }
        else {
            wait(NULL);
        }

        i++;
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nevalidan broj argumenata komandne linije!\n");
        return 1;
    }

    deleteFiles(argv[1]);

    return 0;
}