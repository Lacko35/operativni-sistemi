#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
    long type;
    int num;
};

int main() {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    for(int i = 0; i < 10; i++) {
        struct message receiver;
        msgrcv(msg_queue, &receiver, sizeof(int), 1, 0);
        int num = receiver.num;

        printf("Krecemo obradu...\n");
        fflush(stdout);

        if(num % 2 == 0) {
            num += 2;
        }
        else {
            num -= 2;
        }

        struct message sender;
        sender.type = 2;
        sender.num = num;

        msgsnd(msg_queue, &sender, sizeof(int), 0);
    }

    return 0;
}