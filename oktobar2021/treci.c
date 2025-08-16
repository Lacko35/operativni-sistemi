#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define NUMBER_OF_STUDENTS 3

struct student {
    char ime[20];
    char prezime[20];
    int index;
};

struct message {
    long type;
    struct student std;
};

int main() {
    int msg_queue_id = msgget((key_t)10101, IPC_CREAT | 0666);

    if(msg_queue_id == -1) {
        perror("Doslo je do greske prilikom kreiranja/ucitavanja reda poruka!\n");
        return 1;
    }

    pid_t pid = fork();

    if(pid == 0) {
        struct student students[NUMBER_OF_STUDENTS];
        int stdNum = 0;

        for(int i = 0; i < NUMBER_OF_STUDENTS; i++) {
            struct message receiver;
            msgrcv(msg_queue_id, &receiver, sizeof(struct student), 1, 0);

            students[stdNum++] = receiver.std;
        }

        for(int i = 0; i < stdNum - 1; i++) {
            for(int j = i + 1; j < stdNum; j++) {
                if(students[i].index > students[j].index) {
                    struct student tmp = students[i];
                    students[i] = students[j];
                    students[j] = tmp;
                }
            }
        }

        for(int i = 0; i < stdNum; i++) {
            printf("%s %s %d\n", students[i].ime, students[i].prezime, students[i].index);
            fflush(stdout);
        }
    }
    else if(pid > 0) {
        for(int i = 0; i < NUMBER_OF_STUDENTS; i++) {
            char name[20];
            printf("Unesite ime studenta:");
            fflush(stdout);
            scanf("%s", name);

            char l_name[20];
            printf("Unesite prezime studenta:");
            fflush(stdout);
            scanf("%s", l_name);
            
            int s_id;
            printf("Unesite index:");
            fflush(stdout);
            scanf("%d", &s_id);

            printf("------------------------------------------------------------------------------\n");
            fflush(stdout);

            struct message sender;

            sender.type = 1;

            strcpy(sender.std.ime, name);
            strcpy(sender.std.prezime, l_name);
            sender.std.index = s_id;

            msgsnd(msg_queue_id, &sender, sizeof(struct student), 0);
        }

        wait(NULL);

        msgctl(msg_queue_id, IPC_RMID, NULL);
    }
    else {
        perror("Doslo je do greske prilikom kreiranja procesa!\n");
        return 1;
    }

    return 0;
}