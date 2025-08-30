#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

void syncronizeDirectories(char path1[], char path2[]) {
    DIR* dp1 = opendir(path1);

    if(dp1 == NULL) {
        printf("Doslo je do greske prilikom otvaranja direktorijuma %s\n", path1);
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp1;
    struct stat statbuff1;

    while((dirp1 = readdir(dp1)) != NULL) {
        char obj1[512];
        strcpy(obj1, path1);
        strcat(obj1, "/");
        strcat(obj1, dirp1->d_name);
    
        if(stat(obj1, &statbuff1) == -1) {
            continue;
        }

        if(S_ISREG(statbuff1.st_mode)) {
            int existFlag = 0;

            DIR* dp2 = opendir(path2);

            if(dp2 == NULL) {
                printf("Doslo je do greske prilikom otvaranja direktorijuma %s\n", path2);
                exit(EXIT_FAILURE);
            }

            struct dirent* dirp2;
            struct stat statbuff2;

            while((dirp2 = readdir(dp2)) != NULL) {
                char obj2[512];
                strcpy(obj2, path2);
                strcat(obj2, "/");
                strcat(obj2, dirp2->d_name);
            
                if(stat(obj2, &statbuff2) == -1) {
                    continue;
                }

                if(S_ISREG(statbuff2.st_mode)) {
                    if(statbuff1.st_size == statbuff2.st_size && strcmp(dirp1->d_name, dirp2->d_name) == 0) {
                        printf("datoteka postoji u oba direktorijuma!\n");
                        fflush(stdout);

                        existFlag = 1;
                    }
                }
                else if(S_ISDIR(statbuff2.st_mode) && strcmp(dirp2->d_name, ".") != 0 && strcmp(dirp2->d_name, "..") != 0) {
                    syncronizeDirectories(path1, obj2);
                }
            }

            closedir(dp2);
            
            if(!existFlag) {
                if(fork() == 0) {
                    execlp("cp", "cp", obj1, path2, NULL);
                }
                else {
                    wait(NULL);
                }
            }
        }
        else {
            continue;
        }
    }

    closedir(dp1);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        perror("Nedovoljan broj argumenata komandne linije!\n");
        exit(EXIT_FAILURE);
    }

    syncronizeDirectories(argv[1], argv[2]);

    return 0;
}