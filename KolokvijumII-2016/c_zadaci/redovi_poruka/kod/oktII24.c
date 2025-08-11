#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define LINE_LENGTH 87

struct message {
    long type;
    char text[LINE_LENGTH];
};

struct sig_control {
    int sig_ctl1 = 0;
    int sig_ctl2 = 0;
}

int main() {
    int queue = msgget(10100, IPC_CREAT | 0666);

    struct message msg;
    struct sig_control sigctl;
    
    if(fork() == 0) {
        FILE* f = fopen("c_zadaci/datoteke/prva.txt", "r");

        while(!feof(f)) {
            char line[LINE_LENGTH];

            fgets(line, LINE_LENGTH, f);
            
            strcpy(msg.text, line);
            msg.type = 1;

            msgsnd(queue, &msg, strlen(msg.type) + 1, 0);
        }

        sigctl.sig_ctl1 = 1;

        fclose(f);
    }
    else if(fork() == 0) {
        FILE* f = fopen("c_zadaci/datoteke/druga.txt", "r");

        while(!feof(f)) {
            char line[LINE_LENGTH];

            fgets(line, LINE_LENGTH, f);
            
            strcpy(msg.text, line);
            msg.type = 2;

            msgsnd(queue, &msg, strlen(msg.type) + 1, 0);
        }

        sigctl.sig_ctl2 = 1;

        fclose(f);
    }
    else {
        FILE* f = fopen("c_zadaci/datoteke/prva_mod.txt", "w");
        FILE* f2 = fopen("c_zadaci/datoteke/druga_mod.txt", "w");

        while(1) {
            if(sigctl.sig_ctl1 && sigctl.sig_ctl2) {
                break;
            }

            msgrcv(queue, &msg, LINE_LENGTH, 0, 0);

            if(msg.type == 1) {
                for(int i = 0; i < strlen(msg.text); i++) {
                    msg.text[i] = toupper(msg.text[i]);
                }

                fputs(msg.text, f);
            }
            else {
                for(int i = 0; i < strlen(msg.text); i++) {
                    msg.text[i] = tolower(msg.text[i]);
                }

                fputs(msg.text, f2);
            }
        }

        fclose(f);
        fclose(f2);

        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }

    return 0;
}