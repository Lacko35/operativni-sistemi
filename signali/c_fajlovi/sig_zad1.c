#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void catch_int(int sigNum) {
    signal(SIGALRM, SIG_DFL);

    printf("\n[Proslo je previse vremena od kako nije unet password!]\n");
    fflush(stdout);
    exit(0);
}

int main() {
    signal(SIGALRM, catch_int);

    char username[20];
    printf("Unesite username: ");
    scanf("%s", username);

    alarm(5);

    char password[20];
    printf("Unesite password: ");
    scanf("%s", password);

    printf("username: %s\t password: %s\n", username, password);

    return 0;
}