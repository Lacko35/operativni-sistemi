#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

struct message {
    long type;
    int num;
};

int main() {
    int msg_queue = msgget(10101, 0666);

    struct message msg;

    int suma = 0;
    while(suma <= 1000) {
        msgrcv(msg_queue, &msg, sizeof(int), 1, 1);

        suma += msg.num;

        printf("Primljen broj putem poruke je: [%d]\n", msg.num);
    }

    return 0;
}