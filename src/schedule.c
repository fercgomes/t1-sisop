#include <stdio.h>
#include "schedule.h"
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "lib.h"

static TCB_t* get_next_thread() {
    TCB_t *tcb_it, *prospect_tcb = NULL;
    int current_prio, highest_prio = 0;

    FirstFila2(&thread_queue);
    /* check if queue is empty */
    if(thread_queue.first != NULL)
        do {
            /* Select a thread from queue */
            tcb_it = (TCB_t*) GetAtIteratorFila2(&thread_queue);
            current_prio = tcb_it->prio;

            /* Check for priority */
            if(tcb_it->state == PROCST_APTO) {
                if(current_prio < highest_prio) {
                    prospect_tcb = tcb_it;
                    highest_prio = current_prio;
                }
            }
        }
        while(NextFila2(&thread_queue) == 0);

    return prospect_tcb;
}

/* TODO: schedule */
void schedule() {
}
