#ifndef SHARED_MEMORY_H

#define SHARED_MEMORY_H

#include <stdbool.h>

char* attach_memory_block(char* filename, int size);
bool detach_memory_block(char* block);
bool destroy_memory_block(char* filename);
int getLine (char *prmpt, char *buff, size_t sz);
bool isLineEmpty(int lineNumber, char* block);
int getEmptyLine(char* block);
void writeLine(char *newText, int lineNumber, char* block);
char* readLine(int lineNumber, char* block, char* result);
struct process{
    char* PID;
    char* Type;
    char* action;
    char* state;
    int lineNumber;
    char* date;
    char* time;
};

struct userParameters{
    char* memoryBlock;
    char* spyBlock;
    int exTime;
    int sleepTime;
    int index;
};

typedef struct node {
    struct process *val;
    struct node * next;
} node;

void print_list(node * head);
void print_process(struct process * pr);
node*push(node * head, struct process val);
struct process *pop(node** head);
void queueToString(node * head, char* result);
void processToString(struct process *pr, char* result );
struct process *get_by_index(node ** head, int n);
//Shared values for all programs
#define BLOCK_SIZE 4096  //From the memory that store the real size
#define MEMORYFILE "memorySize.c" //File en el que guardamos el valor de la memoria compatida.
#define FILENAME "shareData.c" //File con el que asociamos la memoria...
//File made to simplify code 
#endif