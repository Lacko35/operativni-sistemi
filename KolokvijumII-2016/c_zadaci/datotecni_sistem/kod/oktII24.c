#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

void searchFolder(char* abs_path) {
    DIR* dp = opendir(abs_path);

    if(dp == NULL) {
        perror("Neusposno otvaranje datoteke [%s]\n", abs_path);
        exit(EXIT_FAILURE);
    }

    int executedNum = 0;

    struct stat statbuff;

    struct dirent* dirp;
    while((dirp = opendir(dp)) != NULL) {
        char obj[80];
        strcpy(obj, abs_path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        stat(obj, &statbuff);

        if(S_ISREG(statbuff.st_mode)) {
            if(strstr(obj, ".sh")) {
                if(fork() == 0) {
                    execlp(obj, obj, NULL);
                }
                else {
                    wait(NULL);
                    executedNum++;
                }
            }
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            searchFolder(obj);
        }
        else {
            continue;
        }
    }

    closedir(abs_path);

    printf("Broj izvrsenih bash skripti je: [%d]\n", executedNum);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nevalidan broj argumenata komandne linije!");
        exit(EXIT_FAILURE);
    }

    searchFolder(argv[1]);

    return 0;
}