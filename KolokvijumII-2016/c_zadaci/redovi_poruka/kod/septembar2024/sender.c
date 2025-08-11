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
        int broj;
        printf("Unesite broj: ");
        scanf("%d", &broj);

        suma += broj;

        msg.type = 1;
        msg.num = broj;
        msgsnd(msg_queue, &msg, sizeof(int), 0);
    }

    return 0;
}