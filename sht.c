/* simplehashtable (sht.c) */
/* this code is under the Unlicense - see LICENSE for details */
/* this uses linear probing open addressing (https://en.wikipedia.org/wiki/Open_addressing) to handle collisions */

#include <string.h>
#include <limits.h>

#include "sht.h"

/* Fowler-Noll-Vo alternative (FNV-1a) hash function https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function */
unsigned long sht_hash(char *s) {
    unsigned long hash;
    int i;
    /* these are standard FNV offsets and primes - 32-bit if unsigned long is 32 bits, else assume 64-bit mode */
    unsigned long fnv_offset = sizeof(unsigned long) * CHAR_BIT == 32 ? 2166136261UL : 14695981039346656037UL;
    unsigned long fnv_prime = sizeof(unsigned long) * CHAR_BIT == 32 ? 16777619UL : 1099511628211UL;

    hash = fnv_offset;
    for (i = 0; i < SHT_MAX_KEY_LENGTH && s[i] != '\0'; i++) {
        hash ^= s[i];
        hash *= fnv_prime;
    }

    return hash;
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

/* internal use function to either find an existing element or position where to insert new element */
static struct sht_entry *find(struct sht_entry table[], unsigned long table_length, char *key) {
    struct sht_entry *p = &table[sht_hash(key) % table_length];
    struct sht_entry *first_free = NULL;

    while (p->key[0] != '\0' && strncmp(p->key, key, SHT_MAX_KEY_LENGTH) != 0) {
        /* this is either "really free" or a "tombstone" */
        /* if we did not see a tombstone before, and this is a tombstone,
         * then this is the first free position */
        if (first_free == NULL && p->value == NULL) first_free = p;
        /* always increment the "current" position p */
        p = &table[(p - table + 1) % table_length];
    }

    if (first_free == NULL) first_free = p;

    if (strncmp(p->key, key, SHT_MAX_KEY_LENGTH) != 0) {
        /* element never found, so return free position */
        return first_free;
    }
    /* otherwise we return the found position */
    return p;
}

/* get element by key */
void *sht_lookup(struct sht_entry table[], unsigned long table_length, char *key) {
    return find(table, table_length, key)->value;
}

/* insert/override an element's value by key */
void sht_insert(struct sht_entry table[], unsigned long table_length, char *key, void *value) {
    struct sht_entry *p = find(table, table_length, key);

    p->key[0] = '\0';
    strncat(p->key, key, SHT_MAX_KEY_LENGTH);

    p->value = value;
}

void *sht_delete(struct sht_entry table[], unsigned long table_length, char *key) {
    struct sht_entry *p = find(table, table_length, key);
    void *value = p->value;

    p->value = NULL; /* sht_lookup returns NULL which also happens if slot was never filled */
    /* key not modified so that in case of collisions, all elements can still be found */

    return value;
}
