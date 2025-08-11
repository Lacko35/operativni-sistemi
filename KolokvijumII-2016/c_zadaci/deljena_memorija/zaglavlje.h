#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 10101
#define SEM1_KEY 10102
#define SEM2_KEY 10103
#define SEM3_KEY 10104
#define LINE_LEN 86

union semun {
    int val;
    short* arr;
    struct semid_ds* buff;
};

struct shared_memory {
    int kraj;
    int broj;
    char text[LINE_LEN];
};