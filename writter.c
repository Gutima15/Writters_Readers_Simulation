#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<semaphore.h>
#include "shared_memory.h" 
#define FILEW "sharedWriterQueue.c"

node* writer_queue =NULL;

sem_t* semSpy;
sem_t* semWR;


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
        //PID,"writer","write","bl",0,"          ","     "
        sem_wait(semSpy);
        struct process *p= get_by_index(&writer_queue, up->index);
        char result [360];
        queueToString(writer_queue, result);
        strncpy(up->spyBlock, result, BLOCK_SIZE);
        sem_post(semSpy);

        while(start < endWait){
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            //Date modification
            char* date = (char *) malloc(sizeof(char));
            bzero(date,sizeof(date));
            char* s= (char *) malloc(sizeof(char));            
            bzero(s,sizeof(s));
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
            char* hour = (char *) malloc(sizeof(char));
            bzero(hour,sizeof(hour));
            sprintf(s,"%02d",(tm.tm_hour));
            strcat(hour,s);
            strcat(date,":");
            sprintf(s,"%02d",(tm.tm_min));
            strcat(hour,s);
            strcpy(p->time,hour);
            sem_wait(semWR);
            /*lógica de escritura*/
            int emptyLine = getEmptyLine(up->memoryBlock);
            if(emptyLine != -1){
                strcpy(p->state,"ex");
                sem_wait(semSpy);
                char result2 [360];
                queueToString(writer_queue, result2);
                strncpy(up->spyBlock, result2, BLOCK_SIZE);
                sem_post(semSpy);

                char* proceso = (char *) malloc(sizeof(char));
                bzero(proceso,sizeof(proceso));
                processToLine(p,proceso);
                writeLine(proceso,emptyLine,up->memoryBlock);
                //Escribe en bitácora
                FILE *fp;
	            fp = fopen("bitacora.txt", "a");
                if(fp == NULL){
                    printf("Error(open): %s\n", "bitacora.txt");        
                    exit(EXIT_FAILURE);
                }
                char bitacoraProcess[40];
                processToString(p,bitacoraProcess);
                fputs(bitacoraProcess, fp);
                fclose(fp);
                
            }else{
                sem_post(semWR);
                break;
            }
            sem_post(semWR);        
            sleep(1);
            start = time(NULL);
        }        
        strcpy(p->state,"sl");
        sem_wait(semSpy);
        char result3 [360];
        queueToString(writer_queue, result3);
        strncpy(up->spyBlock, result3, BLOCK_SIZE);
        sem_post(semSpy);
        sleep(up->sleepTime);
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
        struct process pn = {PID,"writer","write","bl",0,"          ","     "};        
        writer_queue = push(writer_queue,pn);        
    }
    
    //Se ininicializa la memoria compartida con el espía.
    char *spyBlock = attach_memory_block(FILEW, BLOCK_SIZE);
    if(spyBlock == NULL){
        printf("Error: could not get the block\n");
        return -1;
    }
    /**Sem*/

    //sem_unlink(SPYSEM);
    //sem_unlink(WRSEM);

    semSpy = sem_open(SPYSEM,IPC_CREAT,0660,1);
    if(semSpy == SEM_FAILED){
        perror("sem_open/semspy");
        exit(EXIT_FAILURE);
    }

    semWR = sem_open(WRSEM,IPC_CREAT,0660,1);
    if(semWR == SEM_FAILED){
        perror("sem_open/semWR");
        exit(EXIT_FAILURE);
    }

    /**Sem*/

    /*************/
    struct userParameters upList[cantProcess];
    pthread_t threads[cantProcess];	
    for(int i=0; i< cantProcess; i++){
        struct userParameters up = {block,spyBlock,atoi(cantWritingTime), atoi(cantSleep), i};
        upList[i] = up;
        pthread_create(&threads[i],NULL,writerExecution,upList[i]);				
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



