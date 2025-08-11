#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define ROW_LENGTH 87
#define QUEUE_KEY 10101
#define STOP_MSG_LEN 3

struct message {
    long type;
    char text[ROW_LENGTH];
};

int queue = msgget(QUEUE_KEY, IPC_CREAT | 0666);
struct message msg;

void writeIntoFile(int msg_type) {
    FILE* f = fopen("datoteke/sredjeno.txt", "w");

    while(1) {
        struct message stop_msg;
        if(msgrcv(queue, &stop_msg, STOP_MSG_LEN, 10, IPC_NOWAIT) == STOP_MSG_LEN) {
            break;
        }

        msgrcv(queue, &msg, ROW_LENGTH, msg_type, IPC_NOWAIT);
        fputs(msg.text, f);
    }

    fclose(f);
}

int main() {
    if(queue == -1) {
        printf("Doslo je greske prilikom kreiranja reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    pid_t proces1 = fork();
    pid_t proces2 = fork();

    if(proces1 > 0 && proces2 > 0) {
        int rowNum = 0;
        FILE* f = fopen("datoteke/poruka.txt", "r");

        if(!f) {
            printf("Doslo je do greske prilikom otvaranja fajla: [datoteke/poruka.txt]!\n");
            exit(EXIT_FAILURE);
        }

        while(!feof(f)) {
            fgets(msg.text, ROW_LENGTH, f);
            rowNum++;

            if(rowNum % 2 == 0) {
                msg.type = 1;
            }
            else {
                msg.type = 2;
            }

            msgsnd(queue, &msg, strlen(msg.text) + 1, 0);
        }

        fclose(f);

        struct message stop_msg;
        stop_msg.type = 10;
        strcpy(stop_msg.text, "da");
        msgsnd(queue, &stop_msg, STOP_MSG_LEN, 0);
        msgsnd(queue, &stop_msg, STOP_MSG_LEN, 0);

        wait(NULL);
        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }
    else if(proces1 == 0) {
        writeIntoFile(1);
    }
    else if(proces2 == 0) {
        writeIntoFile(2);
    }
    else {
        printf("Doslo je do greske prilikom kreiranja nekog od procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}