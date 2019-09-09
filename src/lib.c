
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include <ucontext.h>

#define TSTACKSIZE 64000

/* Global queues */
static FILA2 thread_queue;

static TCB_t main_thread;

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

PNODE2* get_next_thread() {
    TCB_t* tcb;

    FirstFila2(&thread_queue);
    /* check if queue is empty */
    if(thread_queue.first != NULL)
        do {
            tcb = (TCB_t*) GetAtIteratorFila2(&thread_queue);
            printf("popping thread %d\n", tcb->tid);
        }
        while(NextFila2(&thread_queue) == 0);
    return 0;
}

/* Library initialization */
int cthread_init() {
	printf("+ Initializing cthread...\n");

    main_thread.tid = 0;
    main_thread.state = PROCST_EXEC;
    main_thread.prio = 0;
    
    getcontext(&main_thread.context);
    main_thread.context.uc_link = 0;
    main_thread.context.uc_stack.ss_sp = malloc(TSTACKSIZE);
    main_thread.context.uc_stack.ss_size = TSTACKSIZE;
    main_thread.context.uc_stack.ss_flags = 0;
    //makecontext(&main_thread->context, (void*)start, 0);

	int r = CreateFila2(&thread_queue);

    return r;
}


/* TODO: schedule */
void schedule() {
    
}


int ccreate (void* (*start)(void*), void *arg, int prio) {

    /* Cast function pointer, so it's compatible with what makecontext expects */
    (void(*)(void*))start;

	/* Generate a TCB */
	TCB_t* this_tcb = malloc(sizeof(TCB_t));
	if(this_tcb != NULL) {
		this_tcb->tid = make_tid();
		this_tcb->state = PROCST_APTO;
		this_tcb->prio = 0;
        
        int r = getcontext(&this_tcb->context);

        this_tcb->context.uc_link = 0; // When finished, should return to scheduler!

        this_tcb->context.uc_stack.ss_sp = malloc(TSTACKSIZE);
        this_tcb->context.uc_stack.ss_size = TSTACKSIZE;
        this_tcb->context.uc_stack.ss_flags = 0;

        makecontext(&this_tcb->context, (void*)start, 0);

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


