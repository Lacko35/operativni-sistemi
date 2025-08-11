#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MSG_LEN 1000

struct message {
    long mtype;
    char mtext[MSG_LEN];
};

int main(int argc, char* argv[]) {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    if(msg_queue == -1) {
        perror("msgget() failed");
        exit(1);
    }

    struct message msg;
    
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork() failed");
        exit(1);
    }
    else if(pid == 0) {
        FILE* f = fopen("izlaz.txt", "w");
        
        while(1) {
            msgrcv(msg_queue, &msg, MSG_LEN, 1, 0);

            if(strcmp(msg.mtext, "KRAJ") == 0) {
                break;
            }

            fputs(msg.mtext, f);
        }

        fclose(f);
        exit(0);
    }
    else {
        strcpy(msg.mtext, "");

        do
        {
            printf("Unesite neku rec: ");
            scanf("%s", msg.mtext);

            msg.mtype = 1;
            msgsnd(msg_queue, &msg, strlen(msg.mtext) + 1, 0);
        }
        while (strcmp(msg.mtext, "KRAJ") != 0);
        
        wait(NULL);

        msgctl(msg_queue, IPC_RMID, NULL);
    }

    return 0;
}