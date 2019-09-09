#include <stdio.h>
#include "schedule.h"
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "lib.h"

static PNODE2* get_next_thread() {
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

/* TODO: schedule */
void schedule() {
    
}
