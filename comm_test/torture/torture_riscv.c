#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "dataset.h"


void write_mem(unsigned int * test_matrix, int n);


int main(){
	int i=0, j=0;

	for(i=0; i<N_TESTS; i++){
		//printf("-d_rc: writing test %d\n", i);
		write_mem(test, i);

		//printf("-d_rc: Changing polling variables at %x and %x..\n", POLL_ADDRESS_1, POLL_ADDRESS_2);
		*((volatile unsigned int *) POLL_ADDRESS_1) = 1;
		*((volatile unsigned int *) POLL_ADDRESS_2) = 1;
		
		//printf("-d_rc:mem[%x] = %x, ", POLL_ADDRESS_1, *((volatile unsigned int *) POLL_ADDRESS_1));
		//printf("mem[%x] = %x\n", POLL_ADDRESS_2, *((volatile unsigned int *) POLL_ADDRESS_2));


		//printf("-d_rc: Waiting for ARM to validate memory...\n");
		while((*((volatile unsigned int *) POLL_ADDRESS_1) == 1)&&(*((volatile unsigned int *) POLL_ADDRESS_2) == 1));
		
		//With single variable: it doesn't work!
		//while((*(volatile unsigned int *) POLL_ADDRESS_1) == 1);

	}

	//polling variable to proceed!

	printf("Terminating Rocketchip execution..\n");

	return 0;

}


void write_mem(unsigned int * test_matrix, int n){
	int i = 0;
	int row = n*MEM_SIZE;
	for(i=0; i<MEM_SIZE; i++){
		//if(test_matrix[row + i] != 0){
			//printf("test_matrix[%d][%d]=%x\n", n, i, test_matrix[row + i]);
			*((volatile unsigned int *) ACC_ADDRESS_RC+i)=test_matrix[row + i];
		//}
	}
}