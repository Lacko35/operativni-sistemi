#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

int main() {

    int pid = fork();

    if(pid == 0) {
        execl("./zadatak1b", "zadatak1b", NULL);
    }
    else {
        wait(NULL);
    }

    return 0;
}