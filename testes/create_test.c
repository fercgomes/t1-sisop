#include <stdio.h>
#include <stdlib.h>
#include "../include/cthread.h"

#define LIMIT 100
#define DATA_SIZE 15

FILE* fp1 = NULL;
FILE* fp_ground_truth = NULL; 

void* create_test(void* i)
{
	int j = *((int*) i);
	char data[DATA_SIZE];
	snprintf(data,DATA_SIZE,"t%d\n", j);
	fputs(data, fp1);
	if (ferror(fp1)) {
		printf("Unexpected error occurred while writing on file. Exiting with error\n");
		exit(-9);
	}
}

int main() {
    int i = 1;
    int values[LIMIT] = {0};
    char data_main[] = "t0\n";
    
    fp1 = fopen("generated_data/create_test_data.txt", "w+");
    fp_ground_truth = fopen("ground_truth/groud_truth_create_test.txt", "r");
    
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
		values[i] = i;
		int tcb = ccreate(create_test, &(values[i]), 0);
		if (tcb < 0)
			printf("Error creating TCB\n");
	}
	
    cyield();
	
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
		printf("Create Test Failed\n");
	} else {
		printf("Create Test Succeeded\n");
	}
    return error;
}
