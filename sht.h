/* this code is under the Unlicense - see LICENSE for details */

#ifndef SIMPLEHASHTABLE_SHT_H
#define SIMPLEHASHTABLE_SHT_H

/*
 * COMPILE-TIME CONFIGURABLE OPTION: MAXIMUM KEY LENGTH
 * (not including null terminator)
 * it is recommended to choose a number which is one less than a multiple of 8 to prevent wasted space
 * due to struct padding by many compilers
 */
#define SHT_MAX_KEY_LENGTH 15 /* not including null terminator */

struct sht_entry {
    char key[SHT_MAX_KEY_LENGTH+1];
    void *value;
};


unsigned long sht_hash(char *s);
void *sht_lookup(struct sht_entry table[], unsigned long table_length, char *key);
void sht_insert(struct sht_entry table[], unsigned long table_length, char *key, void *value);
void sht_init_table(struct sht_entry table[], unsigned long table_length);
void *sht_delete(struct sht_entry table[], unsigned long table_length, char *key);

#endif
