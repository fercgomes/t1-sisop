#include "utils.h"
#include <stdio.h>
#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../src/lib.h"

extern FILA2 thread_queue;

TCB_t* thread_lookup(unsigned int tid) {
    TCB_t* tcb_it = NULL;
    int r = FirstFila2(&thread_queue);

    do {
        tcb_it = (TCB_t*) GetAtIteratorFila2(&thread_queue);

        if(tcb_it->tid == tid)
            return tcb_it;
    }
    while(NextFila2(&thread_queue) == 0);
}

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

