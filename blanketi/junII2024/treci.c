#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_QUEUE_KEY 10100

struct message {
  long type;
  int num;  
};

int main(int argc, char* argv[]) {
    int msgQueueID = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | 0666);
    
    if(msgQueueID == -1) {
        perror("Greska: Neuspesno kreiranja reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    pid_t child = fork();
    
    if(child == 0) {
        while(1) {
            struct message msg;

            msgrcv(msgQueueID, &msg, sizeof(int), 1, 0);

            if(msg.num == 0) {
                break;
            }

            FILE* f = fopen("zbir_cifara.txt", "a");

            int sum = 0;
            int num = msg.num;

            while(num > 0) {
                sum += num % 10;
                num /= 10;
            }

            fprintf(f, "%d\n", sum);

            fclose(f);
        }

        return 0;
    }
    else if(child > 0) {
        struct message msg;
        msg.type = 1;

        while(1) {
            int num;

            printf("Unesite trocifreni broj ili 0 za kraj: ");
            fflush(stdout);
            scanf("%d", &num);

            if(num == 0) {
                printf("----------------------------------\nIZLAZIMO IZ PROGRAMA!\n----------------------------------\n");
                fflush(stdout);
                
                msg.num = 0;

                msgsnd(msgQueueID, &msg, sizeof(num), 0);

                break;
            }

            while(num < 100 || num > 999) {
                printf("Molimo Vas, unesite trocifreni broj ili 0 za kraj: ");
                fflush(stdout);
                scanf("%d", &num);
            }

            msg.num = num;

            msgsnd(msgQueueID, &msg, sizeof(num), 0);
        }

        wait(NULL);

        msgctl(msgQueueID, IPC_RMID, NULL);
    }
    else {
        perror("Greska: Neuspesno kreiranje novog procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}