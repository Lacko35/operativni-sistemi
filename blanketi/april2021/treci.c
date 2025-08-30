#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 85
#define MSG_QUEUE_ID 10101
#define ENDING_TYPE 2054

struct message {
    long type;
    int lineNum;
    int uppercaseLettersNum;
    char content[MSG_LEN];
};

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Nevalidan broj argumenata komandne linije!\n");
        exit(EXIT_FAILURE);
    }

    int msg_queue_id = msgget((key_t)MSG_QUEUE_ID, IPC_CREAT | 0666);
    
    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom kreiranja/ucitvanja reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid == 0) {
        int line = 0;

        while(1) {
            int upperCase = 0;

            struct message msg;

            msgrcv(msg_queue_id, &msg, MSG_LEN, 0, 0);

            if(strcmp(msg.content, "KRAJ") == 0) {
                break;
            }

            line++;

            for(int i = 0; i < strlen(msg.content); i++) {
                if(msg.content[i] >= 'A' && msg.content[i] <= 'Z') {
                    upperCase++;
                }
            }

            struct message sender;
            sender.type = 2;
            sender.lineNum = line;
            sender.uppercaseLettersNum = upperCase;

            msgsnd(msg_queue_id, &sender, 2 * sizeof(int), 0);
        }

        exit(EXIT_SUCCESS);
    }
    else if(pid > 0) {
        FILE* f = fopen(argv[1], "r");

        char buffer[MSG_LEN];
        while(fgets(buffer, MSG_LEN, f) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';

            struct message sender;

            sender.type = 1;
            strcpy(sender.content, buffer);

            printf("Poruka koja se prenosi: %s\n", sender.content);
            fflush(stdout);

            msgsnd(msg_queue_id, &sender, strlen(sender.content) + 1, 0);

            struct message receiver;
            msgrcv(msg_queue_id, &receiver, MSG_LEN, 2, 0);

            printf("\nredni broj prosledjene linije: %d Broj velikih slova: %d\n", receiver.lineNum, receiver.uppercaseLettersNum);
            fflush(stdout);
            printf("---------------------------------------------------------------------------------------------------------------------\n");
            fflush(stdout);
        }

        fclose(f);

        struct message ending;
        strcpy(ending.content, "KRAJ");
        ending.type = ENDING_TYPE;
        msgsnd(msg_queue_id, &ending, strlen(ending.content) + 1, 0);

        wait(NULL);

        msgctl(msg_queue_id, IPC_RMID, NULL);

        printf("KRAJ!\n");
        fflush(stdout);
    }
    else {
        perror("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}