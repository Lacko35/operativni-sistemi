#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
    long type;
    char content[100];
};

#define MESSAGE_QUEUE_KEY 10101

int main() {
    int messsage_queue = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | 0666);

    pid_t pid = fork();

    if(pid == 0) {
        execl("./zadatak2b", "zadatak2b", NULL);
    }
    else {
        struct message receiver;
        msgrcv(messsage_queue , &receiver, 100, 1, 0);

        printf("Poruka iz procesa zadatak2b: %s\n", receiver.content);

        wait(NULL);

        msgctl(messsage_queue, IPC_RMID, NULL);
    }

    return 0;
}