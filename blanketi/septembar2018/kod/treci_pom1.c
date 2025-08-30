#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MESSAGE_QUEUE_KEY 10102
#define MESSAGE_LENGTH 70

typedef struct message {
    long type;
    char text[MESSAGE_LENGTH];
}message;

int main() {
    int messageQueueID = msgget((key_t)MESSAGE_QUEUE_KEY, 0666);

    if(messageQueueID == -1) {
        perror("Doslo je do greske prilikom ucitavanja postojeceg reda poruka!\n");
        exit(1);
    }

    message sender;
    sender.type = 1;
    char buff[MESSAGE_LENGTH];
    strcpy(buff, "");

    while(strcmp(buff, "KRAJ") != 0) {
        printf("Unesite recenicu ili KRAJ za prekid programa: ");
        fflush(stdout);
        fgets(buff, MESSAGE_LENGTH, stdin);

        buff[strcspn(buff, "\n")] = '\0';

        strcpy(sender.text, buff);

        msgsnd(messageQueueID, &sender, strlen(buff) + 1, 0);
    }

    return 0;
}