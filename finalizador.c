#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>

#include "shared_memory.h"
//Se encarga de matar todos los procesos que estén en escena. Devolver los 
//recursos que había solicitado. Y eliminar el archivo físico.

int main (int argc, char* argv[]){
    if(argc != 1){
        printf("usage - %s (no args)", argv[0]);
        return -1;
    }
    if(destroy_memory_block(FILENAME)){
        printf("Destroyed block: %s\n", FILENAME);
    }else{
        printf("could not destroy block: %s\n", FILENAME);
    }
    if(destroy_memory_block(MEMORYFILE)){
        printf("Destroyed block: %s\n", MEMORYFILE);
    }else{
        printf("could not destroy block: %s\n", MEMORYFILE);
    }
    if(destroy_memory_block("sharedWriterQueue.c")){
        printf("Destroyed block: %s\n", "sharedWriterQueue.c");
    }else{
        printf("could not destroy block: %s\n", "sharedWriterQueue.c");
    }
    if(destroy_memory_block("sharedReaderQueue.c")){
        printf("Destroyed block: %s\n", "sharedReaderQueue.c");
    }else{
        printf("could not destroy block: %s\n", "sharedReaderQueue.c");
    }
    
    sem_unlink(SPYSEM);
    sem_unlink(WRSEM);
    return 0;
    
}