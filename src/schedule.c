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

extern FILA2 thread_queue;

static TCB_t* get_next_thread() {
    TCB_t *tcb_it, *prospect_tcb = NULL;
    int current_prio, highest_prio = 0;

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
void dispatch(TCB_t* tcb, ucontext_t* sched_context) {
    /* Set return context to scheduler */
    tcb->context.uc_link = sched_context;
    // TODO: anything else??

    /* Effectively transfer control to selected thread */
    setcontext(&tcb->context);
}

/* Selects a thread to execute and dispatches it */
void schedule() {
    TCB_t* next_thread;
    volatile int sched_ready = 1;
    ucontext_t sched_context;

    while(1) {
        /* Save scheduler's context */
        getcontext(&sched_context);

        if(sched_ready) {
            sched_ready = 0;

            next_thread = get_next_thread();    
            if(next_thread != NULL) {
                printf("Dispatching thread tid=%d\n", next_thread->tid);
            
                dispatch(next_thread, &sched_context);
            }
        }
        else printf("Returning from thread\n");
    }
}
