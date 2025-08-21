#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define REPEAT 5
#define MAX_N 10

struct message {
    long type;
    int array[MAX_N + 1];
};

int main() {
    int msg_queue_id = msgget(10101, IPC_CREAT | 0666);

    if(msg_queue_id == -1) {
        perror("Greska: neuspesno ucitavanje/kreiranje reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    msg.type = 1;

    pid_t pid = fork();

    if(pid < 0) {
        perror("Greska: kreiranje novog procesa nije uspelo!");
    }
    else if(pid == 0) {
        for(int i = 0; i < REPEAT; i++) {
            msgrcv(msg_queue_id, &msg, (MAX_N + 1) * 4, 1, 0);

            int n = msg.array[0];

            printf("%d NIZ JE: ", i+1);
            for(int j = 1; j <= n; j++) {
                printf("%d ", msg.array[j]);
            }
            printf("\n");
        }
    }
    else {
        for(int i = 0; i < REPEAT; i++) {
            int n = (rand() % 10) + 1;

            msg.array[0] = n;
            for(int j = 1; j <= n; j++) {
                msg.array[j] = (rand() % 20) + 10;
            }

            msgsnd(msg_queue_id, &msg, sizeof(msg.array), 0);
        }

        wait(NULL);

        msgctl(msg_queue_id, IPC_RMID, NULL);
    }

    return 0;
}