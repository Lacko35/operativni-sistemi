#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

pid_t child1, child2;
int child1Ends = 0, child2Ends = 0;
int iterations = 0;

void readFile(char fileName[]) {
    FILE* f = fopen(fileName, "r");
    
    printf("Sadrzaj datoteke %s je\n", fileName);
    fflush(stdout);

    char buffer[10];
    while(fgets(buffer, 10, f) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        
        printf("%s ", buffer);
        fflush(stdout);
    }
    printf("\n");

    fclose(f);
}

void child1_handler(int sig) {
    if(sig == SIGUSR1) {
        FILE* f = fopen("prva.txt", "w");

        for(int i = 0; i < 1000; i++) {
            fprintf(f, "%d\n", rand() % 100);
        }

        fflush(f);
        fclose(f);

        kill(getppid(), SIGUSR2);
    }
}

void child2_handler(int sig) {
    if(sig == SIGUSR1) {
        FILE* f = fopen("druga.txt", "w");

        for(int i = 0; i < 1000; i++) {
            fprintf(f, "%c\n", ('a' + (rand() % 26)));
        }

        fflush(f);
        fclose(f);

        kill(getppid(), SIGUSR2);
    }
}

void parent_handler(int sig) {
    if(sig == SIGUSR2) {
        if(!child1Ends) {
            child1Ends = 1;
        }
        else if(!child2Ends) {
            child2Ends = 1;
        }

        if(child1Ends && child2Ends) {
            child1Ends = child2Ends = 0;
            iterations++;

            printf("======================%d. ITERACIJA======================\n", iterations);
            fflush(stdout);

            readFile("prva.txt");
            printf("\n");
            fflush(stdout);
            readFile("druga.txt");

            if(iterations >= 20) {
                kill(child1, SIGKILL);
                kill(child2, SIGKILL);
                
                exit(0);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    
    signal(SIGUSR2, parent_handler);

    child1 = fork();

    if(child1 == 0) {
        signal(SIGUSR1, child1_handler);

        while(1) {
            pause();
        }
    }
    else {
        child2 = fork();

        if(child2 == 0) {
            signal(SIGUSR1, child2_handler);

            while(1) {
                pause();
            }
        }
        else {
            while(1) {
                sleep(10);
                
                kill(child1, SIGUSR1);
                kill(child2, SIGUSR1);
            }
        }
    }

    return 0;
}