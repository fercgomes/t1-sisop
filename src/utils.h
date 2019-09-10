#ifndef _UTILS_H
#define _UTILS_H

#include "../include/support.h"
#include "../include/cdata.h"

TCB_t* thread_lookup(unsigned int tid);
void print_queue(PFILA2 queue);

#endif
