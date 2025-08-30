#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define SIZE 1000
#define ITERATIONS 20

pid_t parent, pid1, pid2;
int proccessEnd = 0;

void signal_child1(int sig) {
    FILE* f = fopen("prva.txt", "w");
    if (!f) exit(1);

    srand(time(NULL) ^ getpid());

    for (int i = 0; i < SIZE; i++) {
        fprintf(f, "%d\n", rand() % 100);
    }

    fflush(f);
    fclose(f);

    kill(parent, SIGUSR2);
}

void signal_child2(int sig) {
    FILE* f = fopen("druga.txt", "w");
    if (!f) exit(1);

    srand(time(NULL) ^ getpid());
    for (int i = 0; i < SIZE; i++) {
        fprintf(f, "%c\n", 'a' + rand() % 26);
    }

    fflush(f);
    fclose(f);

    kill(parent, SIGUSR2);
}

void signal_parent(int sig) {
    proccessEnd++;

    if (proccessEnd == 1) {
        kill(pid2, SIGUSR1);
    }
    
    else if (proccessEnd == 2) {
        char buffer[128];
        FILE* f1 = fopen("prva.txt", "r");
        FILE* f2 = fopen("druga.txt", "r");

        printf("============= Sadrzaj prve datoteke =============\n");
        while (fgets(buffer, sizeof(buffer), f1)) {
            printf("%s", buffer);
        }
        fclose(f1);

        printf("============= Sadrzaj druge datoteke =============\n");
        while (fgets(buffer, sizeof(buffer), f2)) {
            printf("%s", buffer);
        }
        fclose(f2);
    }
}

int main() {
    parent = getpid();

    pid1 = fork();
    if (pid1 == 0) {
        signal(SIGUSR1, signal_child1);
        while (1) pause();
    }

    pid2 = fork();
    if (pid2 == 0) {
        signal(SIGUSR1, signal_child2);
        while (1) pause();
    }

    signal(SIGUSR2, signal_parent);

    for (int i = 0; i < ITERATIONS; i++) {
        proccessEnd = 0;
        kill(pid1, SIGUSR1); 
        sleep(10);          
    }

    kill(pid1, SIGTERM);
    kill(pid2, SIGTERM);

    wait(NULL);
    wait(NULL);

    return 0;
}
