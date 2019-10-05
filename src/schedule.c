/*
    THREAD SCHEDULER
    Scheduling policy: non-preemptive by priority (executing time)
*/

#include <stdio.h>
#include "schedule.h"
#include "../include/cthread.h"
#include "lib.h"
#include <ucontext.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

extern FILA2 thread_queue;
extern TCB_t* current_thread;
extern ucontext_t sched_context;

void cleanup();

int g_finished = TRUE;

static TCB_t* get_next_thread() {
    TCB_t *tcb_it, *prospect_tcb = NULL;
    unsigned int current_prio, highest_prio = UINT_MAX; /* TODO: This constant could be safer */
	g_finished = TRUE;
	
    int r = FirstFila2(&thread_queue);

    /* check if queue is empty */
    if(r == 0)
        do {
            /* Select a thread from queue */
            tcb_it = (TCB_t*) GetAtIteratorFila2(&thread_queue);
            current_prio = tcb_it->prio;

            /* Check for priority */
            if(tcb_it->state == PROCST_APTO) {
                if(current_prio < highest_prio) {
                    prospect_tcb = tcb_it;
                    highest_prio = current_prio;
                    g_finished = FALSE;
                }
            } else if (tcb_it->state == PROCST_EXEC || tcb_it->state == PROCST_BLOQ) {
				g_finished = FALSE;
			}
        } while(NextFila2(&thread_queue) == 0);

    return prospect_tcb;
}

/* Starts the execution of a new context(thread) */
void dispatch(TCB_t* tcb) {
    tcb->state = PROCST_EXEC;
    current_thread = tcb;

    /* Effectively transfer control to selected thread */
    setcontext(&tcb->context);
}

/* Selects a thread to execute and dispatches it */
void schedule() {
    TCB_t* next_thread;
    volatile int sched_ready = 1;
    unsigned int elapsed_time;
	
    while(1) {
        /* Selects the next thread to execute */
        next_thread = get_next_thread();    

        if(next_thread) {
            /* Save current context */
            getcontext(&sched_context);

            if(sched_ready) {
                printf("+ [SCHEDULER] Dispatching thread tid=%d\n",next_thread->tid);
                sched_ready = 0;
                startTimer();
                dispatch(next_thread);
            } else {
                /* Thread has returned to scheduler */
                printf("+ [SCHEDULER] Returning from thread tid=%d\n", next_thread->tid);
                elapsed_time = stopTimer();
                next_thread->prio = elapsed_time;

                /* Check state */
                if(next_thread->state == PROCST_APTO) {
                    /* Thread has yielded */
                }
                else if(next_thread->state == PROCST_EXEC) {
                    /* Thread has finished */
                    next_thread->state = PROCST_TERMINO;
                    
                    // Check if the tid is being waited by another thread
                    JOIN_TUPLE* tuple = NULL;
					int r = FirstFila2(&g_joinings);
					int found = FALSE;

					if (r == 0) {
						do {
							tuple = (JOIN_TUPLE*) GetAtIteratorFila2(&g_joinings);
							if(tuple->tid_required == next_thread->tid) {
								tuple->requirer->state = PROCST_APTO;
								found = TRUE;
								DeleteAtIteratorFila2(&g_joinings);						
							}
						} while(NextFila2(&g_joinings) == 0 && found == FALSE);
					} else {
					}
                }
                else if(next_thread->state == PROCST_BLOQ) {
                    /* Thread was blocked */
                }

                sched_ready = 1;
            }
        } else if (g_finished) {
            /* No more threads to execute */ 
            printf("+ [SCHEDULER] No more ready threads.\n");
            cleanup();
            return;
        }
    }
}
