#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MSG_QUEUE_KEY 10101
#define MSG_LENGTH 55

struct message {
    long type;
    char content[MSG_LENGTH];
};

int main() {
    int msg_queue_id = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | 0666);
    if (msg_queue_id == -1) {
        perror("Greška pri kreiranju/učitavanju reda poruka");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Greška pri forku");
        return 1;
    }

    pid_t pid2 = -1;
    if (pid1 > 0) { 
        pid2 = fork();
        if (pid2 == -1) {
            perror("Greška pri forku");
            return 1;
        }
    }

    if (pid1 > 0 && pid2 > 0) {
        while (1) {
            int type;
            char mContent[MSG_LENGTH];

            printf("Unesite tip poruke (1 ili 2) ili bilo koji drugi broj za izlaz: ");
            fflush(stdout);
            if (scanf("%d", &type) != 1) {
                printf("Nevažeći unos.\n");
                break;
            }
            getchar();

            if (type != 1 && type != 2) {
                struct message errorMsg;
                errorMsg.type = 1;
                strcpy(errorMsg.content, "KRAJ");
                msgsnd(msg_queue_id, &errorMsg, strlen(errorMsg.content) + 1, 0);

                errorMsg.type = 2;
                msgsnd(msg_queue_id, &errorMsg, strlen(errorMsg.content) + 1, 0);
                break;
            }

            printf("Unesite string: ");
            fflush(stdout);
            if (fgets(mContent, MSG_LENGTH, stdin) == NULL) {
                break;
            }
            mContent[strcspn(mContent, "\n")] = 0;

            struct message msg;
            msg.type = type;
            strcpy(msg.content, mContent);

            if (msgsnd(msg_queue_id, &msg, strlen(msg.content) + 1, 0) == -1) {
                perror("Slanje poruke neuspešno");
            }
        }

        wait(NULL);
        wait(NULL);
        msgctl(msg_queue_id, IPC_RMID, NULL);
    }
    else if (pid1 == 0) {
        while (1) {
            struct message msg;
            ssize_t rez = msgrcv(msg_queue_id, &msg, MSG_LENGTH, 1, IPC_NOWAIT);

            if (rez != -1) {
                if (strcmp(msg.content, "KRAJ") == 0) {
                    printf("Izlazimo iz deteta (PID=%d)\n", getpid());
                    fflush(stdout);
                    break;
                }
                printf("PID=%d | Sadržaj: %s, Broj karaktera: %zu\n", 
                        getpid(), msg.content, strlen(msg.content));
                fflush(stdout);
            } else if (errno != ENOMSG) {
                perror("Greška pri prijemu poruke");
                break;
            }
        }
        return 0;
    }
    else if (pid2 == 0) {
        while (1) {
            struct message msg;
            ssize_t rez = msgrcv(msg_queue_id, &msg, MSG_LENGTH, 2, IPC_NOWAIT);

            if (rez != -1) {
                if (strcmp(msg.content, "KRAJ") == 0) {
                    printf("Izlazimo iz deteta (PID=%d)\n", getpid());
                    fflush(stdout);
                    break;
                }
                for (size_t i = 0; i < strlen(msg.content); i++) {
                    msg.content[i] = toupper((unsigned char) msg.content[i]);
                }
                printf("PID=%d | Sadržaj: %s\n", getpid(), msg.content);
                fflush(stdout);
            } else if (errno != ENOMSG) {
                perror("Greška pri prijemu poruke");
                break;
            }
            usleep(100000);
        }
        return 0;
    }

    return 0;
}