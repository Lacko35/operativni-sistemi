#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MSG_LEN 40

struct message {
    long type;
    char text[MSG_LEN];
};

int main() {
    int msg_queue_id = msgget(10101, IPC_CREAT | 0666);

    if(msg_queue_id == -1) {
        perror("Error: Greska pri ucitavanju/kreiranju reda poruka!\n");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    msg.type = 1;
    strcpy(msg.text, " ");

    pid_t pid = fork();

    if(pid > 0) {
        do {
            printf("Unesite string ili KRAJ za kraj programa:");
            fflush(stdout);
            scanf("%s", msg.text);

            msgsnd(msg_queue_id, &msg, strlen(msg.text) + 1, 0);
        }
        while(strcmp(msg.text, "KRAJ") != 0);

        wait(NULL);

        msgctl(msg_queue_id, IPC_RMID, NULL);
    }
    else if(pid == 0) {
        FILE* file = fopen("izlaz.txt", "w");

        do {
            msgrcv(msg_queue_id, &msg, MSG_LEN, 1, 0);

            fputs(msg.text, file);
            fflush(file);

            printf("\nPrimljeni string je: [%s]\n----------------------------------------------------------\n", msg.text);
            fflush(stdout);
        }
        while(strcmp(msg.text, "KRAJ") != 0);

        fclose(file);

        exit(EXIT_SUCCESS);
    }
    else {
        perror("Error: Greska pri kreiranju novog procesa!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}