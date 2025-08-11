#include "zaglavlja.h"

int main() {
    int queue = msgget(MSG_QUEUE_ID, IPC_RMID | 0666);

    struct message msg;

    int proc1 = fork();
    if(proc1) {
        while(1) {
            if(msgrcv(queue, &msg, MSG_TEXT_LENGTH, 1, IPC_NOWAIT) == -1) {
                continue;
            }

            if(strcmp(msg.text, "KRAJ") == 0) {
                break;
            }

            printf("Primljena poruka je [%s] a njena duzina je [%d]\n", msg.text, strlen(msg.text));
        }
    }

    int proc2 = fork();
    if(proc2 == 0) {
        while(1) {
            if(msgrcv(queue, &msg, MSG_TEXT_LENGTH, 2, IPC_NOWAIT) == -1) {
                continue;
            }

            if(strcmp(msg.text, "KRAJ") == 0) {
                break;
            }

            for(int i = 0; i < strlen(msg.text); i++) {
                msg.text[i] = toupper(msg.text[i]);
            }

            printf("Primljena poruka je [%s]\n", msg.text);
        }
    }

    char str[MSG_TEXT_LENGTH];
    int type;

    while(1) {
        printf("Unesite broj: ");
        scanf("%d", &type);

        printf("Unesite rec: ");
        scanf("%s", str);

        if(type < 1 || type > 2) {
            strcpy(msg.text, "KRAJ");

            msg.type = 1;
            msgsnd(queue, &msg, strlen(msg.text) + 1, 0);

            msg.type = 2;
            msgsnd(queue, &msg, strlen(msg.text) + 1, 0);

            break;
        }

        msg.type = type;
        strcpy(msg.text, str);

        msgsnd(queue, &msg, strlen(str) + 1, 0);
    }

    wait(NULL);
    wait(NULL);

    msgctl(queue, IPC_RMID, NULL);

    return 0;
}