#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define LINE 87

struct message {
    long type;
    char msg[LINE];
};

int main() {
    int msg_queue = msgget(10100, IPC_CREAT | 0666);

    struct message msg;

    if(fork() == 0) {
        FILE* f = fopen("c_zadaci/redovi_poruka/datoteke/prva.txt", "r");

        while(!feof(f)) {
            fgets(msg.msg, LINE, f);
            msg.type = 1;

            msgsnd(msg_queue, &msg, strlen(msg.msg) + 1, 0);
        }

        msg.type = 1;
        strcpy(msg.msg, "KRAJ CITANJA IZ PRVOG FAJLA");
        msgsnd(msg_queue, &msg, strlen(msg.msg) + 1, 0);

        fclose(f);
    }
    if(fork() == 0) {
        FILE* f = fopen("c_zadaci/redovi_poruka/datoteke/druga.txt", "r");

        while(!feof(f)) {
            fgets(msg.msg, LINE, f);
            msg.type = 2;

            msgsnd(msg_queue, &msg, strlen(msg.msg) + 1, 0);
        }

        msg.type = 2;
        strcpy(msg.msg, "KRAJ CITANJA IZ DRUGOG FAJLA");    
        msgsnd(msg_queue, &msg, strlen(msg.msg) + 1, 0);    
    }

    FILE* f1 = fopen("c_zadaci/redovi_poruka/datoteke/prva_mod.txt", "w");
    FILE* f2 = fopen("c_zadaci/redovi_poruka/datoteke/druga_mod.txt", "w");
    struct message msg_receiver;

    int msg1_sig = 0;
    int msg2_sig = 0;

    while(1) {
        msgrcv(msg_queue, &msg_receiver, LINE, 0, 0);

        if(msg_receiver.type == 1) {
            if(strcmp(msg_receiver.msg, "KRAJ CITANJA IZ PRVOG FAJLA") != 0) {
                for(int i = 0; i < strlen(msg_receiver.msg); i++) {
                    if(msg_receiver.msg[i] >= 'a' && msg_receiver.msg[i] <= 'z') {
                        msg_receiver.msg[i] = toupper(msg_receiver.msg[i]);
                    }
                }
                fputs(msg_receiver.msg, f1);
            }
            else {
                msg1_sig = 1;
            }
        }

        if(msg_receiver.type == 2) {
            if(strcmp(msg_receiver.msg, "KRAJ CITANJA IZ DRUGOG FAJLA") != 0) {
                for(int i = 0; i < strlen(msg_receiver.msg); i++) {
                    if(msg_receiver.msg[i] >= 'A' && msg_receiver.msg[i] <= 'Z') {
                        msg_receiver.msg[i] = tolower(msg_receiver.msg[i]);
                    }
                }
                fputs(msg_receiver.msg, f2);
            }
            else {
                msg2_sig = 1;
            }
        } 
        
        if(msg1_sig && msg2_sig) {
            break;
        }
    }

    fclose(f1);
    fclose(f2);

    wait(NULL);
    wait(NULL);

    msgctl(msg_queue, IPC_RMID, NULL);

    return 0;
}