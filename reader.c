#include <string.h>
#include <stdio.h>
#include "shared_memory.h"

int main(){
    
    //Grab the size of the shared memory
    char *mblock = attach_memory_block(MEMORYFILE, BLOCK_SIZE);
    if(mblock == NULL){
        printf("Error: could not get the Memory block\n");
        return -1;
    }
    printf("Reading: \"%s\"\n",mblock);
    //Grab the shared memory
    int size = atoi(mblock);
    char *block = attach_memory_block(FILENAME, size);
    detach_memory_block(mblock);
    if(block == NULL){
        printf("Error: could not get the block\n");
        return -1;
    }

    printf("Reading:\n\"%s\"\n",block);
    detach_memory_block(block);

    return 0;
    //reads from a block of shared memory
}