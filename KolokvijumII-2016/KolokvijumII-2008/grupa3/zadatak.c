#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void catch_int(int sig_num) {
    signal(SIGINT, SIG_DFL);

    printf("Nije moguce prekinuti program!\n");
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, catch_int);

    char linija[100];
    int brojLinija = 0;
    FILE* f = fopen(argv[1], "r");

    while(fgets(linija, 100, f) != NULL) {
        brojLinija++;
        sleep(1);
    }

    fclose(f);

    printf("Fajl ima %d linija\n", brojLinija);
}