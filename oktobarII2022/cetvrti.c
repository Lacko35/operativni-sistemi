#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void findFile(char path[], char file[]) {
    DIR* dp = opendir(path);

    if(dp == NULL) {
        perror("Doslo je do greske prilikom otvaranja ");
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statbuff;

    int find = 0;

    while((dirp = readdir(dp)) != NULL && !find) {
        char obj[512];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            if(strcmp(dirp->d_name, file) == 0) {
                printf("Pronadjen fajl!\nnjegova lokacija: %s\nnjegova velicina: %ld\n", obj, statbuff.st_size);
                find = 0;
            }
        }
        
        if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            findFile(obj, file);
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        perror("Ne validan broj komandne linije!\n");
        exit(EXIT_FAILURE);
    }
    
    findFile(argv[1], argv[2]);
    
    return 0;
}