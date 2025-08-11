#include "../zaglavlja.h"

int main(int argc, char* argv[]) {
    int queue_id = msgget(MSG_QUEUE_ID, IPC_CREAT | 0666);

    struct message msg;

    if(fork() != 0) {
        for(int i = 0; i < 5; i++) {
            fgets(msg.text, MSG_TEXT_LENGTH, stdin);
            msg.type = 1;

            msgsnd(queue_id, &msg, strlen(msg.text) + 1, 0);
        }

        wait(NULL);

        msgctl(queue_id, IPC_RMID, NULL);
    }
    else {
        if(execl("./primer1b", "primer1b", "1", NULL) < 0) {
            printf("Greska prilikom izvrsenja");
        }
    }

    return 0;
}