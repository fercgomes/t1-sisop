#include "utils.h"
#include <stdio.h>
#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../src/lib.h"

extern FILA2 thread_queue;

/* Utils */
void print_queue(PFILA2 queue) {
	queue = &thread_queue;
	PNODE2 it = queue->first;
	while(it != NULL) {
		TCB_t* tcb = (TCB_t*) it->node;
		printf("Thread id = %d\n", tcb->tid);
		printf("Thread prio = %d\n", tcb->prio);
		switch(tcb->state) {
			case PROCST_APTO:
				printf("Thread is ready.\n");
				break;
			case PROCST_BLOQ:
				printf("Thread is blocked.\n");
				break;
			case PROCST_EXEC:
				printf("Thread is executing.\n");
				break;
			case PROCST_TERMINO:
				printf("Thread is finished.\n");
				break;
			default:
				printf("Invalid thread state.\n");
				break;
		}
		printf("------\n");

		it = it->next;
	}
}

int compareFiles(FILE* fp1, FILE* fp2) 
{ 
	if (!fp1 || !fp2)
	{
		printf("Error: File pointer is null\n");
		return -1;
    }
    // fetching character of two file 
    // in two variable ch1 and ch2 
    char ch1 = getc(fp1); 
    char ch2 = getc(fp2); 
  
    // error keeps track of number of errors 
    // pos keeps track of position of errors 
    // line keeps track of error line 
    int error = 0, pos = 0, line = 1; 
  
    // iterate loop till end of file 
    while (ch1 != EOF && ch2 != EOF) 
    { 
        pos++; 
  
        // if both variable encounters new 
        // line then line variable is incremented 
        // and pos variable is set to 0 
        if (ch1 == '\n' && ch2 == '\n') 
        { 
            line++; 
            pos = 0; 
        } 
  
        // if fetched data is not equal then 
        // error is incremented 
        if (ch1 != ch2) 
        { 
            error++; 
            printf("Line Number : %d \tError"
               " Position : %d \n", line, pos); 
        } 
  
        // fetching character until end of file 
        ch1 = getc(fp1); 
        ch2 = getc(fp2); 
    } 
  
    printf("Total Errors : %d\t\n", error);
    return error;
} 

