#ifndef SIMPLEHASHTABLE_SHT_H
#define SIMPLEHASHTABLE_SHT_H

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
