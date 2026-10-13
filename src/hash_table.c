#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"

static int HT_PRIME_1 = 151;
static int HT_PRIME_2 = 163;
static HashTableItem HT_DELETED_ITEM = {NULL, NULL};
static int INITIAL_BASE_SIZE = 53;

static HashTableItem* create_new_item(const char* key, const char* value) {
    HashTableItem* item = malloc(sizeof(HashTableItem));

    item->key = strdup(key);
    item->value = strdup(value);
    return item;
}

static HashTable* create_sized_hash_table(const int base_size) {
    HashTable* ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->entries = calloc((size_t)ht->size, sizeof(HashTableItem*));
    if (ht->entries == NULL) {
        free(ht);  
        return NULL;
    }
    return ht;
}

HashTable* create_new_hash_table() {
    return create_sized_hash_table(INITIAL_BASE_SIZE);
}

static void free_item(HashTableItem* item) {
    free(item->key);
    free(item->value);
    free(item);
}

void free_hash_table(HashTable* ht) {
    for (size_t i = 0; i < ht->size; i++) {
        HashTableItem* item = ht->entries[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            free_item(item);
        }
    }
    free(ht->entries);
    free(ht);
}

int hash(const char* key, const int prime, const int mod){
    long hash = 0;
    const int string_len = strlen(key);
    for (int i = 0; i< string_len; i++){
        hash += (long)pow(prime, string_len - (i + 1)) * key[i];
        hash = hash % mod;
    }
    return (int)hash;
}

static int get_hash(const char* key, const int num_buckets, const int attempt) {
    // double hashing method
    const int hash_a = hash(key, HT_PRIME_1, num_buckets);
    const int hash_b = hash(key, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

static void hash_table_resize(HashTable* ht; const int base_size) {
    if (base_size < INITIAL_BASE_SIZE) {
        return;
    }
    HashTable* new_ht = create_sized_hash_table(base_size);
    if (new_ht == NULL) {
        return;
    }

    for (int i = 0; i < ht->size; i++) {
        HashTable* item = ht->entries[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    const int temp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = temp_size;

    HashTableItem** tmp_entries = ht->entries;
    ht->entries = new_ht->entries;
    new_ht->entries = tmp_entries;

    free_hash_table(new_ht);
}

static void hash_resize_up(HashTable* ht) {
    const int new_size = ht->base_size * 2;
    hash_table_resize(ht, new_size);
}

static void hash_resize_down(HashTable* ht) {
    const int new_size = ht->base_size / 2;
    hash_table_resize(ht, new_size);
}

void insert(HashTable* ht, const char* key, const char* value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        hash_resize_up(ht);
    }

    HashTableItem* item = create_new_item(key, value);
    int index = get_hash(item->key, ht->size, 0);
    HashTableItem* current_item = ht->entries[index];
    int i = 1;
    while (current_item != NULL && current_item != &HT_DELETED_ITEM) {
        if (strcmp(current_item->key, key) == 0) {
            free_item(current_item);
            ht->entries[index] = item;
            return;
        }
        index = get_hash(item->key, ht->size, i);
        current_item = ht->entries[index];
        i++;
    }
    ht->entries[index] = item;
    ht->count++;
}

char* search(HashTable* ht, const char* key) {
    int index = get_hash(key, ht->size, 0);
    HashTableItem* current_item = ht->entries[index];
    int i = 1;
    while (current_item != NULL) {
        if (current_item != &HT_DELETED_ITEM) {
            if (strcmp(current_item->key, key) == 0) {
                return current_item->value;
            }
        }
        index = get_hash(item->key, ht->size, i);
        current_item = ht->entries[index];
        i++;
    }
    return NULL;
}

void delete(HashTable* ht, const char* key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        hash_resize_down(ht);
    }

    int index = get_hash(key, ht->size, 0);
    HashTableItem* current_item = ht->entries[index];
    int i = 1;
    while (current_item != NULL) {
        if (current_item != &HT_DELETED_ITEM) {
            if (strcmp(current_item->key, key) == 0) {
                free_item(current_item);
                ht->entries[index] = &HT_DELETED_ITEM;
                ht->count--;
            }
        }
        index = get_hash(item->key, ht->size, i);
        current_item = ht->entries[index];
        i++;
    }
    
}