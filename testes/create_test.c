#include <stdio.h>
#include <stdlib.h>
#include "../include/cthread.h"

#define LIMIT 100
#define DATA_SIZE 10

FILE* fp1 = NULL;
FILE* fp_ground_truth = NULL; 

void* create_test(void* i)
{
	int j = *((int*) i);
	char data[DATA_SIZE];
	snprintf(data,DATA_SIZE,"c%d\n", j);
	fputs(data, fp1);
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
	}
}

int main() {
    int i = 0;
    int values[LIMIT] = {0};
    
    fp1 = fopen("data/create_test_data.txt", "w+");
    fp_ground_truth = fopen("data/groud_truth_create_test.txt", "r");
    
	if (!fp1 || !fp_ground_truth) 
	{
		printf("Error openning file. Exiting\n");
		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < LIMIT; i++)
	{
		printf("%d\n",i);
		values[i] = i;
		int tcb = ccreate(create_test, &(values[i]), 0);
		if (tcb < 0)
			printf("Error creating TCB\n");
	}
	
    cyield();
	
	int error = compareFiles(fp1, fp_ground_truth);
	
	fclose(fp1);
	fclose(fp_ground_truth);
	
    return error;
}
