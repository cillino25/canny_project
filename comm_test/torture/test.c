#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "dataset.h"


volatile unsigned int * base_add = (volatile unsigned int *)536870912;

int test[16] = {55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void write_mem(int * mem){
	int i = 0;
	for(i=0; i<MEM_SIZE; i++){
		if(mem[i] != 0){
			printf("mem[%d]=%x\n", i, mem[i]);
			//printf("Hello world!\n");
		}
	}
}

int main(){

	write_mem(test1);

	printf("-d:Changing polling variable at %x.\n", POLL_ADDRESS);
	//*((volatile unsigned int *) POLL_ADDRESS) = 1;

	//polling variable to proceed!

	printf("Terminating Rocketchip execution..\n");

	return 0;

}
