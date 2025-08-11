#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_QUEUE_ID 10101
#define MSG_TEXT_LENGTH 87

struct message {
    long type;
    char text[MSG_TEXT_LENGTH];
};