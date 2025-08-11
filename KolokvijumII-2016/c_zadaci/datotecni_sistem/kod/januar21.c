#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void search(char* path, int N, int M) {
    DIR* dp = opendir(path);
    
    struct dirent* dirp;
    struct stat statbuff;

    while((dirp = readdir(dp)) != NULL) {
        char* obj;
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, dirp->d_name);

        stat(obj, &statbuff);

        if(S_ISREG(statbuff.st_mode)) {
            if(statbuff.st_size > N && strlen(obj) > M) {
                printf("Apsolutna putanja je: [%s]\nVelicina datoteke u bajtovima: [%ld]B", obj, statbuff.st_size);
            }
        }
        else if(S_ISDIR(statbuff.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            search(obj, N, M);
        }
        else {
            continue;
        }
    }
    
    closedir(dp);
}

int main(int argc, char* argv[]) {
    search(argv[1], atoi(argv[2]), atoi(argv[3]));

    return 0;
}