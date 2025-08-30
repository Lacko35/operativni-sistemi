#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILE_LEN 512

void ProccessDir(char dirpath[], int sortDirection) {
    DIR* dp = opendir(dirpath);

    if(dp == NULL) {
        perror("GRESKA: neuspesno otvaranje direktorijuma!\n");
        exit(EXIT_FAILURE);
    }

    struct dirent* dirp;
    struct stat statubuff;

    char array[100][MAX_FILE_LEN];
    int sizes[100];
    int filesNum = 0;

    while((dirp = readdir(dp)) != NULL) {
        char obj[MAX_FILE_LEN];
        strcpy(obj, dirpath);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        if(stat(obj, &statubuff) == -1) {
            perror("GRESKA: nije moguce prikupiti informacije o datom objektu!\n");
            continue;;
        }

        if(S_ISREG(statubuff.st_mode)) {
            if(filesNum == 0) {
                strcpy(array[filesNum], dirp->d_name);
                sizes[filesNum] = statubuff.st_size;
                filesNum++;
            }
            else {
                strcpy(array[filesNum], dirp->d_name);
                sizes[filesNum] = statubuff.st_size;
                filesNum++;

                for(int i = 0; i < filesNum - 1; i++) {
                    for(int j = i + 1; j < filesNum; j++) {
                        if(sortDirection == 0) {
                            if(sizes[i] < sizes[j]) {
                                char temp[MAX_FILE_LEN];
                                strcpy(temp, array[i]);
                                strcpy(array[i], array[j]);
                                strcpy(array[j], temp);
                            }
                        }
                        else {
                            if(sizes[i] > sizes[j]) {
                                char temp[MAX_FILE_LEN];
                                strcpy(temp, array[i]);
                                strcpy(array[i], array[j]);
                                strcpy(array[j], temp);
                            }
                        }
                    }
                }
                
            }
        }
    }

    for(int i = 0; i < filesNum; i++) {
        printf("%s ", array[i]);
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        perror("GRESKA: Nedovoljan broj argumenata komandne linije!\n");
        exit(EXIT_FAILURE);
    }
    
    ProccessDir(argv[1], atoi(argv[2]));

    return 0;
}