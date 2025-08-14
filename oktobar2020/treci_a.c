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

    struct message sender;

    while(1) {
        printf("Unesite poruku: ");
        fflush(stdout);
        scanf("%s", sender.content);

        if(strcmp(sender.content, "KRAJ") == 0) {
            break;
        }

        sender.type = 1;

        msgsnd(msg_queue_id, &sender, strlen(sender.content) + 1, 0);
    }

    strcpy(sender.content, "KRAJ");
    sender.type = 2;
    msgsnd(msg_queue_id, &sender, strlen(sender.content) + 1, 0);

    return 0;
}