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

/* Initialization macros */
#define CHECK_INIT if(!initialized){init();initialized=1;}

/* Global queues */
FILA2 thread_queue;

/* Other globals */
static TCB_t main_thread; // TODO: state queues
static int initialized = 0;

TCB_t* current_thread;
ucontext_t sched_context;

/* Library initialization */
int init() {
	printf("+ Initializing cthread...\n");

    main_thread.tid = 0;
    main_thread.state = PROCST_EXEC;
    main_thread.prio = 0;
    
    getcontext(&main_thread.context);

    getcontext(&sched_context);
    sched_context.uc_link = 0;
    sched_context.uc_stack.ss_sp = malloc(TSTACKSIZE);
    sched_context.uc_stack.ss_size = TSTACKSIZE;
    sched_context.uc_stack.ss_flags = 0;
    makecontext(&sched_context, (void*)schedule, 0);

    current_thread = &main_thread;

	CreateFila2(&thread_queue);
    AppendFila2(&thread_queue, &main_thread);

    return 0;
}

void cleanup() {
    TCB_t* tcb_it;
    int r = FirstFila2(&thread_queue);

    /* Free stack memory, and thread blocks */
    printf("+ Cleaning up\n");
    /* Check if queue is empty */
    if(r == 0)
        do {
            /* Select a thread from queue */
            tcb_it = (TCB_t*) GetAtIteratorFila2(&thread_queue);
            free(tcb_it->context.uc_stack.ss_sp);
            free(tcb_it);
        }
        while(NextFila2(&thread_queue) == 0);

    /* Free scheduler stack */
    free(sched_context.uc_stack.ss_sp);
}

int ccreate (void* (*start)(void*), void *arg, int prio) {
    CHECK_INIT;

	/* Generate a TCB */
	TCB_t* this_tcb = malloc(sizeof(TCB_t));
	if(this_tcb != NULL) {
		this_tcb->tid = make_tid();
		this_tcb->state = PROCST_APTO;
		this_tcb->prio = 0;
        
        getcontext(&this_tcb->context);

        this_tcb->context.uc_link = &sched_context;
        this_tcb->context.uc_stack.ss_sp = malloc(TSTACKSIZE);
        this_tcb->context.uc_stack.ss_size = TSTACKSIZE;
        this_tcb->context.uc_stack.ss_flags = 0;

        makecontext(&this_tcb->context, (void*)start, 1, arg);

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
    CHECK_INIT;

    int rescheduled = 0;
    printf("+ Thread %d is yielding.\n", current_thread->tid);

    /* Save current context */
    getcontext(&current_thread->context);

    if(rescheduled) {
        /* Thread will resume */
        printf("+ Thread %d is resuming.\n", current_thread->tid);
        current_thread->state = PROCST_EXEC;
        rescheduled = 0;
        return 0;
    }
    else {
        /* Thread has yielded. It goes back to the scheduler */
        printf("+ Thread %d is going back to scheduler.\n", current_thread->tid);
        current_thread->state = PROCST_APTO;
        rescheduled = 1;
        setcontext(&sched_context);
    }
}

int cjoin(int tid) {
    CHECK_INIT;
	return -1;
}

int csem_init(csem_t *sem, int count) {
    CHECK_INIT;
	return -1;
}

int cwait(csem_t *sem) {
    CHECK_INIT;
	return -1;
}

int csignal(csem_t *sem) {
    CHECK_INIT;
	return -1;
}

int cidentify (char *name, int size) {
    CHECK_INIT;
	strncpy (name, "Autores: \nFernando Correa Gomes -> 00264317\nIron Prando -> xxxxxxxx\n Nicolau Alff -> xxxxxxxx", size);
	return 0;
}


