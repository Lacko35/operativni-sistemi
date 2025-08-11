#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define FILE_SIZE 1024

void findSmallest(char* path, char* smallest, int* size, int border1, int border2) {
    DIR* dp;
    if((dp = opendir(path)) == NULL) {
        perror("Doslo je do greske prilikom otvaranja direktorijuma\n");
        exit(EXIT_FAILURE);
    }

    struct stat statbuff;
    struct dirent* dirp;

    while((dirp = readdir(dp)) != NULL) {
        char obj[FILE_SIZE];
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statbuff) == -1) {
            continue;
        }

        if(S_ISREG(statbuff.st_mode)) {
            if(strstr(dirp->d_name, ".c") != NULL) {
                if(statbuff.st_size > border1 && statbuff.st_size < border2) {
                    if(statbuff.st_size < *size) {
                        *size = statbuff.st_size;
                        strcpy(smallest, dirp->d_name);
                    }
                }
            }
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            findSmallest(obj, smallest, size, border1, border2);
        }
        else {
            continue;
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nedovoljan broj argumenata komandne linije!\n");
        exit(EXIT_FAILURE);
    }

    int donjaGranica;
    printf("Unesite donju granicu u bajtovima: ");
    scanf("%d", &donjaGranica);

    int gornjaGranica;
    printf("Unesite gornju granicu u bajtovima: ");
    scanf("%d", &gornjaGranica);

    char smallest[FILE_SIZE];
    int size = 999999999;
    findSmallest(argv[1], smallest, &size, donjaGranica, gornjaGranica);

    printf("Najmanja .c datoteka u zadatom opsegu iz zadatog fajla: %s\n", smallest);

    return 0;
}