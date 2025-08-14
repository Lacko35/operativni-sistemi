#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_QUEUE_KEY 10101
#define MSG_LEN 55

struct message {
    long type;
    char content[55];
};

int main() {
    int msg_queue_id = msgget(MSG_QUEUE_KEY, 0666);

    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom ucitivanja reda poruka!\n");
        exit(1);
    }

    struct message receiver;
    
    while(1) {
        msgrcv(msg_queue_id, &receiver, 55, 0, 0);

        if(strcmp(receiver.content, "KRAJ") == 0) {
            break;
        }

        printf("Prosledjena poruka je: %s\n", receiver.content);
        fflush(stdout);
    }

    return 0;
}