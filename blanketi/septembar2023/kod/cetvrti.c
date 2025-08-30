#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define LINE_LENGTH 103
void searchDirectory(char path[], int n, int m) {
    DIR* dp = opendir(path);

    if(dp == -1) {
        printf("Doslo je do greske prilikom otvaranja direktorijuma %s\n", path);
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statbuff;

    while((dirp = readdir(dp)) != NULL) {
        char obj[1024];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);
    
        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode) && n > 0) {
            int pom = m;

            FILE* f = fopen(obj, "r");
            
            while(pom > 0) {
                char buffer[LINE_LENGTH];

                fgets(buffer, LINE_LENGTH, f);
                
                if(pom != 1) {
                    buffer[strcspn(buffer, "\n")] = '\0';
                }

                printf("[%d]. linija iz [%d]. datoteke je: %s\n", pom, n, buffer);
                fflush(stdout);

                pom--;
            }

            fclose(f);

            n--;
        }
        else {
            continue;
        }
    }

    closedir(dp);
}

int main(int argc. char* argv[]) {
    if(argc != 4) {
        perror("Nevalidan broj parametara\n");
        exit(1);
    }

    searchDirectory(argv[1], atoi(argv[2]), atoi(argv[3]));

    return 0;
}