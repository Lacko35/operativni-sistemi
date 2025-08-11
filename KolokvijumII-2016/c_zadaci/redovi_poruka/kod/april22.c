#include "zaglavlja.h"

int main(int argc, char* argv[]) {
    int queue = msgget(MSG_QUEUE_ID, IPC_CREAT | 0666);

    if(queue == -1) {
        perror("Doslo je do greske prilikom preuzimanja reference postojeceg ili kreiranja novog reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    struct message msg;

    int proccess = fork();

    if(proccess > 0) {
        msg.type = 1;
        int N = (rand() % 10) + 1;

        for(int i = 0; i < N; i++) {
            int broj = rand() % 10;

            sprintf(msg.text, "%d", broj);

            msgsnd(queue, &msg, sizeof(int), 0);
        }

        sprintf(msg.text, "%d", -1);

        msgsnd(queue, &msg, sizeof(int), 0);

        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }
    else if(proccess == 0) {
        int brParnih = 0;
        int brNeparnih = 0;

        while(1) {
            int broj;

            msgrcv(queue, &msg, sizeof(int), 1, 0);
            broj = atoi(msg.text);

            printf("Primljeni broj je: [%d]\n", broj);

            if(broj == -1) {
                printf("-------------------------\nBroj parnih brojeva je [%d] a broj neparnih [%d]\n-------------------------\n", 
                    brParnih, brNeparnih
                );
                break;
            }
            else if(broj % 2 == 0) {
                brParnih++;
            }
            else {
                brNeparnih++;
            }
        }
    }
    else {
        perror("Doslo je do greske prilikom kreiranja novog procesa\n");
        exit(EXIT_FAILURE);        
    }

    return 0;
}