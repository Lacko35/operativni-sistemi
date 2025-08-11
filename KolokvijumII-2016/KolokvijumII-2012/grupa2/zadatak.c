#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

struct message {
    long type;
    char text[100];
};

int main() {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    struct message msg;

    if(fork() == 0) {
        msgrcv(msg_queue, &msg, 100, 1, 0);

        msg.type = 2;

        if(strcmp(msg.text, "pera") == 0) {
            strcpy(msg.text, "peric");
        }
        else {
            strcpy(msg.text, "USER NOT FOUND");
        }

        msgsnd(msg_queue, &msg, strlen(msg.text) + 1, 0);
    }
    else {
        char username[20];
        printf("Unesite username: ");
        fflush(stdout);
        fgets(username, sizeof(username), stdin);

        msg.type = 1;
        strcpy(msg.text, username);

        msgsnd(msg_queue, &msg, strlen(username) + 1, 0);

        msgrcv(msg_queue, &msg, 100, 2, 0);

        printf("username: [%s] password: [%s]", username, msg.text);

        wait(NULL);

        msgctl(msg_queue, IPC_RMID, NULL);
    }

    return 0;
}