#include <stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/shm.h>
#include "shared_memory.h"
#define IPC_RESULT_ERROR (-1)
/*Returns the ID of the memory Shared Block or -1 in error case*/
static int get_shared_block(char* filename, int size){
    key_t key; //Semaphores Use it
    key = ftok(filename,0); //get a numered key asocieted with the filename we are using.
    if(key == IPC_RESULT_ERROR){
        return IPC_RESULT_ERROR; //no logra obtener el bloque
    }
    //Uses the key to create or get a block of shared memory associated with that key
    // It returns a shared memory block ID 
    return shmget(key, size, 0644 | IPC_CREAT); 
}

/*It calls the get memory get the block ID to lather map the block into my adress space*/
/**/
char * attach_memory_block(char*filename, int size){
    int shared_block_id = get_shared_block(filename, size);
    char* result;
    if(shared_block_id == IPC_RESULT_ERROR){
        return NULL;
    }

    //Uses the block ID to map the block into his processes space 
    //and give us a pointer to the block so, we can start using the memory
    result = shmat(shared_block_id, NULL, 0); 
    if(result == (char *)IPC_RESULT_ERROR){
        return NULL;
    }

    return result;    
}

bool detach_memory_block(char * block){
    return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroy_memory_block(char * filename){
    int shared_block_id = get_shared_block(filename,0);
    if(shared_block_id == IPC_RESULT_ERROR){
        return NULL;
    }
    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR );    
}

int getLine (char *prmpt, char *buff, size_t sz) {
    int ch, extra;

    // Get line with buffer overrun protection.
    if (prmpt != NULL) {
        printf ("%s", prmpt);
        fflush (stdout);
    }
    if (fgets (buff, sz, stdin) == NULL)
        return 1;

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? 2 : 0;
    }
    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff)-1] = '\0';
    return 0;
}
//File made to simplify code




