typedef struct {
    char* key;
    char* value;
} HashTableItem;

typedef struct {
    HashTableItem** entries;
    size_t size;
    size_t base_size;
    size_t count;
} HashTable;

int insert(HashTable* ht, const char* key, const char* value);
const char* search(HashTable* ht, const char* key);
int delete(HashTable* ht, const char* key);