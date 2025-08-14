#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_LENGTH 512
#define FILE_LENGTH 80
#define FILE_NUM 100
#define KILOBYTE 1024

void findFiles(char path[]) {
    DIR* dp;
    if((dp = opendir(path)) == NULL) {
        perror("Doslo je do greske prilikom otvaranja fajla!\n");
        exit(1);
    }

    struct dirent* dirp;
    struct stat statbuff;
    
    char files[FILE_NUM][FILE_LENGTH];
    int filesSizes[FILE_NUM];
    int currentFiles = 0;

    while((dirp = readdir(dp)) != NULL) {
        char obj[PATH_LENGTH];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(S_ISREG(statbuff.st_mode) && statbuff.st_size > 15 * KILOBYTE) {
            strcpy(files[currentFiles], dirp->d_name);
            filesSizes[currentFiles] = statbuff.st_mode;
            currentFiles++;
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..")) {
            findFiles(obj);
        }
    }

    for(int i = 0; i < currentFiles - 1; i++) {
        for(int j = i + 1; j < currentFiles; j++) {
            if(filesSizes[i] > filesSizes[j]) {
                char tmp[FILE_LENGTH];
                strcpy(tmp, files[i]);
                strcpy(files[i], files[j]);
                strcpy(files[j], tmp);
            }
        }
    }

    for(int i = 0; i < currentFiles; i++) {
        printf("%s\n", files[i]);
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nevalidan broj argumenata komandne linije!\n");
        exit(1);
    }

    findFiles(argv[1]);

    return 0;
}