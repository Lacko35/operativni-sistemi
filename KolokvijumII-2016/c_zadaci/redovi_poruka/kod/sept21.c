#include "zaglavlja.h"

#define N 10

int main(int argc, char* argv[]) {
    int queue = msgget(MSG_QUEUE_ID, IPC_CREAT | 0666);

    struct message msg;

    if(fork() == 0) {
        for(int i = 0; i < N; i++) {
            msgrcv(queue, &msg, sizeof(int), 1, 0);

            int broj = atoi(msg.text);

            if(broj % 2 == 0) {
                printf("PARAN BROJ\n");
            }
            else {
                printf("[%d]", broj);
            }
        }
    }
    else {
        for(int i = 0; i < N; i++) {
            int broj = rand() % 100;

            msg.type = 1;
            sprintf(msg.text, "%d", broj);

            msgsnd(queue, &msg, sizeof(int), 0);
        }

        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }

    return 0;
}