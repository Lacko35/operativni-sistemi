#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>

struct message {
    long type;
    int num;
};

int main(int argc, char* argv[]) {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    struct message msg;
    msg.type = 1;

    if(fork() != 0) {
        while(1) {
            int broj = rand() % 128;

            msg.num = broj;
            msgsnd(msg_queue, &msg, sizeof(broj), 0);

            if(broj == 0) {
                break;
            }
        }

        wait(NULL);

        printf("-----KRAJ PROGRAMA-----\n");

        msgctl(msg_queue, IPC_RMID, NULL);
    }
    else {
        int numOfReceiving = 0;
        while(1) {
            msgrcv(msg_queue, &msg, sizeof(int), 1, 0);

            numOfReceiving++;

            if(msg.num == 0) {
                break;
            }

            printf("\tASCII karakter [%c]\n", msg.num);
        }

        printf("Broj prosledjenih brojeva je: [%d]\n", numOfReceiving);

        return 0;
    }
    
    return 0;
}