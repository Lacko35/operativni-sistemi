#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

struct student {
    char ime[20];
    char prezime[20];
    int index;
};

struct message {
    long mtype;
    struct student std;
};

#define NUM_OF_ITERATIONS 5

int main() {
    int msg_queue = msgget(10101, IPC_CREAT | 0666);

    if(msg_queue == -1) {
        printf("Doslo je do greske!");
        return 1;
    }

    struct message msg;

    pid_t proccess = fork();
    if(proccess == 0) {
        struct student students[NUM_OF_ITERATIONS];

        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            msgrcv(msg_queue, &msg, sizeof(struct student), 1, 0);
            students[i] = msg.std;
        }

        for(int i = 0; i < NUM_OF_ITERATIONS - 1; i++) {
            for(int j = i + 1; j < NUM_OF_ITERATIONS; j++) {
                if(students[i].index > students[j].index) {
                    struct student pom_std = students[i];
                    students[i] = students[j];
                    students[j] = pom_std;
                }
            }
        }

        printf("---SPISAK STUDENATA---\n");
        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            printf("\t%s %s\n", students[i].ime, students[i].prezime);
        }
    }
    else if(proccess > 0) {
        for(int i = 0; i < NUM_OF_ITERATIONS; i++) {
            char ime[20];
            printf("Unesite ime: ");
            scanf("%s", ime);

            char prezime[20];
            printf("Unesite prezime: ");
            scanf("%s", prezime);
            
            int index;
            printf("Unesite index: ");
            scanf("%d", &index);

            struct  student std;
            strcpy(std.ime, ime);
            strcpy(std.prezime, prezime);
            std.index = index;
            
            msg.mtype = 1;
            msg.std = std;
            msgsnd(msg_queue, &msg, sizeof(std), 0);
        }

        wait(NULL);

        msgctl(msg_queue, IPC_RMID, NULL);
    }
    else {
        printf("Doslo je do greske!");
        return 1;
    }

    return 0;
}