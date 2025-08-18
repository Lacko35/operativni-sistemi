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
    if(argc != 3) {
        perror("Nevalidan broj argumenata komandne linije!\n");
        exit(1);
    }

    int msg_queue_id = msgget((key_t)MSG_QUEUE_KEY, 0666);

    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom kreiranja/ucitavanja reda poruka!\n");
        exit(EXIT_FAILURE);
    }    

    FILE* f = fopen(argv[2], "r");

    int num;
    while(fscanf(f, "%d", &num) == 1) {
        struct message sender;

        if(strstr(argv[2], "prva.txt")) {
            sender.type = 1;
        }
        else {
            sender.type = 2;
        }

        sender.num = num;

        msgsnd(msg_queue_id, &sender, sizeof(sender.num), 0);
    }

    fclose(f);

    return 0;
}