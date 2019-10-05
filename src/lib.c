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
#include <limits.h>

// Check initialized lib state macro
#define CHECK_INIT if(!initialized){init();}
#define TRUE 1
#define FALSE 0

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
void init() {
	if (initialized)
		return;
		
	printf("+ Initializing cthread...\n");

    // Populate main thread TCB
    main_thread.tid = 0;
    main_thread.state = PROCST_APTO;
    main_thread.prio = 0;
    
    getcontext(&main_thread.context);
    
    if (!initialized) 
    {
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
		
		int error = CreateFila2(&g_joinings);

		initialized=1;
		setcontext(&sched_context);
	}
    return;
}

void cleanup() {
    TCB_t* tcb_it;
    JOIN_TUPLE* tuple;
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
        } while(NextFila2(&thread_queue) == 0);

    /* Free scheduler stack */
    free(sched_context.uc_stack.ss_sp);
    
    // Clean g_joinings (which actually should be empty
	r = FirstFila2(&g_joinings);
	if(r == 0)
        do {
            tuple = (JOIN_TUPLE*) GetAtIteratorFila2(&g_joinings);
            free(tuple);
        } while(NextFila2(&thread_queue) == 0);
        
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
		fprintf(stderr, "Error allocating memory at ccreate.\n");
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
    if (current_thread->tid == tid)
		return -9;
		
    JOIN_TUPLE* tuple = NULL; // Structure defined at schedule.h
    int r = FirstFila2(&g_joinings);
    
    // Check if the tid is already being waited by another thread
    if (r == 0) {
		do {
            tuple = (JOIN_TUPLE*) GetAtIteratorFila2(&g_joinings);
            if(tuple->tid_required == tid) 
                return -9;
                
        } while(NextFila2(&g_joinings) == 0);
	}
		
	TCB_t *tcb_it = NULL;;
	r = FirstFila2(&thread_queue);
	int found = FALSE;
	
	if (r == 0) {
		do {
			tcb_it = (TCB_t*) GetAtIteratorFila2(&thread_queue);
			if (tcb_it->tid == tid) {
				if (tcb_it->state == PROCST_TERMINO)
					return -9;
					
				found = TRUE;
				tuple = malloc(sizeof(JOIN_TUPLE));
				if (tuple) {
					tuple->requirer = current_thread;
					tuple->tid_required = tid;
					int blocking = TRUE;
					getcontext(&current_thread->context);
					if (blocking) {
						int error = AppendFila2(&g_joinings, (void*)tuple);
						if (error != 0) {
							fprintf(stderr, "Error joining threads. Couldn't append JOIN_TUPLE to g_joining at cjoin.\n");
							free(tuple);
							return -9;
						}
						blocking = FALSE;
						current_thread->state = PROCST_BLOQ;
						setcontext(&sched_context);
					}
					free(tuple);
					return 0;
					// if dispatcher is correct there's no need for current_thread->state = PROCST_EXEC;
				} else {
					fprintf(stderr, "Error joining threads. Couldn't allocate memory for JOIN_TUPLE at cjoin.\n");
					return -9;
				}
					
			}
		} while(NextFila2(&thread_queue) == 0 && found == FALSE);
	}
	
	// No thread with the given tid
	return -9;
}

int csem_init(csem_t *sem, int count) {
    CHECK_INIT;
    if (sem && count > 0) {
		sem->count = count;
		sem->fila = malloc(sizeof(FILA2));
		int error = CreateFila2(sem->fila);
		if (error != 0) {
			fprintf(stderr, "Error creating semaphor. Couldn't create queue at csem_init.\n");
			return -9;
		}
	} else {
		fprintf(stderr, "Error creating semaphor. Invalid parameter at csem_init.\n");
		return -9;
	}
	return 0;
}

int cwait(csem_t *sem) {
    CHECK_INIT;
    if (sem) {
		sem->count -= 1;
		if (sem->count < 0) {
			int blocking = TRUE;
			getcontext(&current_thread->context);
			if (blocking) {
				current_thread->state = PROCST_BLOQ;
				if (AppendFila2(sem->fila, current_thread) != 0) {
					fprintf(stderr, "Error requesting resource. Couldn't append process to queue at cwait.\n");
					return -9;
				}
				blocking = FALSE;
				setcontext(&sched_context);
			} else {
				current_thread->state = PROCST_EXEC;
			}
		}
	} else {
		fprintf(stderr, "Error requesting resource. Invalid semaphor at cwait.\n");
		return -9;
	}
	return 0;
}

int csignal(csem_t *sem) {
    CHECK_INIT;
    if (sem) {
		sem->count += 1;
		if (sem->count <= 0) {
			int r = FirstFila2(sem->fila);
			if (r != 0) {
				fprintf(stderr, "Error signaling resource release. Couldn't retrieve process from queue at csignal.\n");
				return -9;
			}
			TCB_t *tcb_it, *prospect_tcb = NULL;
			unsigned int current_prio, highest_prio = UINT_MAX; /* TODO: This constant could be safer */
			do {
            /* Select a thread from queue */
				tcb_it = (TCB_t*) GetAtIteratorFila2(sem->fila);
				current_prio = tcb_it->prio;

				/* Check for priority. Maintain first highest priority found */
				if(current_prio < highest_prio) {
						prospect_tcb = tcb_it;
						highest_prio = current_prio;
				}
			} while(NextFila2(sem->fila) == 0);
			
			r = FirstFila2(sem->fila);
			if (r != 0) {
				fprintf(stderr, "Error signaling resource release. Couldn't retrieve process from queue at csignal.\n");
				return -9;
			}

			do {
            /* Select a thread from queue */
				tcb_it = (TCB_t*) GetAtIteratorFila2(sem->fila);
				if (tcb_it->tid == prospect_tcb->tid) {
					DeleteAtIteratorFila2(sem->fila);
					prospect_tcb->state = PROCST_APTO;
					return 0;
				}
			} while(NextFila2(sem->fila) == 0);
			
		}
	} else {
		fprintf(stderr, "Error signaling resource release. Invalid semaphor at csignal.\n");
		return -9;
	}
	return 0;
}

int cidentify (char *name, int size) {
    CHECK_INIT;
	strncpy (name, "Autores: \nFernando Correa Gomes -> 00264317\nIron Prando -> 00231590\n Nicolau Alff -> xxxxxxxx", size);
	return 0;
}


