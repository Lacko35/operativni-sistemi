#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MESSAGE_QUEUE_KEY 10102
#define MESSAGE_LENGTH 70

typedef struct message {
    long type;
    char text[MESSAGE_LENGTH];
}message;

int main() {
    pid_t pid1, pid2;

    int messageQueueID = msgget((key_t)MESSAGE_QUEUE_KEY, IPC_CREAT | 0666);

    if(messageQueueID == -1) {
        perror("Doslo je do greske prilikom kreiranja novog/ucitavanja postojeceg reda poruka!\n");
        exit(1);
    }

    pid1 = fork();

    if(pid1 == 0) {
        execl("/home/lazar/operativni_sistemi/septembar2018/izvrsni/treci_pom1", "treci_pom1", NULL);
    }
    else {
        pid2 = fork();

        if(pid2 == 0) {

            execl("/home/lazar/operativni_sistemi/septembar2018/izvrsni/treci_pom2", "treci_pom2", NULL);
        }
        else {
            wait(NULL);
            wait(NULL);

            msgctl(messageQueueID, IPC_RMID, NULL);
        }
    }

    return 0;
}