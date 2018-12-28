/* simplehashtable (sht.c) */
/* this uses linear probing open addressing (https://en.wikipedia.org/wiki/Open_addressing) to handle collisions */

#include <string.h>

#include "sht.h"

/* Fowler-Noll-Vo alternative (FNV-1a) hash function https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function */
unsigned long sht_hash(char *s) {
    unsigned long hash;
    int i;
    /* these are standard FNV offsets and primes */
    unsigned long fnv_offset[] = {0x811c9dc5UL, 0xcbf29ce484222325UL, 0x6c62272e07bb014262b821756295c58dUL, 0xdd268dbcaac550362d98c384c4e576ccc8b1536847b6bbb31023b4c8caee0535UL};
    unsigned long fnv_prime[] = {(2UL << 23) + (2UL << 7) + 0x93, (2UL << 39) + (2UL << 7) + 0xb3, (2UL << 87) + (2UL << 7) + 0x3b, (2UL << 167) + (2UL << 7) + 0x63};
    int fnv_index = sizeof(unsigned long) / 4 - 1; /* 4 (32-bit) becomes 0, 8 (64-bit) becomes 1 etc. */

    hash = fnv_offset[fnv_index];
    for (i = 0; i < SHT_MAX_KEY_LENGTH && s[i] != '\0'; i++) {
        hash ^= s[i];
        hash *= fnv_prime[fnv_index];
    }

    return hash;
}

/* internal use function to either find an existing element or position where to insert new element */
static unsigned long find(struct sht_entry table[], unsigned long table_length, char *key) {
    unsigned long p = sht_hash(key) % table_length;

    while (table[p].key[0] != '\0' && strncmp(table[p].key, key, SHT_MAX_KEY_LENGTH) != 0) {
        p = (p + 1) % table_length;
    }

    return p;
}

/* get element by key */
void *sht_lookup(struct sht_entry table[], unsigned long table_length, char *key) {
    return table[find(table, table_length, key)].value;
}

/* insert/override an element's value by key */
void sht_insert(struct sht_entry table[], unsigned long table_length, char *key, void *value) {
    unsigned long p = find(table, table_length, key);

    table[p].key[0] = '\0';
    strncat(table[p].key, key, SHT_MAX_KEY_LENGTH);

    table[p].value = value;
}

/* fill a table of elements with "null" elements */
void sht_init_table(struct sht_entry table[], unsigned long table_length) {
    unsigned long i;
    struct sht_entry entry;

    entry.key[0] = '\0'; /* label entry as empty/null */
    entry.value = NULL;

    for (i = 0; i < table_length; i++) {
        table[i] = entry;
    }
}

void *sht_delete(struct sht_entry table[], unsigned long table_length, char *key) {
    unsigned long p = find(table, table_length, key);
    void *value = table[p].value;

    table[p].value = NULL; /* sht_lookup returns NULL which also happens if slot was never filled */
    /* key not modified so that in case of collisions, all elements can still be found */

    return value;
}
