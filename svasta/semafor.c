#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BROJ_NITI 5
#define BROJ_STAMPACA 3

sem_t stampaci;


void* korisnik(void* arg) {
    int id = *(int*)arg;

    printf("Korisnik %d: čeka slobodan štampač...\n", id);
    sem_wait(&stampaci); // smanji broj dostupnih stampaca

    printf("Korisnik %d: koristi štampač.\n", id);
    sleep(2); // simulacija štampanja

    printf("Korisnik %d: završio štampanje.\n", id);
    sem_post(&stampaci); // oslobodi štampač

    return NULL;
}

int main() {
    pthread_t niti[BROJ_NITI];
    int id[BROJ_NITI];

    sem_init(&stampaci, 0, BROJ_STAMPACA); // brojački semafor sa 3 resursa

    for (int i = 0; i < BROJ_NITI; i++) {
        id[i] = i + 1;
        pthread_create(&niti[i], NULL, korisnik, &id[i]);
    }

    for (int i = 0; i < BROJ_NITI; i++) {
        pthread_join(niti[i], NULL);
    }

    sem_destroy(&stampaci);
    return 0;
}