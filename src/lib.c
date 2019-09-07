
#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"

/* Global queues */
static FILA2 thread_queue;

/* TID generator */
static int current_tid = 0;
static inline int make_tid() {
	return current_tid++;
}

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

/* Library initialization */
int cthread_init() {
	printf("+ Initializing cthread...\n");
	return CreateFila2(&thread_queue);
}

int ccreate (void* (*start)(void*), void *arg, int prio) {

	/* Generate a TCB */
	TCB_t* this_tcb = malloc(sizeof(TCB_t));
	if(this_tcb != NULL) {
		this_tcb->tid = make_tid();
		this_tcb->state = PROCST_APTO;
		this_tcb->prio = 0;
		// this_tcb->context = ????;

		/* Insert it into ready queue */
		AppendFila2(&thread_queue, this_tcb);

		return this_tcb->tid;
	}
	else {
		fprintf(stderr, "Error allocating memory.\n");
		return -1;
	}
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {
	return -1;
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
	strncpy (name, "Sergio Cechin - 2019/2 - Teste de compilacao.", size);
	return 0;
}


