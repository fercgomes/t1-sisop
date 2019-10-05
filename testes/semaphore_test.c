#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/cthread.h"

#define LIMIT 5
#define SEMAPHOR_COUNT 2
#define DATA_SIZE 15


FILE* fp1 = NULL;
FILE* fp_ground_truth = NULL; 

typedef struct s_sem_val {
	int i;
	csem_t* sem;
} S_SEMAPHORE_TEST;

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )	
        now = clock();
}

void* semaphor_test(void* sem_test)
{
	S_SEMAPHORE_TEST j = *((S_SEMAPHORE_TEST*) sem_test);
	char data[DATA_SIZE];
	int i;
	
	snprintf(data,DATA_SIZE,"t\n", j.i);
	fputs(data, fp1);
	
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
		exit(-9);
	}
	
	cwait(j.sem);
	
	snprintf(data,DATA_SIZE,"it\n", j.i);
	fputs(data, fp1);
	
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
		exit(-9);
	}
	delay(LIMIT);
	
	cyield();
	
	snprintf(data,DATA_SIZE,"fit\n", j.i);
	fputs(data, fp1);
	
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
		exit(-9);
	}
	
	csignal(j.sem);
}

int main() {
    int i = 1;
    csem_t semaphor;
    S_SEMAPHORE_TEST values[LIMIT];
    char data_main[] = "t0\n";

    if (csem_init(&semaphor, SEMAPHOR_COUNT)) {
		printf("Couldn't create semaphor.\nSemaphor test Failed. Exiting\n");
		exit(-9);
	}
    
    fp1 = fopen("data/semaphore_test_data.txt", "w+");
    fp_ground_truth = fopen("data/groud_truth_semaphore_test.txt", "r");
    
	if (!fp1 || !fp_ground_truth) 
	{
		printf("Error openning file. Exiting\n");
		exit(EXIT_FAILURE);
	}
	
	fputs(data_main, fp1);
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
		exit(-9);
	}
	
	for (i = 1; i < LIMIT; i++)
	{
		values[i].i = i;
		values[i].sem = &semaphor;
		int tcb = ccreate(semaphor_test, &(values[i]), 0);
		if (tcb < 0)
			printf("Error creating TCB\n");
	}
	
	delay(LIMIT*10);
		
    cyield();
	
	cwait(&semaphor);
	csignal(&semaphor);
	
	fputs(data_main, fp1);
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
		exit(-9);
	}
	
	rewind(fp1);
	int error = compareFiles(fp1, fp_ground_truth);
	
	fclose(fp1);
	fclose(fp_ground_truth);
	
	if (error)
	{
		printf("Semaphore Test Failed\n");
	} else {
		printf("Semaphore Test Succeeded\n");
	}
    return error;
}
