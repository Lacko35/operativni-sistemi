#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/ipc.h>

#define LINE_LENGTH 87
#define QUEUE_KEY 10101
#define MSG_TYPE1 1
#define MSG_TYPE2 2

struct message {
    long type;
    char text[LINE_LENGTH];
    int lineNum;
    int upperCaseNum;
};

int main(int argc, char* argv[]) {
    int queue = msgget(QUEUE_KEY, IPC_CREAT | 06666);

    struct message msg;

    if(fork() != 0) {
        FILE* f = fopen(argv[1], "r");
        char line[LINE_LENGTH];

        while(!feof(f)) {
            fgets(line, LINE_LENGTH, f);

            msg.type = MSG_TYPE1;
            strcpy(msg.text, line);
            msgsnd(queue, &msg, strlen(line) + 1, 0);

            struct message new_msg;
            msgrcv(queue, &new_msg, 2 * sizeof(int), MSG_TYPE2, 0);
            printf("Redni broj linije je [%d] a broj velikih slova u toj liniji je [%d]\n", new_msg.lineNum, new_msg.upperCaseNum);
        }

        fclose(f);

        struct message stop_msg;
        stop_msg.type = 10;
        msgsnd(queue, &stop_msg, 0, 0);

        printf("----------------KRAJ!----------------\n");

        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }
    else {
        char line[LINE_LENGTH];
        int rbr = 0;

        while(1) {
            struct message stop_msg;
            if(msgrcv(queue, &stop_msg, 10, 0, IPC_NOWAIT) == 0) {
                break;
            }

            msgrcv(queue, &msg, LINE_LENGTH, MSG_TYPE1, 0);
            strcpy(msg.text, line);
            rbr++;

            struct message send_msg;
            send_msg.type = MSG_TYPE2;
            int brVelikih = 0;

            for(int i = 0; i < strlen(line); i++) {
                if(line[i] >= 'A' && line[i] <= 'Z') {
                    brVelikih++;
                }
            }

            send_msg.lineNum = rbr;
            send_msg.upperCaseNum = brVelikih;
            msgsnd(queue, &send_msg, 2 * sizeof(int), 0);
        }
    }

    return 0;
}