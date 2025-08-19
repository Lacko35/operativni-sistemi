#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define M_LEN 60
#define M_QUEUE_KEY 10101

struct message {
    long type;
    char text[M_LEN];
};

void writeIntoFile(char file[], char sentance[]) {
    FILE* f = fopen(file, "a");

    fputs(sentance, f);

    fclose(f);
}

int main() {
    int msgQueue = msgget((key_t)M_QUEUE_KEY, IPC_CREAT | 0666);

    if(msgQueue == -1) {
        perror("Doslo je do greske prilikom kreiranja reda poruka!\n");
        exit(1);  
    }

    pid_t pid1 = fork();

    if(pid1 == 0) {
        while(1) {
            struct message msg;

            msgrcv(msgQueue, &msg, M_LEN, 1, 0);

            if(strcmp(msg.text, "KRAJ") == 0) {
                break;
            }

            for(int i = 0; i < strlen(msg.text); i++) {
                msg.text[i] = toupper(msg.text[i]);
            }

            writeIntoFile("septembar2023/datoteke/sredjena.txt", msg.text);
        }

        return 0;
    }
    else {
        pid_t pid2 = fork();

        if(pid2 == 0) {
            while(1) {
                struct message msg;

                msgrcv(msgQueue, &msg, M_LEN, 2, 0);

                if(strcmp(msg.text, "KRAJ") == 0) {
                    break;
                }

                for(int i = 0; i < strlen(msg.text); i++) {
                    msg.text[i] = tolower(msg.text[i]);
                }

                writeIntoFile("septembar2023/datoteke/sredjena.txt", msg.text);
            }

            return 0; 
        }
        else {
            int rowNo = 0;

            FILE* f = fopen("septembar2023/datoteke/poruka.txt", "r");

            char buff[M_LEN];
            while(fgets(buff, M_LEN, f)) {
                rowNo++;
                
                struct message msg;

                buff[strcspn(buff, "\n")] = '\0';
                strcpy(msg.text, buff);

                if(rowNo % 2 == 0) {
                    msg.type = 1;
                }
                else {
                    msg.type = 2;
                }

                msgsnd(msgQueue, &msg, strlen(buff) + 1, 0);
            }

            fclose(f);

            struct message errorMsg;
            strcpy(errorMsg.text, "KRAJ");

            errorMsg.type = 1;
            msgsnd(msgQueue, &errorMsg, strlen(errorMsg.text) + 1, 0);

            errorMsg.type = 2;
            msgsnd(msgQueue, &errorMsg, strlen(errorMsg.text) + 1, 0);

            wait(NULL);
            wait(NULL);

            msgctl(msgQueue, IPC_RMID, NULL);
        }
    }

    return 0;
}