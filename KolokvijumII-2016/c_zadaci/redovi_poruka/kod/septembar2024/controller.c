#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

int main() {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    pid_t senderID = fork();
    if(senderID == 0) {
        execl("./sender", "sender", NULL);
        exit(1);
    }

    pid_t receiverID = fork();
    if(receiverID == 0) {
        execl("./receiver", "receiver", NULL);
        exit(1);
    }

    wait(NULL);
    wait(NULL);

    msgctl(msg_queue, IPC_RMID, NULL);

    return 0;
}