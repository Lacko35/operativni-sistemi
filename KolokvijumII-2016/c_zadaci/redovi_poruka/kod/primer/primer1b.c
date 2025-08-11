#include "../zaglavlja.h"

int main(int argc, char* argv[]) {
    int queue_id = msgget(MSG_QUEUE_ID, IPC_CREAT | 0666);

    struct message msg;
    int tip = atoi(argv[1]);

    for(int i = 0; i < 5; i++) {
        msgrcv(queue_id, &msg, MSG_TEXT_LENGTH, tip, 0);

        printf("Sadrzaj primljene poruke -> %s\n", msg.text);
    }

    return 0;
}