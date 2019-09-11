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

// Check initialized lib state macro
#define CHECK_INIT if(!initialized){init();initialized=1;}

//  Global queues
FILA2 thread_queue;
// TODO: state queues

// Guarantees initialization at the first call of the lib
static int initialized = 0;

//  Thread control globals
static TCB_t main_thread;
TCB_t* current_thread;
ucontext_t sched_context;

//  Library initialization
int init() {
	printf("+ Initializing cthread...\n");

    // Populate main thread TCB
    main_thread.tid = 0;
    main_thread.state = PROCST_EXEC;
    main_thread.prio = 0;
    
    getcontext(&main_thread.context);

    // Creates a new context to begin execution at function schedule()
    getcontext(&sched_context);
    sched_context.uc_link = 0;
    sched_context.uc_stack.ss_sp = malloc(TSTACKSIZE);
    sched_context.uc_stack.ss_size = TSTACKSIZE;
    sched_context.uc_stack.ss_flags = 0;
    makecontext(&sched_context, (void*)schedule, 0);

    // Sets the main thread and initialize the queue
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
    CHECK_INIT;  // Checks if the lib has been initialized. If not, will initialize it

	/* Tries to generate a TCB */
	TCB_t* this_tcb = malloc(sizeof(TCB_t));
	if(this_tcb != NULL) {
        // Create the new therad
		this_tcb->tid = make_tid();
		this_tcb->state = PROCST_APTO;
		this_tcb->prio = 0;
        
        // Copy actual context and make the new one
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
		return -9;
	}
}

int cyield(void) {
    CHECK_INIT;  // Checks if the lib has been initialized. If not, will initialize it

    printf("+ Thread %d is yielding.\n", current_thread->tid);

    // Thread has yielded
    printf("+ Thread %d is going back to scheduler.\n", current_thread->tid);
    current_thread->state = PROCST_APTO;
    // Save current context
    int yielding = TRUE;
    getcontext(&current_thread->context);
    if (yielding == TRUE) {
        yielding = FALSE;
        setcontext(&sched_context);
    } 

    // Thread resumes
    printf("+ Thread %d is resuming.\n", current_thread->tid);
    current_thread->state = PROCST_EXEC;

    return 0;
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


