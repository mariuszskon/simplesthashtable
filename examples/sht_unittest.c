/* this code is under the Unlicense - see LICENSE for details */

#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "sht.h"

#define HASH_TABLE_SIZE 10

int main() {
    struct sht_entry table[HASH_TABLE_SIZE];
    int i;

    /* verify sane assumptions */
    assert(CHAR_BIT == 8);
    assert(SHT_MAX_KEY_LENGTH == 15);

    /* size-agnostic general tests */
    sht_init_table(table, HASH_TABLE_SIZE);
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        assert(table[i].key[0] == '\0');
        assert(table[i].value == NULL);
    }

    assert(sht_lookup(table, HASH_TABLE_SIZE, "doesnotexist") == NULL);

    sht_insert(table, HASH_TABLE_SIZE, "exists", (void *) 0x1234);
    assert(sht_lookup(table, HASH_TABLE_SIZE, "exists") == (void *) 0x1234);

    sht_insert(table, HASH_TABLE_SIZE, "another", (void *) 0x5678);
    assert(sht_lookup(table, HASH_TABLE_SIZE, "another") == (void *) 0x5678);

    sht_insert(table, HASH_TABLE_SIZE, "012345678912345", (void *) 0xffffffff);
    assert(sht_lookup(table, HASH_TABLE_SIZE, "012345678912345") == (void *) 0xffffffff);

    assert(sht_lookup(table, HASH_TABLE_SIZE, "anothe") == NULL);
    assert(sht_lookup(table, HASH_TABLE_SIZE, "existsalso") == NULL);
    assert(sht_lookup(table, HASH_TABLE_SIZE, "012345678912346") == NULL);

    assert(sht_delete(table, HASH_TABLE_SIZE, "doesnotexist") == NULL);
    assert(sht_delete(table, HASH_TABLE_SIZE, "anothe") == NULL);
    assert(sht_delete(table, HASH_TABLE_SIZE, "012345678912346") == NULL);

    assert(sht_delete(table, HASH_TABLE_SIZE, "exists") == (void *) 0x1234);
    assert(sht_lookup(table, HASH_TABLE_SIZE, "exists") == NULL);

    sht_init_table(table, HASH_TABLE_SIZE);
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        assert(table[i].key[0] == '\0');
        assert(table[i].value == NULL);
    }

    /* specific bitness required */
    if (sizeof(unsigned long) * CHAR_BIT == 32) {
        assert(sht_hash("") == 0x811c9dc5);
        assert(sht_hash("hello") == 0x4f9f2cab);
        assert(sht_hash("012345678912345") == 0xe9cce339);
    } else {
        assert(sht_hash("") == 0xcbf29ce484222325);
        assert(sht_hash("hello") == 0xa430d84680aabd0b);
        assert(sht_hash("012345678912345") == 0xf0179b37002ce099);

        sht_insert(table, HASH_TABLE_SIZE, "hello", (void *) 1);
        sht_insert(table, HASH_TABLE_SIZE, "world", (void *) 2);
        sht_insert(table, HASH_TABLE_SIZE, "universe", (void *) 3);
        sht_insert(table, HASH_TABLE_SIZE, "goodbye", (void *) 4);
        sht_insert(table, HASH_TABLE_SIZE, "example", (void *) 5);
        sht_insert(table, HASH_TABLE_SIZE, "abc", (void *) 6);

        /* make sure collisions are handled correctly, even when certain elements are deleted */
        assert(sht_lookup(table, HASH_TABLE_SIZE, "goodbye") == (void *) 4);
        assert(sht_delete(table, HASH_TABLE_SIZE, "hello") == (void *) 1);
        assert(sht_lookup(table, HASH_TABLE_SIZE, "goodbye") == (void *) 4);
        assert(sht_delete(table, HASH_TABLE_SIZE, "universe") == (void *) 3);
        assert(sht_lookup(table, HASH_TABLE_SIZE, "goodbye") == (void *) 4);
        assert(sht_lookup(table, HASH_TABLE_SIZE, "abc") == (void *) 6);

        assert(sht_lookup(table, HASH_TABLE_SIZE, "hello") == NULL);
        assert(sht_lookup(table, HASH_TABLE_SIZE, "world") == (void *) 2);
    }
}
