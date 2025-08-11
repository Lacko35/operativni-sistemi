#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
    long type;
    char content[50];
};

#define MESSAGE_QUEUE_KEY 10101

int main() {
    int messsage_queue = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | 0666);

    struct message sender;
    sender.type = 1;
    strcpy(sender.content, "pozdrav svima iz procesa zadatak2b");
    
    msgsnd(messsage_queue, &sender, strlen(sender.content) + 1, 0);

    return 0;
}