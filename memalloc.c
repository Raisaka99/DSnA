#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

typedef char ALIGN[16];

union header
{
    struct
    {
        size_t size;
        unsigned is_free;
        union header_t *next;
    } s;
    ALIGN stub;

};
typedef union header header_t;
//will keep track of our linked list
header_t *head, *tail;
//so two or more threads arent able to access memory at the same time
pthread_mutex_t global_malloc_lock;



void *malloc(size_t size)
{
    //data members to use
    size_t total_size;
    void *block;
    header_t *header;

    if(!size)//if the size to allocate is 0 we can return NULL
        return NULL;
    
    pthread_mutex_lock(&global_malloc_lock);//otherwise we use the thread lock
    header = get_free_block(size);//returns a pointer to the next available block of space with desired size

    if(header)// if the pointer is pointing to a block
    {
        header->s.is_free = 0;//mark the block as not free since we will use it
        pthread_mutex_unlock(&global_malloc_lock);//unlock the thread
        return (void*)(header + 1);//this points to the end of the of the header which is where the actual memory block starts
        //thats the one the caller is interested in, we then typecast it to void* and return it
    }

    total_size = sizeof(header_t) + size;//compute the total size of the block 
    block = sbrk(total_size);//sbrk() will extend the heap memory with the total_size we computed 
    if(block == (void*) - 1)
    {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    header = block;//point the header to the block
    header->s.size = size;//fill the header with the requested size
    header->s.is_free = 0;//mark it as being used
    header->s.next = NULL;//the next position will be NULL

    //no we can keep track of our linked list using the global pointers head and tail
    if(!head)
        head = header;//if the head is not pointing to anything we can point it to the first header/block of memory
    if(tail)
        tail->s.next = header;//otherwise the tail can now 
    tail = header;//tail points to the next header and the end of our list
    pthread_mutex_lock(&global_malloc_lock);//lock the thread
    return (void*)(header + 1);//return the pointer
    
}
//function that looks for the next available memory block with the desired size of memory to be allocated
header_t *get_free_block(size_t size)
{
    header_t *curr = head;//point to the start of the list
    while(curr)//go until the end
    {
        if(curr->s.is_free && curr->s.size >= size)
            return curr;//return the pointer if we found a block thats suitable
        curr = curr->s.next;//keep searching 
    }
    return NULL;//return NULL if nothing is found 
}

//this function will first determine if a block of memory is at the end of the heap and will release it back to the OS
//if thats not the case then we can simply mark that block of memory as "free" and we can maybe use it later
void free(void* block)
{
    //helping pointers
    header_t *header, *tmp;
    //flexible pointer
    void *programbreak;

    if(!block) 
        return;

    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;//header will point to the block we want to free

    programbreak = sbrk(0);//programbreak now points to the current end of the heap
    if((char*)block + header->s.size == programbreak)//this calculates the end of the list and endblock which is compared with programbreak
    {
        if(head == tail)//if theres only one block of memory
        {
            head = tail = NULL;//both pointers now pint to NULL
        }
        else
        {
            tmp = head;//tmp now points to the start
            while(tmp)//traverse the list
            {
                if(tmp->s.next == tail)//go until we find the end of the list, move tail up the list
                {
                    tmp->s.next = NULL;//now tmp points the end of the list to NULL
                    tail = tmp;//move tail to reflect this change and be the end of the list again
                }
                tmp = tmp->s.next;//search the list 
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);//this will release the exact amount of memory, we find the size of our header and the size of the block of memory
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = 1;//mark the block as "free" if it was not at the end of the heap
    pthread_mutex_unlock(&global_malloc_lock);//unlock the thread
}

//function to allocate memory to an array of num elements of nsize bytes each and returns a pointer to the memory that was allocated
//the memory is set to all 0s using memset()
void *calloc(size_t num, size_t nsize)
{
    size_t size;
    void *block;

    if(!num || !nsize)
        return NULL;//if either of the number of elements of the size of bytes is 0 we can return NULL
    size = num * nsize;//make sure we have enough space or risk overflow
    /*check mul overflow*/
    if(nsize != size / num)
        return NULL;//return if we risk overflow
    block = malloc(size);//returns pointer to the start of the allocated memory
    if(!block)
        return NULL;//if nothing was return we can return NULL
    memset(block, 0, size);//set the memory allocated to 0
    return block;//return the pointer to the memory
}

void *realloc(void *block, size_t size)
{
    header_t *header;
    void *ret;

    if(!block || !size)
        return malloc(size);
    header = (header_t*)block - 1;//we get the blocks header
    if(header->s.size >= size)//we check to see if it has the size to accomodate it
        return block;//if it does then we simply return the block back
    
    ret = malloc(size);//we call malloc() with the requested size
    if(ret)//if ret is holding memory
    {
        memcpy(ret, block, header->s.size);//reallocate to a bigger block with all its contents
        free(block);//we free the old block or memory
    }
    return ret;//return the new memory block
}