#ifndef SHARED_MEMORY_H

#define SHARED_MEMORY_H

#include <stdbool.h>

char* attach_memory_block(char* filename, int size);
bool detach_memory_block(char* block);
bool destroy_memory_block(char* filename);
int getLine (char *prmpt, char *buff, size_t sz);
//Shared values for all programs
#define BLOCK_SIZE 4096  //From the memory that store the real size
#define MEMORYFILE "memorySize.c" //File en el que guardamos el valor de la memoria compatida.
#define FILENAME "shareData.c" //File con el que asociamos la memoria...
//File made to simplify code 
#endif