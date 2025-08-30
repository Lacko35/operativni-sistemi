#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define LINE_LENGTH 100

struct message {
    long type;
    char text[LINE_LENGTH];
};

void writeIntoFile(char file[], char content[]) {
    FILE* f = fopen(file, "a");

    if(!f) {
        perror("Doslo je do greske prilikom otvaranja fajla!\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "%s\n", content);

    fclose(f);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Ne ispravan broj argumenata komandne linije!\n");
        exit(1);
    }

    int msgQueueID = msgget((key_t)10101, IPC_CREAT | 0666);

    if(msgQueueID == -1) {
        perror("Doslo je do greske prilikom kreiranja reda poruka!");
        exit(1);
    }

    pid_t child1 = fork();

    if(child1 == 0) {
        while(1) {
            struct message msg;

            msgrcv(msgQueueID, &msg, LINE_LENGTH, 1, 0);

            if(strcmp(msg.text, "KRAJ") == 0) {
                break;
            }

            writeIntoFile("./kolokvijumII-2024-grupaIIa/prva.txt", msg.text);
        }

        exit(0);
    }
    else if(child1 > 0) { 
        pid_t child2 = fork();

        if(child2 == 0) {
            while(1) {
                struct message msg;

                msgrcv(msgQueueID, &msg, LINE_LENGTH, 2, 0);

                if(strcmp(msg.text, "KRAJ") == 0) {
                    break;
                }

                writeIntoFile("./kolokvijumII-2024-grupaIIa/druga.txt", msg.text);
            }

            exit(0);            
        }
        else if(child2 > 0) {
            FILE* f = fopen(argv[1], "r");

            if(!f) {
                perror("Doslo je do greske prilikom otvaranja fajla!\n");
                exit(1);
            }

            char buffer[LINE_LENGTH];
            while(fgets(buffer, LINE_LENGTH, f)) {
                struct message msg;
                
                buffer[strcspn(buffer, "\n")] = '\0';
                strcpy(msg.text, buffer);
            
                if(buffer[0] == 'Z' || buffer[0] == 'z') {
                    msg.type = 1;
                }
                else {
                    msg.type = 2;
                }

                msgsnd(msgQueueID, &msg, strlen(buffer) + 1, 0);
            }

            fclose(f);

            struct message error;

            strcpy(error.text, "KRAJ");

            error.type = 1;
            msgsnd(msgQueueID, &error, strlen(error.text) + 1, 0);

            error.type = 2;
            msgsnd(msgQueueID, &error, strlen(error.text) + 1, 0);

            wait(NULL);
            wait(NULL);

            msgctl(msgQueueID, IPC_RMID, NULL);
        }
        else {
            perror("Doslo je do greske prilikom kreiranja procesa deteta!\n");
            exit(1);  
        }
    }
    else {
        perror("Doslo je do greske prilikom kreiranja procesa deteta!\n");
        exit(1);
    }

    return 0;
}