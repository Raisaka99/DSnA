#include <stdlib.h>
#include <string.h>

#include "hash.h"

#define HT_PRIME_1 401
#define HT_PRIME_2 599
#define HT_INITIAL_BASE_SIZE 50

//used for the delete function
static ht_item HT_DELETED_ITEM = {NULL, NULL};

//static funtion to avoid calling or using anywhere else but in this file
//this basically makes a new ht_item allocating memory so we can use it 
static ht_item* ht_new_item(const char* k, const char* v)
{
    ht_item* i = malloc(sizeof(ht_item));// same as ht_item* i = new ht_item in C++
    i->key = strdup(k);//returns pointer to k
    i->value = strdup(v);//returns pointer to v
    return i;
}

static ht_hash_table* ht_new_sized(const int base_size)
{
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size); 

    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

//function that initializes our hash table like the one above
//instead of malloc() we use calloc() which allocates a specific amount of blocks of memory with a specific type(ht_item*)
//in our example we allocated 69 blocks of the size of ht_item*
ht_hash_table* ht_new()
{
    // ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    // ht->size = 69;
    // ht->count = 0;
    // ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    // return ht;

    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

/*This function will helps us resize our hashtable if it gets too big or too small
We create a new hashtable pointer to hold our old hashtable
We then can copy over all our non NULL values to the new hashtable
Once that is done we can swap pointers and sizes
Then we can delete the temp table*/
static void ht_resize(ht_hash_table* ht, const int base_size)
{
    if(base_size < HT_INITIAL_BASE_SIZE)
        return;

    ht_hash_table* new_ht = ht_new_sized(base_size);
    for(int i = 0; i < ht->size; i++)
    {
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM)
        {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}


//Function to resize our table up for more room
static void ht_resize_up(ht_hash_table* ht)
{
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}
//function to resize our table down when items are discarded
static void ht_resize_down(ht_hash_table* ht)
{
    const int new_size = ht->base_size/2;
    ht_resize(ht, new_size);
}
//this funtion will "free" the memory allocated to the specific block of memory
static void ht_del_item(ht_item* i)
{
    free(i->key);
    free(i->value);
    free(i);
}

void ht_del_hash_table(ht_hash_table* ht)
{
    for(int i = 0; i < ht->size; i++)
    {
        ht_item* item = ht->items[i];
        if(item != NULL)
            ht_del_item(item);
    }
    free(ht->items);
    free(ht);
}

//hash funtion that calculates the hash of a string by multiplying each letter using its ASCII value 
//the variable prime would be a prime number bigger than the alphabet size of 128
//we will run a loop that will use pow() to calculate each invidual letter against our prime number
static int ht_hash(const char* s, const int prime, const int m)
{
    long hash = 0;//using a long for a big number
    const int len_s = strlen(s);//store the length of the given string 
    for(int i = 0; i < len_s; i++)
    {
        hash += (long)pow(prime, len_s - (i+1)) * s[i];//calculate a sum for each letter in the string 
    }
    
    hash = hash % m;//take the remainder of the hash sum using mod
    
    return (int)hash;
}

//hash function that will help us with collisions
//this function uses open addressing and double hashing using two large prime numbers
static int ht_get_hash(const char* s, const int num_buckets, const int attempt)
{
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt *(hash_b + 1))) % num_buckets;
}

