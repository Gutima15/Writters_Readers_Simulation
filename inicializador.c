#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "shared_memory.h" 

static int get_block_size(char* answer){
    
    int validateLines = getLine("Write the number of lines that it's going to have the shared memory: ",answer,sizeof(answer));
    if(validateLines == 1){
        puts("You have not write anything, try again.");
        return -1;
    }
    if(validateLines == 2){
        puts("Your input is too long, you can just write 4 digits numbers, try again.");
        return -1;
    }
    if (atoi(answer) == 0){
        puts("There's no numbers in the sentence, try again.");
        return -1;
    }    
    return validateLines;
}

int main (){   
    //Creo la memoria compartida para guardar el valor real de la memoria que se utilizará en el proyecto.
    char *mblock = attach_memory_block(MEMORYFILE, BLOCK_SIZE);
    if(mblock == NULL){
        puts("Error: could not get the block\n");
        return -1;
    }
    char answer[4];
    int ansResult= get_block_size(answer);

    if(ansResult == -1){
        return 0;
    }
    int size= atoi(answer)*22; //Que son la cantidad de caracteres por línea " 1 23/11/2020 12:36 5\n"
    char newAnswer[6];
    sprintf(newAnswer,"%d",size);
    printf("Memory size of: \"%s\"\n", newAnswer);
    strncpy(mblock, newAnswer, BLOCK_SIZE); 
    detach_memory_block(mblock);


    //Grab the shared memory for the simulation
    char *block = attach_memory_block(FILENAME,size); 
    if(block == NULL){
        puts("Error: could not get the block\n");
        return -1;
    }
    for(int i = 0; i<size; i++){
        char character[1];
        strcpy(character,"_");
        if((i+1)%22 == 0 && i>0){
            printf("indice del enter: %d\n",i);
            strcpy(character,"\n");
            
        }
        strcat(block,character);        
    }
    detach_memory_block(block);
    return 0;
    
 //gcc inicializador.c shared_memory.c -o init  
}
