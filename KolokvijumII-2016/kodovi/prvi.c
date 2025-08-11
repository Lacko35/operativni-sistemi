#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 50
#define MSG_QUEUE_KEY 10101
#define ITERATIONS 4

struct message {
    long type;
    char text[MSG_LEN];
};

int main() {
    int msg_queue = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0666);

    if(msg_queue == -1) {
        perror("GRESKA: Red poruka je nemoguce kreirati/ucitati!\n");
        exit(EXIT_FAILURE);
    }

    pid_t chld_pid = fork();

    if(chld_pid == -1) {
        perror("GRESKA: proces dete nije kreiran!");
    }
    else if(chld_pid == 0) {
        int control = 1;

        for(int i = 0; i < ITERATIONS; i++) {
            struct message child_msg;

            msgrcv(msg_queue, &child_msg, MSG_LEN, 1, 0);
        
            child_msg.type = 2;

            if(control % 2 == 0) {
                child_msg.text[0] = toupper(child_msg.text[0]);
            }

            msgsnd(msg_queue, &child_msg, strlen(child_msg.text) + 1, 0);

            control++;
        }
    }
    else {
        for(int i = 0; i < ITERATIONS; i++) {
            char input[MSG_LEN];

            printf("Unesite string: ");
            fflush(stdout);
            fgets(input, MSG_LEN, stdin);

            struct message parent_msg;
            parent_msg.type = 1;
            strcpy(parent_msg.text, input);

            msgsnd(msg_queue, &parent_msg, strlen(parent_msg.text) + 1, 0);

            printf("Proces roditelj je detetu poslao poruku (%s)\n", input);
            fflush(stdout);

            msgrcv(msg_queue, &parent_msg, MSG_LEN, 2, 0);

            if(parent_msg.text[0] >= 'A' && parent_msg.text[0] <= 'Z') {
                printf("Primljena je modifikovana poruka od procesa deteta!\n--------------------------------------------------------\n");
                fflush(stdout);
            }
            else {
                printf("Primljena je ne modifikovana poruka od procesa deteta!\n--------------------------------------------------------\n");
                fflush(stdout);
            }
        }

        wait(NULL);

        msgctl(msg_queue, IPC_RMID, NULL);
    }

    return 0;
}