//hash function to insert an item at a given hashed index using the item key, table size and number of attempts so far
//if the spot in the hash table is empty we can insert the item first time, however,
//if the spot is taken, the function will run a while loop to find a place for the item to be inserted
//how it works is by giving the item a different hash until we find a spot in the table that is not taken
void ht_insert(ht_hash_table* ht, const char* key, const char* value)
{
    const int load = ht->count * 100 / ht->size;
    if(load > 70)
        ht_resize_up(ht);

    ht_item* item = ht_new_item(key, value);//create new item
    int index = ht_get_hash(item->key, ht->size, 0);//index will hold our hashed value so we can find a place on the table
    ht_item* cur_item = ht->items[index];//cur_item is a pointer that helps us allocate the said spot by pointing at it so
    //item can be easily added into it

    int i = 1;//this is our attempt counter that will increase every time the function cant find a spot for the item
    while(cur_item != NULL)//while loop using cur_item pointer to find a spot
    {
        if(cur_item != &HT_DELETED_ITEM)
        {
            if(strcmp(cur_item->key, key) == 0)
            {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->size, i);//our hash will be calculated again but with a new attempt 
        cur_item = ht->items[index];//cur_item now points to the new spot that was found, if the spot is still taken the loop will run again
        i++;//increment attempts 
    }
    //if a spot was found we can simply add the item into the empty table spot
    ht->items[index] = item;//item is now being pointed at by the empty hash table slot
    ht->count++;//the number of items has now increased
}

//hash function to search for an element in our hash table given the key(string)
//the function works by hashing the key we are given and getting an index value back from our hashing function
//with the returned index value we can point exactly to where they key might be
//if the key is found we can return it otherwise if the spot is NULL we can return NULL (not found)
//we have a while loop to keep searching for the key if the is not found on the first attempt
//the loop will run until the while loop finds a NULL value or the key
char* ht_search(ht_hash_table* ht, const char* key)
{
    int index = ht_get_hash(key, ht->size, 0);//first we get the index using our get hash function with the key
    ht_item* item = ht->items[index];//we make an item pointer so we can point to the item in the hash table with the given index

    int i = 1;//we will inialize i to 1 for our counter of how many times we will look for a key 
    while(item != NULL)//while loop to run until the item pointer does not equal NULL
    {
        if(item != &HT_DELETED_ITEM)
        {
            if(strcmp(item->key, key) == 0)//we can compare two strings with this function, if the keys are the same
            {
                return item->value;//we can return the value found with the given key
            }
        }
        index = ht_get_hash(key, ht->size, i);//otherwise, we can get a different index using our get_hash function with an updated i(attempts)
        item = ht->items[index];//item now points to the new spot where the value with the given key might be
        i++;//update attempts 
    }
    return NULL;//if we encounter NULL at any stage, the key is not there and we can't return a value
}


//hash function to delete an item from our hash table
//this funtion works by searching for a value with our given key
//we made a static item variable and the contents of it are NULL, this will replace the currents contents as if they were deleted
//now we can search whether an item is NULL(not found) or if the item is "deleted" which is just marked as empty/NULL
//if both of these are not true then we can compare keys and if we find the value with the given key
//we can simply call our ht_del_item function which frees the contents of the item object
//after we free the object we can mark it as "deleted" we do this by making it point to the global static variable HT_DELETED_ITEM
//we can keep doing this until the key is found and then we can decrease the table size
void ht_delete(ht_hash_table* ht, const char* key)
{
    const int load = ht->count * 100 / ht->size;
    if(load < 10)
        ht_resize_down(ht);
        
    int index = ht_get_hash(key, ht->size, 0);//first we get the index using our get hash function with the key
    ht_item* item = ht->items[index];//we make an item pointer so we can point to the item in the hash table with the given index
    
    int i = 1;//we will inialize i to 1 for our counter of how many times we will look for a key 
    while(item != NULL)//while loop to run until the item pointer does not equal NULL
    {
        if(item != &HT_DELETED_ITEM)//we also have to check if the item we are pointing to is not a "deleted" item
        {
            if(strcmp(item->key, key) == 0)//compare the keys(strings) if they are the same 
            {
                ht_del_item(item);//we call the function to free the contents of the item object
                ht->items[index] = &HT_DELETED_ITEM;//we now mark that item as "deleted" by making it point to the global static variable 
            }
        }
        index = ht_get_hash(key, ht->size, i);//otherwise, we can get a different index using our get_hash function with an updated i(attempts)
        item = ht->items[index];//item now points to the new spot where the value with the given key might be
        i++;
    }
    ht->count--;//decrease table sizes
}