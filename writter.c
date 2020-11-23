#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "shared_memory.h" 

node *writer_queue =NULL;

int validateAnswer(int answer){
    if(answer == 1){
        puts("You have not write anything, try again.");
        return -1;
    }
    if(answer == 2){
        puts("Your input is too long, you can just write 4 digits numbers, try again.");
        return -1;
    }
    return 0;
}

int main (){

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
    //Ask the number of writers from the user
    
    char cantWritters[2];
    int answerCantW = getLine("Write the number of [Writers] to start the program (MAX 9):",cantWritters,sizeof(cantWritters));
    printf("value of: %s",cantWritters);
    if(validateAnswer(answerCantW) != 0){
        return -1;
    }
    //Ask the sleep time from the user
    char cantSleep[3];
    int answerCantS= getLine("Write the sleep time to start the program (MAX 99): ",cantSleep,sizeof(cantSleep));
    if(validateAnswer(answerCantS) != 0){
        return -1;
    }
    //Ask the writing time from the user
    char cantWritingTime[3];
    int answerCantWT= getLine("Write the writing time to start the program (MAX 99): ",cantWritingTime,sizeof(cantWritingTime));
    if(validateAnswer(answerCantWT) != 0){
        return -1;
    }
    int cantProcess = atoi(cantWritters);

    for(int i=0; i< cantProcess; i++){
        char PID[2];
        char num[2];
        strcpy(PID,"w");
        sprintf(num,"%d",i);
        strcat(PID,num);
        //bl, sl, ex, wt
        //PID, Type, action, state, lineNumber, date, time
        struct process pn = {PID,"writer","write","wt",0,"          ","     "};
        writer_queue = push(writer_queue,pn);
    }
    //print_list(writer_queue);
    // Crear la función que usarán los threads.
    //Esta recibe como parámetro la cola
    // Detro de ella usamos writeLine y como implica acceso a memoria compartida, se usa el semáforo.


    /*************/
    // pthread_t threads[cantProcess];
	// char* lines[cantProcess];	
    // for(int i=0; i< cantProcess; i++){
		
	// 	pthread_create(&threads[i],NULL,writeLine,lines[pos]);				
		
	// 	sleep(atoi(cantSleep));
	// }

	// for (int i = 0; i < cantProcess; i++){
    //   if (pthread_join(threads[i], NULL) != 0)
    //     {
    //       fprintf(stderr, "error: Cannot join thread # %d\n", i);
    //     }
    // }
    /************/
    

     writeLine(" 1 23/11/2020 12:36 0\n",0,block);
     writeLine(" 1 23/11/2020 12:36 0\n",3,block);
     writeLine(" 1 23/11/2020 12:36 0\n",5,block);

    detach_memory_block(block);

    return 0;
    //writes to a block of shared memory
}


