#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int int_counter = 0;
int alarm_counter = 0;

void catch_int(int sigNum) {
    signal(SIGINT, catch_int);
    
    int_counter++;
    
    printf("\nNastavljamo...\n");
    fflush(stdout);
}

void catch_alarm(int sigNum) {
    signal(SIGALRM, catch_alarm);
    
    printf("Kombinacija tastera [CTRL-C] je do sada pritisnuta %d puta\n", int_counter);
    fflush(stdout);

    alarm(10);
    alarm_counter++;

    if(alarm_counter == 3) {
        printf("Kombinacija tastera [CTRL-C] je UKUPNO pritisnuta %d puta\n", int_counter);
        exit(1);
    }
}

int main() {
    signal(SIGINT, catch_int);
    signal(SIGALRM, catch_alarm);

    alarm(10);

    while(1) {
        pause();
    }

    return 0;
}