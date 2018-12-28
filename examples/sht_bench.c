#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sht.h"

#define HASH_TABLE_SIZE 300000
#define MAX_INPUT 16

int sht_bench_getline(char *buf, int count) {
    int c;
    int len;

    fgets(buf, count, stdin);

    if (buf != NULL) {
        len = strcspn(buf, "\n");

        if (buf != NULL && buf[len] != '\n') { /* we did not finish the line! let's clear the input so we can start fresh next time */
            while ((c = fgetc(stdin)) != EOF && c != '\n');
        }

        buf[len] = '\0';
        return len;
    } else {
        return 0;
    }
}

int main() {
    struct sht_entry *table = malloc(HASH_TABLE_SIZE * sizeof(struct sht_entry));
    char input[MAX_INPUT+1];
    char key[SHT_MAX_KEY_LENGTH+1];

    sht_init_table(table, HASH_TABLE_SIZE);

    printf("Memory used: entry = (key, value) + padding = (%ld + %ld) + padding = %ld bytes, %d elements, total %ld bytes\n", (SHT_MAX_KEY_LENGTH + 1) * sizeof(char), sizeof(void *),
           sizeof(struct sht_entry),
           HASH_TABLE_SIZE,
           sizeof(struct sht_entry) * HASH_TABLE_SIZE);

    while (sht_bench_getline(input, MAX_INPUT+1) != 0 && input[0] != '\n') {
        key[0] = '\0';
        strncat(key, input, SHT_MAX_KEY_LENGTH);

        /* yes... this is a bit of a hack, but we store the number (value of first character) as a pointer to ensure we do not need any additional (dynamic) allocations */
        sht_insert(table, HASH_TABLE_SIZE, key, (void *)key[0]);
    }

    sht_bench_getline(input, MAX_INPUT+1);

    printf("%i\n", (char)sht_lookup(table, HASH_TABLE_SIZE, input));

    free(table);

    return 0;
}
