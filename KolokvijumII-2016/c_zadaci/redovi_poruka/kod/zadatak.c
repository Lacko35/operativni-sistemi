#include "zaglavlja.h"

int main() {
    int queue = msgget(MSG_QUEUE_ID, IPC_CREAT | 0666);

    struct message msg;

    if(fork() != 0) {
        while(1) {
            int broj = rand() % 127;

            if(broj == 0) {
                break;
            }

            sprintf(msg.text, "%d", broj);
            msg.type = 1;
            msgsnd(queue, &msg, sizeof(int), 0);
        }

        wait(NULL);

        printf("Prekid programa --- prosledjena je 0.\n");

        msgctl(queue, IPC_RMID, NULL);
    }
    else {
        int brojacParnih = 0;

        while(1) {
            msgrcv(queue, &msg, sizeof(int), 1, 0);

            int broj = atoi(msg.text);

            if(broj == 0) {
                break;
            }

            if(broj % 2 == 0) {
                brojacParnih++;
            }
            else {
                printf("Prosledjeni broj je: [%d]\n", broj);
            }
        }

        printf("Broj prosledjenih parnih brojeva je: [%d]\n", brojacParnih);

        return 0;
    }

    return 0;
}