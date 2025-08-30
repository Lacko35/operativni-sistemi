#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MSG_QUEUE_KEY 10101

struct message {
    long type;
    int num;
};

int main(int argc, char* argv[]) {
    int msg_queue_id = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | 0666);
    
    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom kreiranja/ucitavanja reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    pid_t pid2 = fork();

    if(pid1 == 0) {
        struct message receiver;
        pid1 = getpid();

        if(msgrcv(msg_queue_id, &receiver, sizeof(int), 3, IPC_NOWAIT) != -1) {
            execl(
                "/home/lazar/operativni_sistemi/kolokvijumII-2024-grupaIIIb/izvrsni/prvi_pom",
                "prvi_pom", 
                "/home/lazar/operativni_sistemi/kolokvijumII-2024-grupaIIIb/datoteke/prva.txt",
                NULL
            );
        }
        else {
            exit(0);
        }          
    }
    else if(pid2 == 0) {
        struct message receiver;
        pid2 = getpid();

        if(msgrcv(msg_queue_id, &receiver, sizeof(int), 3, IPC_NOWAIT) != -1) {
            execl(
                "/home/lazar/operativni_sistemi/kolokvijumII-2024-grupaIIIb/izvrsni/prvi_pom",
                "prvi_pom", 
                "/home/lazar/operativni_sistemi/kolokvijumII-2024-grupaIIIb/datoteke/druga.txt",
                NULL
            );
        }   
        else {
            exit(0);
        }                
    }
    else if(pid1 != 0 && pid2 != 0) {
        int sum1 = 0;
        int sum2 = 0;

        while(1) {
            struct message receiver;

            msgrcv(msg_queue_id, &receiver, sizeof(int), 0, 0);

            if(receiver.type == 1) {
                if(receiver.num == 0) {
                    struct message sender;
                    sender.type = 3;
                    msgsnd(msg_queue_id, &sender, sizeof(int), 0);
                }

                sum1 += receiver.num;
            }
            else {
                if(receiver.num == 0) {
                    struct message sender;
                    sender.type = 3;
                    msgsnd(msg_queue_id, &sender, sizeof(int), 0);
                }

                sum2 += receiver.num;
            }
        }

        wait(NULL);
        wait(NULL);

        msgctl(msg_queue_id, IPC_RMID, NULL);

        if(sum1 > sum2) {
            printf("Roditelj je vecu sumu dobio od procesa [%d]\n", pid1);
        }
        else {
            printf("Roditelj je vecu sumu dobio od procesa [%d]\n", pid2);
        }
    }
    else {
        perror("Doslo je do greske prilikom kreiranja novih procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}