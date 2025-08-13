#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define ITERATIONS 10
#define MSG_QUEUE_KEY 10101

struct message {
    long type;
    int num;
};

int main() {
    int msg_queue_id = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | 0666);

    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom kreiranja/uctivanja reda poruka!");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid == 0) {
        for(int i = 0; i < ITERATIONS; i++) {
            struct message receivedMsg;
            msgrcv(msg_queue_id, &receivedMsg, sizeof(int), 1, 0);

            int number = receivedMsg.num;
            int digitsSum = 0;

            while(number > 0) {
                digitsSum += number % 10;
                number /= 10;
            }

            printf("Suma cifara broja [%d] je: [%d]", receivedMsg.num, digitsSum);
            printf("\n--------------------------------------------------------------------------\n");
        }

        exit(EXIT_SUCCESS);
    }
    else if(pid > 0) {
        for(int i = 0; i < ITERATIONS; i++) {
            struct message sendingMsg;
            sendingMsg.type = 1;
            
            printf("Unesite ceo visecifreni broj: ");
            scanf("%d", &sendingMsg.num);

            msgsnd(msg_queue_id, &sendingMsg, sizeof(int), 0);
        }

        wait(NULL);

        msgctl(msg_queue_id, IPC_RMID, NULL);
    }
    else {
        perror("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}