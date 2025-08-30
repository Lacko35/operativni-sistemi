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

    message receiver;

    while(1) {
        msgrcv(messageQueueID, &receiver, MESSAGE_LENGTH, 1, 0);

        if(strcmp(receiver.text, "KRAJ") == 0) {
            break;
        }

        printf("Prosledjena recenica je: %s\n", receiver.text);
        fflush(stdout);
    }

    return 0;
}