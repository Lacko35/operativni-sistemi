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

    pid_t pid = fork();

    if(pid == 0) {
        execl("./zadatak3b", "zadatak3b", NULL);
    }
    else {
        for(int i = 0; i < 10; i++) {
            struct message sender;
            sender.type = 1;
            sender.num = rand() % 100;

            printf("Poslat je broj: %d\n", sender.num);
            fflush(stdout);
            msgsnd(msg_queue, &sender, sizeof(int), 0);

            struct message receiver;
            msgrcv(msg_queue, &receiver, sizeof(int), 2, 0);
            printf("Broj koji je primljen nazad je: %d\n--------------------------------------------------\n", receiver.num);
            fflush(stdout);
        }

        wait(NULL);

        msgctl(msg_queue, IPC_RMID, NULL);
    }

    return 0;
}