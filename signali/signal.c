#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

int currentIteration = 0;
#define ITERATIONS 20

int buffer[20];
int first = 0, second = 0;
pid_t child1, child2;

void child1_handler(int sig) {
    if(sig == SIGUSR1) {
        for(int i = 0; i < 10; i++) {
            buffer[i] = rand() % 100;
        }

        kill(getppid(), SIGUSR2);
    }
}

void child2_handler(int sig) {
    if(sig == SIGUSR1) {
        for(int i = 10; i < 20; i++) {
            buffer[i] = (rand() % 100) + 100;
        }

        kill(getppid(), SIGUSR2);
    }
}

void parent_handler(int sig) {
    if(sig == SIGUSR2) {
        if(!first) {
            first = 1;
        }
        else if(!second) {
            second = 1;
        }

        if(first && second) {
            currentIteration++;
            first = second = 0;
            int sum = 0;

            for(int i = 0; i < 10; i++) {
                sum += buffer[i];
            }
            
            printf("Suma prve polovine niza: %d\n", sum);
            fflush(stdout);
            
            sum -= sum;

            for(int i = 10; i < 20; i++) {
                sum += buffer[i];
            }

            printf("Prosecna vrednost druge polovine niza: %f\n", (sum / 10));
            fflush(stdout);

            if(currentIteration > ITERATIONS) {
                kill(child1, SIGKILL);
                kill(child2, SIGKILL);
            }
        }
    }
}

int main() {
   srand(time(NULL));
   
   signal(SIGUSR2, parent_handler);

    child1 = fork();
    if(child1 == 1) {
        signal(SIGUSR1, child1_handler);

        while(1) {
            pause();
        }
    }
    else {
        child2 = fork();

        if(child2 == 0) {
            while(1) {
                pause();
            }
        }
        else {
            while(1) {
                sleep(10);

                kill(child1, SIGUSR1);
                kill(child2, SIGUSR2);
            }
        }
    }

   return 0;
}