
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include <ucontext.h>
#include "utils.h"
#include "lib.h"
#include "schedule.h"

/* Global queues */
FILA2 thread_queue;
static TCB_t main_thread;

static ucontext_t sched_queue;

/* Library initialization */
int cthread_init() {
	printf("+ Initializing cthread...\n");

    main_thread.tid = 0;
    main_thread.state = PROCST_EXEC;
    main_thread.prio = 0;
    
    getcontext(&main_thread.context);
    main_thread.context.uc_link = &sched_queue;
    main_thread.context.uc_stack.ss_sp = malloc(TSTACKSIZE);
    main_thread.context.uc_stack.ss_size = TSTACKSIZE;
    main_thread.context.uc_stack.ss_flags = 0;
    //makecontext(&main_thread->context, (void*)start, 0);

	CreateFila2(&thread_queue);
    AppendFila2(&thread_queue, &main_thread);

    return 0;
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
        
        getcontext(&this_tcb->context);

        this_tcb->context.uc_link = &this_tcb->sched_context;
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


