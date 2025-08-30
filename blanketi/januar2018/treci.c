#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>

struct message {
    long type;
    char num;
};

int main() {
    int msgQueueID = msgget((key_t)10101, IPC_CREAT | 0666);

    if(msgQueueID == -1) {
        perror("Neuspesno kreiranje reda poruka!\n");
        return -1;
    }

    pid_t child = fork();

    if(child == 0) {
        for(int i = 0; i < 10; i++) {
            struct message msg1;

            msgrcv(msgQueueID, &msg1, sizeof(char), 1, 0);

            struct message msg2;
            msg2.num = 2 * msg1.num;
            msg2.type = 2;

            msgsnd(msgQueueID, &msg2, sizeof(char), 0);
        }

        return 0;
    }
    else if(child > 0) {
        for(int i = 0; i < 10; i++) {
            struct message msg1;
            msg1.type = 1;

            printf("Unesite broj: ");
            fflush(stdout);
            scanf("%c", &msg1.num);

            printf("\n----------------------------------------------------\n");
            fflush(stdout);

            printf("Poslata poruka: %c\n", msg1.num);
            fflush(stdout);

            msgsnd(msgQueueID, &msg1, sizeof(char), 0);

            struct message msg2;
            msgrcv(msgQueueID, &msg2, sizeof(char), 2, 0);
        
            printf("Primljena poruka je: %c\n", msg2.num);
            fflush(stdout);
            
            printf("----------------------------------------------------\n");
            fflush(stdout);
        }
    }
    else {
        perror("Neuspesno kreiranje procesa!\n");
        return -1;
    }

    return 0;
}