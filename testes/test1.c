#include <stdio.h>
#include "../include/cthread.h"
#include "ucontext.h"
#include "../src/schedule.h"

int cthread_init();
void print_queue(PFILA2 queue);
void get_next_thread();

void* test_func(void* args) {
   printf("test_func\n"); 
}

void* test_func2(void* args) {
   printf("test_func2\n"); 
}

int main() {
    if(cthread_init() == 0) {
        printf("+ cthread initialized.\n");
        int a = 4;

        ccreate(test_func, &a, 0);
        ccreate(test_func2, &a, 0);
        print_queue(NULL);

        schedule();
        print_queue(NULL);
    }
    else {
        fprintf(stderr, "+ Error initializing cthread.\n");
    }

    return 0;
}
