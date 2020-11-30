#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<semaphore.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "shared_memory.h" 
#define FILEW "sharedReaderQueue.c"

node* reader_queue =NULL;

sem_t* semSpy;
sem_t* semWR;
sem_t* semMutex;

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

void getDateByLine(char* line ,char*result){
    int cont = 0;
    for(int i = 3; i<13; i++){
        result[cont] = line[i];
        cont++;
    }
}

void getHourByLine(char*linea, char*result){
   int cont = 0;
   for(int i = 14; i < 19; i++){
     result[cont] = linea[i];
     cont++;
   }
 }

void readerExecution(struct userParameters* up){
    struct process *p= get_by_index(&reader_queue, up->index);
    int numLinea = 0;
    int read_count = 0;
    while(true){
        int exTime = up->exTime;
        int sleepTime = up->sleepTime;
        int seconds = 0;
        //PID,"writer","write","bl",0,"          ","     "
        
        sem_wait(semSpy);        
        
        char result [360];
        queueToString(reader_queue, result);
        strncpy(up->spyBlock, result, BLOCK_SIZE);
        bzero(result,sizeof(result));
        sem_post(semSpy);
        
        sem_wait(semMutex);        
        read_count++;
        if(read_count == 1){
            sem_wait(semWR);            
        }
        sem_post(semMutex);          

        while(seconds < exTime){              
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            //Date modification
            char date[11];            
            char s[5];
            bzero(s,sizeof(s));
            sprintf(s,"%02d",(tm.tm_mday));
            strcat(date,s);
            strcat(date,"/");
            bzero(s,sizeof(s));
            sprintf(s,"%02d",(tm.tm_mon+1));
            strcat(date,s);
            strcat(date,"/");
            bzero(s,sizeof(s));
            sprintf(s,"%d",(tm.tm_year+1900));
            strcat(date,s);
            strncpy(p->date,date,sizeof(p->date));     
            bzero(date,sizeof(date));       
            //Hour modification
            bzero(s,sizeof(s));
            char hour[6];
            bzero(hour,sizeof(hour));
            sprintf(s,"%02d",(tm.tm_hour));
            strcat(hour,s);
            strcat(hour,":");
            bzero(s,sizeof(s));
            sprintf(s,"%02d",(tm.tm_min));
            strcat(hour,s);
            strcpy(p->time,hour); 
            bzero(s,sizeof(s));
            bzero(hour,sizeof(hour));  
            /*lógica de lectura*/            
            if(!isLineEmpty(numLinea,up->memoryBlock)){                
                strcpy(p->state,"ex");
                p->lineNumber = numLinea;                
                sem_wait(semSpy);
                char result2 [360];
                queueToString(reader_queue, result2);                
                strncpy(up->spyBlock, result2, BLOCK_SIZE);   
                bzero(result2,sizeof(result2));             
                sem_post(semSpy);

                char proceso[23];
                readLine(numLinea,up->memoryBlock,proceso);                
                printf("Proceso %s leyendo: %s\n", p->PID,proceso); 
                
                //Escribe en bitácora
                FILE *fp;
	            fp = fopen("bitacora.txt", "a");
                if(fp == NULL){
                    printf("Error(open): %s\n", "bitacora.txt");        
                    exit(EXIT_FAILURE);
                }
                char bitacoraProcess[40];                
                bzero(bitacoraProcess,sizeof(bitacoraProcess));
                processToString(p,bitacoraProcess);
                strcat(bitacoraProcess," read: ");
                strcat(bitacoraProcess,proceso);
                bzero(proceso,sizeof(proceso));
                strcat(bitacoraProcess,"\n");
                fputs(bitacoraProcess, fp);
                bzero(bitacoraProcess,sizeof(bitacoraProcess));
                fclose(fp);
                
            }else{
                break;
            } 
            seconds++;
            numLinea++;
            sleep(1);
        }        
        sem_wait(semMutex);

        read_count--;        
        if(read_count == 0){            
            sem_post(semWR);
        }
        sem_post(semMutex);
        puts("proceso terminado...");     
        if(numLinea > up->memoryLines){
            numLinea = 0;
        }
        strcpy(p->state,"sl");
        sem_wait(semSpy);
        char result3 [360];
        queueToString(reader_queue, result3);
        strncpy(up->spyBlock, result3, BLOCK_SIZE);
        bzero(result3,sizeof(result3));
        sem_post(semSpy);        
        sleep(up->sleepTime);
        
        strcpy(p->state,"bl");
        sem_wait(semSpy);
        char result4 [360];
        queueToString(reader_queue, result4);
        strncpy(up->spyBlock, result4, BLOCK_SIZE);
        bzero(result4,sizeof(result4));
        sem_post(semSpy);
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
    char cantReaders[2];
    int answerCantW = getLine("Write the number of [Readers] to start the program (MAX 9):",cantReaders,sizeof(cantReaders));
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
    char cantReadingTime[3];
    int answerCantRT= getLine("Write the reading time to start the program (MAX 99): ",cantReadingTime,sizeof(cantReadingTime));
    if(validateAnswer(answerCantRT) != 0){
        return -1;
    }
    int cantProcess = atoi(cantReaders);
    //Crea todos los procesos previo a su ejecución
    for(int i=0; i< cantProcess; i++){
        char* PID;
        PID = (char *) malloc(sizeof(char));
        bzero(PID, sizeof(PID));
        
        char* num;
        num = (char *) malloc(sizeof(char));
        bzero(num, sizeof(num));

        strcpy(PID,"r");
        sprintf(num,"%d",i);        
        strcat(PID,num);
        //bl, sl, ex, wt
        //PID, Type, action, state, lineNumber, date, time
        struct process pn = {PID,"reader","read","bl",0,"          ","     "};        
        reader_queue = push(reader_queue,pn);        
    }
    
    //Se ininicializa la memoria compartida con el espía.
    char *spyBlock = attach_memory_block(FILEW, BLOCK_SIZE);
    if(spyBlock == NULL){
        printf("Error: could not get the block\n");
        return -1;
    }
    /**Sem setup*/
    // sem_unlink(SPYSEM);
    // sem_unlink(WRSEM);
    sem_unlink(SEMMUTEX);

    semSpy = sem_open(SPYSEM,O_CREAT);
    if(semSpy == SEM_FAILED){
        perror("sem_open/semspy");
        exit(EXIT_FAILURE);
    }

    semWR = sem_open(WRSEM,O_CREAT);
    if(semWR == SEM_FAILED){
        perror("sem_open/semWR");
        exit(EXIT_FAILURE);
    }

    semMutex = sem_open(SEMMUTEX,O_CREAT,0660,cantProcess);
    if(semMutex == SEM_FAILED){
        perror("sem_open/semMutex");
        exit(EXIT_FAILURE);
    }
    struct userParameters upList[cantProcess];
    pthread_t threads[cantProcess];	
    for(int i=0; i< cantProcess; i++){        
        struct userParameters up = {block,spyBlock,atoi(cantReadingTime), atoi(cantSleep), i, (size/22)};
        upList[i] = up;    
        pthread_create(&threads[i],NULL,readerExecution,&upList[i]);				
	}    
	for (int i = 0; i < cantProcess; i++){
        if (pthread_join(threads[i], NULL) != 0){
            fprintf(stderr, "error: Cannot join thread # %d\n", i);
        }
    }
    puts("post hilos 70");
    detach_memory_block(block);

    // sem_close(semWR);
    // sem_close(semSpy);
    // sem_close(semMutex);
    return 0;
    //writes to a block of shared memory
}