#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_OF_SENTECES 5
#define NUM_OF_LOOPS 10
#define LINE_LEN 86

int signal_coming = 0;

void signalHandler(int sig) {
    signal_coming = 1;
}

void parent_process(char* path, pid_t child_pid) {
    FILE* f = fopen(path, "w");
    char line[LINE_LEN];

    for(int i = 0; i < NUM_OF_LOOPS; i++) {
        for(int j = 0; j < NUM_OF_SENTECES; j++) {
            printf("Unesite recenicu: ");
            scanf("%s", line);

            fputs(line, f);
        }
    }

    fclose(f);

    kill(child_pid, SIGUSR1);

    while(!signal_coming) {
        pause();
    }
    signal_coming = 0;
}

void child_process(char* path, pid_t parent_pid) {
    FILE* f = fopen(path, "r");
    char line[LINE_LEN];

    for(int i = 0; i < NUM_OF_LOOPS; i++) {
        while(!signal_coming) {
            pause();
        }
        signal_coming = 0;

        for(int j = 0; j < NUM_OF_SENTECES; j++) {
            fgets(line, LINE_LEN, f);

            printf("Linija procitana je: [%s]\n", line);
        }

        kill(parent_pid, SIGUSR2);        
    }

    fclose(f);
}

int main() {
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    pid_t process = fork();
    if(process > 0) {
        parent_process("datoteke/podaci.txt", process);
        wait(NULL);
    }
    else {
        child_process("datoteke/podaci.txt", getppid());
    }

    return 0;
}