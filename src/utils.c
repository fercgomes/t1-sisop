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

int compareFiles(FILE * fPtr1, FILE * fPtr2)
{
    char ch1, ch2;

    int line = 1;
    int col  = 0;

    do
    {
        // Input character from both files
        ch1 = fgetc(fPtr1);
        ch2 = fgetc(fPtr2);
        
        // Increment line 
        if (ch1 == '\n')
        {
            line += 1;
            col = 0;
        }
        // If characters are not same then return -1
        if (ch1 != ch2)
            return -1;

        col  += 1;

    } while (ch1 != EOF && ch2 != EOF);

    /* If both files have reached end */
    if (ch1 == EOF && ch2 == EOF)
        return 0;
    else
        return -1;
}

