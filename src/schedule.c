/*
    THREAD SCHEDULER
    Scheduling policy: non-preemptive by priority (executing time)
*/

#include <stdio.h>
#include "schedule.h"
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "lib.h"
#include <ucontext.h>
#include <limits.h>

extern FILA2 thread_queue;
extern TCB_t* current_thread;
extern ucontext_t sched_context;

void cleanup();

static TCB_t* get_next_thread() {
    TCB_t *tcb_it, *prospect_tcb = NULL;
    unsigned int current_prio, highest_prio = UINT_MAX; /* TODO: This constant could be safer */

    int r = FirstFila2(&thread_queue);

    /* check if queue is empty */
    if(r == 0)
        do {
            /* Select a thread from queue */
            tcb_it = (TCB_t*) GetAtIteratorFila2(&thread_queue);
            current_prio = tcb_it->prio;

            /* Check for priority */
            if(tcb_it->state == PROCST_APTO) {
                if(current_prio <= highest_prio) {
                    prospect_tcb = tcb_it;
                    highest_prio = current_prio;
                }
            }
        }
        while(NextFila2(&thread_queue) == 0);

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
            }
            else {
                /* Thread has returned to scheduler */
                printf("+ [SCHEDULER] Returning from thread tid=%d\n", next_thread->tid);
                elapsed_time = stopTimer();
                next_thread->prio += elapsed_time;

                /* Check state */
                if(next_thread->state == PROCST_APTO) {
                    /* Thread has yielded */
                }
                else if(next_thread->state == PROCST_EXEC) {
                    /* Thread has finished */
                    next_thread->state = PROCST_TERMINO;
                }
                else if(next_thread->state == PROCST_BLOQ) {
                    /* Thread was blocked */
                }

                sched_ready = 1;
            }
        }
        else {
            /* No more ready threads */ 
            printf("+ [SCHEDULER] No more ready threads.\n");
            cleanup();
            return;
        }
    }
}
