#include <stdio.h>
#include "../include/cthread.h"

int cthread_init();
void print_queue(PFILA2 queue);

int main() {
    if(cthread_init() == 0) {
        printf("+ cthread initialized.\n");

        ccreate(NULL, NULL, 0);
        ccreate(NULL, NULL, 0);
        ccreate(NULL, NULL, 0);
        ccreate(NULL, NULL, 0);
        ccreate(NULL, NULL, 0);
        print_queue(NULL);
    }
    else {
        fprintf(stderr, "+ Error initializing cthread.\n");
    }

    return 0;
}