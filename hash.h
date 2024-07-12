//hash item 
typedef struct
{
    char* key;
    char* value;
}ht_item;//name of struct

//our hash table with a size and count
//it has an array of pointers pointing to items 
typedef struct
{
    int size;
    int count;
    ht_item** items;
}ht_hash_table;

void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);