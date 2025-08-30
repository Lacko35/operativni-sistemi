#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SENTENCES 1000   // maksimalan broj recenica
#define MAX_LENGTH 1024      // maksimalna duzina jedne recenice

int main() {
    FILE *inFile, *outFile;
    char *sentences[MAX_SENTENCES];
    int count = 0;

    inFile = fopen("ulaz.txt", "r");
    if (!inFile) {
        perror("Greska pri otvaranju ulaznog fajla");
        return 1;
    }

    // čitanje svih rečenica u memoriju
    char buffer[MAX_LENGTH];
    while (fgets(buffer, sizeof(buffer), inFile)) {
        // ukloni newline ako postoji
        buffer[strcspn(buffer, "\n")] = 0;

        // proveri da li recenica vec postoji
        int exists = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(sentences[i], buffer) == 0) {
                exists = 1;
                break;
            }
        }

        // ako ne postoji, sacuvaj je
        if (!exists && count < MAX_SENTENCES) {
            sentences[count] = strdup(buffer);
            count++;
        }
    }
    fclose(inFile);

    // upis u novi fajl
    outFile = fopen("izlaz.txt", "w");
    if (!outFile) {
        perror("Greska pri otvaranju izlaznog fajla");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(outFile, "%s\n", sentences[i]);
        free(sentences[i]); // oslobadjanje memorije
    }

    fclose(outFile);

    printf("Zavrseno. Rezultat je u izlaz.txt\n");
    return 0;
}