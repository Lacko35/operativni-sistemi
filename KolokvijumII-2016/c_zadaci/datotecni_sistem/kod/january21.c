#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void findFiles(char* path, int N, int M) {
    DIR* dp;
    if((dp = opendir(path)) == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        exit(EXIT_FAILURE);
    }

    struct stat statbuff;
    struct dirent* dirp;

    while((dirp = readdir(dp)) != NULL) {
        char obj[512];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            perror("Greska prilikom prikupljanja informacija o objektu\n");
            continue;
        }

        if(S_ISREG(statbuff.st_mode) && statbuff.st_size > N && strlen(obj) > M) {
            printf("\t%s\n", obj);
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(".", dirp->d_name) != 0 && strcmp(".", dirp->d_name) != 0) {
            findFiles(obj, N, M);
        }
        else {
            continue;
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 4) {
        perror("Nedovoljan broj argumenata komandne linije!\n");
        exit(EXIT_FAILURE);
    }

    findFiles(argv[1], atoi(argv[2]), atoi(argv[3]));
    return 0;
}