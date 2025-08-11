#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define NUM_OF_ITERATIONS 5

struct message {
    long m_type;
    int m_content;
};

int main(int argc, char* argv[]) {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    if(msg_queue == -1) {
        perror("Doslo je do greske prilikom [preuzimanja reference/kreiranja novog] reda poruka!\n");
        exit(EXIT_FAILURE);
    } 
    
    struct message msg;

    pid_t proccess = fork();

    if(proccess > 0) {
        int even_counter = 0;

        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            int num = rand() % 100;

            msg.m_type = 1;
            msg.m_content = num;
            msgsnd(msg_queue, &msg, sizeof(num), 0);

            struct message even_msg;
            if(msgrcv(msg_queue, &even_counter, sizeof(int), 2, IPC_NOWAIT) != -1) {
                even_counter++;
            }
        }

        printf("Broj prosledjenih parnih brojeva je: [%d]\n", even_counter);
    }
    else if(proccess == 0) {
        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            msgrcv(msg_queue, &msg, sizeof(int), 1, 0);

            if(msg.m_content % 2 == 0) {
                struct message even_msg;
                even_msg.m_type = 2;
                even_msg.m_content = msg.m_content;
                msgsnd(msg_queue, &even_msg, sizeof(int), 0);
            }
            else {
                continue;
            }
        }
    }
    else {
        perror("Doslo je do greske prilikom kreiranja procesa deteta!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}