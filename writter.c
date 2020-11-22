#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "shared_memory.h" 

int main (int argc, char* argv[]){

    if(argc!=2){
        printf("usage- -s% [stuff to write]", argv[0]);
        return -1;
    }

    //Grab the size of the shared memory
    char *mblock = attach_memory_block(MEMORYFILE, BLOCK_SIZE);
    if(mblock == NULL){
        printf("Error: could not get the Memory block\n");
        return -1;
    }

    //Grab the shared memory
    int size = atoi(mblock);
    char *block = attach_memory_block(FILENAME, size);
    detach_memory_block(mblock);
    if(block == NULL){
        printf("Error: could not get the block\n");
        return -1;
    }

    printf("writting: \"%s\"\n", argv[1]);
    strncpy(block, argv[1], size);
    detach_memory_block(block);

    return 0;
    //writes to a block of shared memory
}