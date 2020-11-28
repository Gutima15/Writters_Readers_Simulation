#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include "shared_memory.h" 
#define FILE "sharedWriterQueue.c"

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

void writerExecution(struct userParameters* up){
    while(true){

        int exTime = up->exTime;
        int sleepTime = up->sleepTime;

        time_t endWait;
        time_t start = time(NULL);
        time_t seconds = exTime;

        endWait = start + seconds;
        //11/11/1121
        //PID,"writer","write","wt",0,"          ","     "
        //wait (sspy)
        struct process *p= pop(writer_queue);
        char result = "";
        queueToString(writer_queue, result);
        strncpy(up->spyBlock, result, BLOCK_SIZE);
        //Signal(sspy)

        while(start < endWait){
            strcpy(p->state,"bl");
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            //Date modification
            char date [10];
            char s[4];            
            sprintf(s,"%02d",(tm.tm_mday));
            strcat(date,s);
            strcat(date,"/");
            sprintf(s,"%02d",(tm.tm_mon+1));
            strcat(date,s);
            strcat(date,"/");
            sprintf(s,"%d",(tm.tm_year+1900));
            strcat(date,s);
            strcpy(p->date,date);
            //Hour modification
            char hour[5];
            sprintf(s,"%02d",(tm.tm_hour));
            strcat(hour,s);
            strcat(date,":");
            sprintf(s,"%02d",(tm.tm_min));
            strcat(hour,s);
            strcpy(p->time,hour);
            //wait del semaforo.//
            // cambiar estado
            // ¿cómo lo ponemos en la memoria compartida del writer?
            /*lógica de escritura*/
            int emptyLine = getEmptyLine(up->memoryBlock);
            if(emptyLine != -1){
                //obtenemos fecha
                char proceso[22];
                processToString(p,proceso);
                writeLine(proceso,emptyLine,up->memoryBlock);
                //Escribe en bitácora
            }else{
                break;
            }
            /*Fin de escritura*/
            //signal de semaforo.
            sleep(1);
            start = time(NULL);
        }
        //Signal(sspy)
        // se manda a dormir... los segundos del usuario
    }
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
    //Crea todos los procesos previo a su ejecución
    for(int i=0; i< cantProcess; i++){
        char* PID;
        PID = (char *) malloc(sizeof(char));
        bzero(PID, sizeof(PID));
        
        char* num;
        num = (char *) malloc(sizeof(char));
        bzero(num, sizeof(num));

        strcpy(PID,"w");
        sprintf(num,"%d",i);        
        strcat(PID,num);
        //bl, sl, ex, wt
        //PID, Type, action, state, lineNumber, date, time
        struct process pn = {PID,"writer","write","wt",0,"          ","     "};        
        writer_queue = push(writer_queue,pn);        
    }
    
    //Se ininicializa la memoria compartida con el espía.
    char *spyBlock = attach_memory_block(FILE, BLOCK_SIZE);
    if(spyBlock == NULL){
        printf("Error: could not get the block\n");
        return -1;
    }
    struct userParameters *up = {block,spyBlock,atoi(cantWritingTime), atoi(cantSleep)};

    // Crear la función que usarán los threads.
    //Esta recibe como parámetro la cola
    // Detro de ella usamos writeLine y como implica acceso a memoria compartida, se usa el semáforo.


    /*************/
    pthread_t threads[cantProcess];	
    for(int i=0; i< cantProcess; i++){
        pthread_create(&threads[i],NULL,writerExecution,up);				
	}

	for (int i = 0; i < cantProcess; i++){
        if (pthread_join(threads[i], NULL) != 0){
            fprintf(stderr, "error: Cannot join thread # %d\n", i);
        }
    }
    /************/
    

    // writeLine(" 1 23/11/2020 12:36 0\n",0,block);
    // writeLine(" 1 23/11/2020 12:36 0\n",3,block);
    // writeLine(" 1 23/11/2020 12:36 0\n",5,block);

    detach_memory_block(block);

    return 0;
    //writes to a block of shared memory
}


