#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
    long mtype;
    int number;
};

int main() {
    int queue = msgget(10101, IPC_CREAT | 0666);

    if(queue == -1) {
        perror("Doslo je do greske prilikom pristupanja refererenci/kreiranja reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    struct message msg;

    pid_t proccess = fork();

    if(proccess == 0) {
        FILE* f = fopen("brojevi.txt", "w");

        fputs("Suma cifara prosledjenih brojeva", f);

        while(1) {
            int broj;
            
            msgrcv(queue, &msg, sizeof(msg.number), 1, 0);
            broj = msg.number;

            if(broj == 0) {
                break;
            }

            int suma = 0;
            while(broj > 0) {
                suma += broj % 10;
                broj /= 10;
            }

            fprintf(f, "%d ", suma);
            fflush(f);
        }

        fclose(f);
    }
    else if(proccess > 0) {
        //proces roditelj

        while(1) {
            int broj;
            printf("Unesite neki broj ili 0 za prekid programa: ");
            scanf("%d", &broj);

            if((broj >= 100 && broj <= 999) || broj == 0) {
                msg.mtype = 1;
                msg.number = broj;
                msgsnd(queue, &msg, sizeof(msg.number), 0);
            }
            else {
                continue;
            }

            if(broj == 0) {
                break;
            }
        }

        wait(NULL);

        msgctl(queue, IPC_RMID, NULL);
    }
    else {
        perror("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}