#ifndef _SCHED_H
#define _SCHED_H

#include "../include/cdata.h"  // JOIN_TUPLE
#include "../include/support.h"  // g_joinings

void schedule();

FILA2 g_joinings;
typedef struct s_join_tuple {
	TCB_t* requirer;
	int tid_required;
} JOIN_TUPLE;

#endif
