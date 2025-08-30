#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define LINE_LENGTH 100

struct parentMessage {
    long type;
    char text[LINE_LENGTH];
};

struct childMessage {
    long type;
    int upperCaseLetterNum;
    int rowNum;
};

int main(int argc, char* argv[]) {
    int msgQueueID = msgget((key_t)10101, IPC_CREAT | 0666);
    
    if(msgQueueID == -1) {
        perror("Doslo je do greske prilikom kreiranja redom poruka!\n");
        exit(EXIT_FAILURE);
    }

    pid_t procc = fork();

    if(procc == 0) {
        int row = 0;

        while(1) {
            struct parentMessage pMsg;

            msgrcv(msgQueueID, &pMsg, LINE_LENGTH, 1, 0);

            if(strcmp(pMsg.text, "KRAJ") == 0) {
                break;
            }

            row++;
            int upperCase = 0;

            struct childMessage cMsg;
            cMsg.type = 2;
            cMsg.rowNum = row;

            for(int i = 0; i < LINE_LENGTH; i++) {
                if(pMsg.text[i] >= 'A' && pMsg.text[i] <= 'Z') {
                    upperCase++;
                }
            }
            cMsg.upperCaseLetterNum = upperCase;

            msgsnd(msgQueueID, &cMsg, sizeof(int) * 2, 0);
        }
    }
    else if(procc > 0) {
        FILE* f = fopen(argv[1], "r");

        char buffer[LINE_LENGTH];

        while(fgets(buffer, 100, f) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';

            struct parentMessage pMsg;
            strcpy(pMsg.text, buffer);
            pMsg.type = 1;

            msgsnd(msgQueueID, &pMsg, strlen(buffer) + 1, 0);

            struct childMessage cMsg;
            msgrcv(msgQueueID, &cMsg, sizeof(int) * 2, 2, 0);

            printf("U [%d.] redu fajla imamo [%d] velikih slova\n", cMsg.rowNum, cMsg.upperCaseLetterNum);
            fflush(stdout);
        }

        fclose(f);

        struct parentMessage endMsg;
        strcpy(endMsg.text, "KRAJ");
        endMsg.type = 1;

        msgsnd(msgQueueID, &endMsg, strlen(endMsg.text) + 1, 0);

        wait(NULL);

        msgctl(msgQueueID, IPC_RMID, NULL);
    }
    else {

    }

    return 0;
}