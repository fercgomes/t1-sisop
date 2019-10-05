#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include "../include/support.h"
#include "../include/cdata.h"

TCB_t* thread_lookup(unsigned int tid);
void print_queue(PFILA2 queue);
int compareFiles (FILE* fp1, FILE* fp2);
#endif
