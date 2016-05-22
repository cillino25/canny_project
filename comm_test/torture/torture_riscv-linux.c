#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "dataset.h"


void * acc_mmap;

void write_mem(unsigned int * test_matrix, int n);

int main(int argc, char ** argv){
	unsigned int * base_add = (unsigned int *)ACC_ADDRESS_RC;
	int i = 0;

	int fd_mem;
	void *poll_mmap_1, *poll_mmap_2;

	if(argc == 2)
		base_add = (unsigned int *)((int)atoi(argv[1]));

	/***********************************************************************/
	// Init

	if((fd_mem = open("/dev/mem", O_RDWR|O_SYNC)) == -1){
		printf("Error opening /dev/mem.\n");
		exit(1);
	}
	if((poll_mmap_1 = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, fd_mem, POLL_ADDRESS_1)) == (void*) -1){
		printf("Error mapping memory for polling variable at %x...\n", POLL_ADDRESS_1);
		exit(1);
	}
	if((poll_mmap_2 = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, fd_mem, POLL_ADDRESS_2)) == (void*) -1){
		printf("Error mapping memory for polling variable at %x...\n", POLL_ADDRESS_1);
		exit(1);
	}
	if((acc_mmap = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, ACC_ADDRESS_RC)) == (void*) -1){
		printf("Error mapping memory for accelerator at %x...\n", ACC_ADDRESS_RC);
		exit(1);
	}
	
	/************************************************************************/
	// Main program
	for(i=0; i<N_TESTS; i++){
		//printf("-d_rc: writing test %d\n", i);
		write_mem(test, i);

		//printf("-d_rc: Changing polling variables at %x and %x..\n", POLL_ADDRESS_1, POLL_ADDRESS_2);
		//*((volatile unsigned int *) POLL_ADDRESS_1) = 1;
		//*((volatile unsigned int *) POLL_ADDRESS_2) = 1;
		*((volatile unsigned int *) poll_mmap_1) = 1;
		*((volatile unsigned int *) poll_mmap_2) = 1;


		//printf("-d_rc:mem[%x] = %x, ", POLL_ADDRESS_1, *((volatile unsigned int *) POLL_ADDRESS_1));
		//printf("mem[%x] = %x\n", POLL_ADDRESS_2, *((volatile unsigned int *) POLL_ADDRESS_2));


		//printf("-d_rc: Waiting for ARM to validate memory...\n");
		//while((*((volatile unsigned int *) POLL_ADDRESS_1) == 1)&&(*((volatile unsigned int *) POLL_ADDRESS_2) == 1));
		
		//With single variable: it doesn't work!
		//while((*(volatile unsigned int *) POLL_ADDRESS_1) == 1);

	}



	
	/***************************************************************************************************/
	// Exiting procedure

	close(fd_mem);
	
	if (munmap(poll_mmap_1, sysconf(_SC_PAGESIZE)) == -1) {
		printf("Can't unmap polling memory from user space.\n");
		exit(0);
	}
	if (munmap(poll_mmap_2, sysconf(_SC_PAGESIZE)) == -1) {
		printf("Can't unmap polling memory from user space.\n");
		exit(0);
	}
	if (munmap(acc_mmap, sysconf(_SC_PAGESIZE)) == -1) {
		printf("Can't unmap accelerator memory from user space.\n");
		exit(0);
	}

	printf("Terminating execution on ARM.\n\n");
	return 0;

}


void write_mem(unsigned int * test_matrix, int n){
	int i = 0;
	int row = n*MEM_SIZE;
	for(i=0; i<MEM_SIZE; i++){
		//if(test_matrix[row + i] != 0){
			//printf("test_matrix[%d][%d]=%x\n", n, i, test_matrix[row + i]);
			//*((volatile unsigned int *) ACC_ADDRESS_RC+i)=test_matrix[row + i];	// Bare-metal write
			*((volatile unsigned int *) acc_mmap+i)=test_matrix[row + i];	// OS write
		//}
	}
}