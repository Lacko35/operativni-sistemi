#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct message {
    long type;
    int* arr;
};

int main() { 
    int msgQueueID = msgget((key_t)10101, IPC_CREAT | 0666);

    pid_t child = fork();

    if(child == 0) {
        short oddCounter = 0;
        short evenCounter = 0;

        struct message msg;

        msgrcv(msgQueueID, &msg, 40, 1, 0);

        int i = 0;
        while(msg.arr[i] != -1) {
            if(msg.arr[i] % 2 == 0) {
                oddCounter++;
            }
            else {
                evenCounter++;
            }

            printf("Prosledjeni broj: %d ");
        }

        printf("\n");

        return 0;
    }
    else {
        int N = (rand() % 10) + 1;
        struct message msg;
        msg.type = 1;

        int i = 0;
        while(N > 0) {
            msg.arr[i++] = rand() % 100;
            
            N--;
        }

        msg.arr[i] = -1;

        msgsnd(msgQueueID, &msg, sizeof(int) * i; 0);

        wait(NULL);

        msgctl(msgQueueID, IPC_RMID, NULL);
    }

    return 0;
}