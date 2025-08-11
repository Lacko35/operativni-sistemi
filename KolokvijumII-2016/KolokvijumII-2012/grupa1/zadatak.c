#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void catch_int(int sig_int) {
    signal(SIGINT, catch_int);
}

void catch_alrm() {
    signal(SIGALRM, SIG_DFL);

    printf("\nVreme je isteklo!\n");
    exit(1);
}

int main() {
    signal(SIGINT, catch_int);
    signal(SIGALRM, catch_alrm);

    char username[25];
    printf("Unesite username: ");
    fgets(username, sizeof(username), stdin);
    
    alarm(5);

    char password[25];
    printf("Unesite password: ");
    fgets(password, sizeof(password), stdin);    

    printf("username = [%s], password = [%s]\n", username, password);

    return 0;
}