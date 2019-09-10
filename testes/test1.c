#include <stdio.h>
#include "../include/cthread.h"

void print_queue(PFILA2 queue);

void* test_func(void *args) {
    printf("test_func\n"); 
    printf("args = %d\n", *(int*)args);
}

void* test_func2(void *args) {
    printf("test_func2\n"); 
    printf("args = %d\n", *(int*)args);
    cyield();
    printf("back from yield tf2\n");
}

int main() {
    int a = 4;

    ccreate(test_func, &a, 0);
    //ccreate(test_func2, &a, 0);

    cyield();

    print_queue(NULL);

    return 0;
}
