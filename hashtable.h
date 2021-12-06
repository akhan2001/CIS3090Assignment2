/**
 * @file hashtable.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sizeTable;

/**
 * @brief Hashtable implementation
 */
typedef struct table {
    char *key;
    char *value;
    struct table *next;
} table;

typedef struct {
    table **entries;
} HashTable;

int hash(char *key) {
    int value = 0;
    for (int i = 0; i < strlen(key); i++) {
        value = value * 37 + key[i];
    }
    value = value % sizeTable;

    return value;
}

table *allocate_hash(char *key, char *value) {
    table *entry = malloc(sizeof(table));
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    strcpy(entry->key, key);
    strcpy(entry->value, value);

    entry->next = NULL;

    return entry;
}

HashTable *initialize_table(void) {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->entries = malloc(sizeof(table*) * sizeTable);

    for (int i = 0; i < sizeTable; i++) {
        ht->entries[i] = NULL;
    }

    return ht;
}

void insert_value(HashTable *hashtable, char *key, char *value) {
    int index = hash(key);
    table *entry = hashtable->entries[index];
    table *prev;

    if (entry == NULL) {
        hashtable->entries[index] = allocate_hash(key, value);
        return;
    } else {
        while (entry != NULL) {
            if (strcmp(entry->key, key) == 0) {
                free(entry->value);
                entry->value = malloc(strlen(value) + 1);
                strcpy(entry->value, value);
                return;
            }

            prev = entry;
            entry = prev->next;
        }
        prev->next = allocate_hash(key, value);
    }
}

char *receive_value(HashTable *hashtable, char *key) {
    int index = hash(key);
    table *entry = hashtable->entries[index];

    if (entry == NULL) {
        return NULL;
    }

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        entry = entry->next;
    }
    return NULL;
}