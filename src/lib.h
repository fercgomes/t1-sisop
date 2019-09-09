#ifndef _LIB_H
#define _LIB_H

#include "../include/support.h"
#include "../include/cdata.h"

#define TSTACKSIZE 64000

/* TID generator */
static int current_tid = 1;
static inline int make_tid() {
	return current_tid++;
}

#endif
