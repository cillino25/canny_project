#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ACC_BASE_R 1073741824
#define ACC_BASE_V 536870912

int main(int argc, char ** argv){

	unsigned int range = 16;
	unsigned int offset = 0;
	unsigned int * mem_add = (unsigned int *)ACC_BASE_V;
	unsigned int i=0;

	//printf("Resetting accelerator memory at %x\n", mem_add);

	// Reset the memory slave
	for(i=0; i<16; i++)
		*((volatile unsigned int *)mem_add+i) = 0;

	//printf("Memory written:\n");
	//for(i=offset; i<range+offset; i++)
		//printf("[%x]  %d\n", mem_add+i, *(mem_add+i));

	//printf("Terminate..\n");

	return 0;
}