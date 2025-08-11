#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

struct message {
    long type;
    int number;
};

int main() {
    int queue = msgget(10100, IPC_CREAT | 0666);

    struct message msg;

    if(fork() == 0) {
        while(1) {
            msgrcv(queue, &msg, sizeof(msg.number), 0, 0);

            if(msg.type == 2) {
                break;
            }

            printf("Prosledjeni broj je: [%d]\n", msg.number);
        }
    }
    else {
        int suma = 0;
        while(suma <= 1000) {
            int broj;

            printf("Unesite broj: ");
            scanf("%d", &broj);

            suma += broj;

            msg.type = 1;
            msg.number = broj;
            msgsnd(queue, &msg, sizeof(msg.number), 0);
        }

        msg.type = 2;
        msg.number = 0000;
        msgsnd(queue, &msg, sizeof(msg.number), 0);

        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }

    return 0;
}