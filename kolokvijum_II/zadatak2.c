#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define EVEN_SHM_KEY 15112
#define ODD_SHM_KEY 15678
#define MEMORY_SIZE 400

int main(int argc, char* argv[]) {
    int even_shm_id = shmget(EVEN_SHM_KEY, MEMORY_SIZE, IPC_CREAT | 0666);
    
    if(even_shm_id == -1) {
        perror("Error: shmget() failed.\n");
        exit(EXIT_FAILURE);
    }
    
    int odd_shm_id = shmget(ODD_SHM_KEY, MEMORY_SIZE, IPC_CREAT | 0666);
    
    if(odd_shm_id == -1) {
        perror("Error: shmget() failed.\n");
        exit(EXIT_FAILURE);
    }

    int* even_shm = (int*) shmat(even_shm_id, NULL, 0);

    if(even_shm == NULL) {
        perror("Error: shmat() failed.\n");
        exit(EXIT_FAILURE);
    }

    int* odd_shm = (int*) shmat(odd_shm_id, NULL, 0);

    if(odd_shm == NULL) {
        perror("Error: shmat() failed.\n");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen("brojevi.dat", "r");

    if(file == NULL) {
        perror("Error: fopen() failed.\n");
    }

    int broj;
    int evenNum = 0;
    int oddNum = 0;

    while(fread(&broj, sizeof(int), 1, file) == 1) {
        if(broj % 2 == 0) {
            evenNum++;
            even_shm[evenNum] = broj;
        }
        else {
            oddNum++;
            odd_shm[oddNum] = broj;
        }
    }

    fclose(file);

    even_shm[0] = evenNum;
    odd_shm[0] = oddNum;

    shmdt(even_shm);
    shmdt(odd_shm);

    shmctl(even_shm_id, IPC_RMID, 0);
    shmctl(odd_shm_id, IPC_RMID, 0);

    return 0;
}