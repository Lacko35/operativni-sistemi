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
    int msg_queue_id = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0666);

    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom ucitivanja reda poruka!\n");
        exit(1);
    }

    pid_t pid = fork();

    if(pid == 0) {
        execl("./treci_b", "treci_b", NULL);
    }
    else {
        execl("./treci_a", "treci_a", NULL);
    }

    return 0;
}