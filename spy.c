#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include "shared_memory.h"

sem_t *semSpy;

int main(){
    semSpy = sem_open(SPYSEM,O_CREAT);
    if(semSpy == SEM_FAILED){
        perror("sem_open/semspy");
        exit(1);
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
    char *wblock = attach_memory_block("sharedWriterQueue.c", BLOCK_SIZE);
    if(wblock == NULL){
        printf("Error: could not get the Memory block\n");
        return -1;
    }

    char *rblock = attach_memory_block("sharedReaderQueue.c", BLOCK_SIZE);
    if(rblock == NULL){
        printf("Error: could not get the Memory block\n");
        return -1;
    }

    char *sblock = attach_memory_block("sharedREQueue.c", BLOCK_SIZE);
    if(sblock == NULL){
        printf("Error: could not get the Memory block\n");
        return -1;
    }

   int j = 0;
    char result[21]="";
    for(int i=0; i< (int)strlen(block); i+=22){        
        printf("Linea %d: %s\n", j, readLine( j, block, result));
        j++;
    }
    
    sem_wait(semSpy);
    printf("Writers block\n%s\nReaders block\n%s\nSelfils readers block\n%s\n",wblock,rblock,sblock);
    sem_post(semSpy);
    
    detach_memory_block(block);
    detach_memory_block(wblock);
    detach_memory_block(rblock);
    detach_memory_block(sblock);

    return 0;
    //reads from a block of shared memory
}