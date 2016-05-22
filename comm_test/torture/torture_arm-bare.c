#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "dataset.h"


void * acc_mmap;
pid_t pid;
int status = 0;
int error = 0;

int reset_accelerator();

int read_mem(unsigned int * mem, int n);

int main(int argc, char ** argv){
	unsigned int * base_add = (unsigned int *)1073741824;

	int fd_mem;
	void *poll_mmap_1, *poll_mmap_2;

	if(argc == 2)
		base_add = (unsigned int *)atoi(argv[1]);


	printf("Launching torture test on 16-registers accelerator at %x.\n", base_add);
	if(reset_accelerator()){
		printf("Error resetting accelerator\n");
	}

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
	if((acc_mmap = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, ACC_ADDRESS_ARM)) == (void*) -1){
		printf("Error mapping memory for accelerator at %x...\n", ACC_ADDRESS_ARM);
		exit(1);
	}
	// Reset polling variable
	*((volatile unsigned int *)poll_mmap_1) = 0;
	//*((volatile unsigned int *)poll_mmap_2) = 0;


	if((pid = fork()) < 0){
		printf("Fork error. Terminating\n");
		return(1);
	}else if(pid == 0){ // child process	
	  execl("/home/root/mnt/torture/fesvr-zynq", "fesvr-zynq", "torture_test_riscv-bare", NULL); // Bare-metal torture
		exit(0);
	}else{	// parent process
		//wait(&status);		// we're not waiting for the process end, but for the rocket to change the polling variable
		int i=0;

		for(i=0; i<N_TESTS; i++){
			//printf("--d_arm: waiting for rc to write memory for test %d..\n", i);

			//while((*((volatile unsigned int *) poll_mmap_1) == 0)&&(*((volatile unsigned int *) poll_mmap_2) == 0));
			while((*(volatile unsigned int *) poll_mmap_1 ) == 0);
			//printf("HEllo!\n");
			//printf("--d_arm: polling variable changed.\n");

			if(read_mem(test, i)){
				//printf("--d_arm: error reading test %d\n", i);
				error++;
			}else{
				//printf("--d_arm: test %d passed\n", i);
			}
			//printf("--d_arm: changing polling variable..\n");
			*((volatile unsigned int *)poll_mmap_1) = 0;
			//*((volatile unsigned int *)poll_mmap_2) = 0;
			
			//printf("--d_arm: mem[%x] = %d, ", POLL_ADDRESS_1, *((volatile unsigned int *)poll_mmap_1));
			//printf("mem[%x] = %d\n", POLL_ADDRESS_2, *((volatile unsigned int *)poll_mmap_2));
			//__clear_cache(poll_mmap, poll_mmap+32);
			
		}

		if(error){
			printf("\n\n%d test(s) not passed.\n", error);
		}else{
			printf("\n\nAll %d tests passed.\n", N_TESTS);
		}

		wait(&status);
	}
	
	

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

int reset_accelerator(){
	if((pid = fork()) < 0){
		printf("Fork error. Terminating..\n");
		return(1);
	}else if(pid == 0){ // child process
		execl("/home/root/mnt/torture/reset_mem_arm", "reset_mem_arm", NULL);

		//printf("Memory reset\n");
		exit(0);
	}else{
		wait(&status);
		if(status) printf("Error in the child process\n");
	}
}


int read_mem(unsigned int * mem, int n){
	int i = 0;
	int row = n * MEM_SIZE;
	unsigned int tmp = 0;

	for(i=0; i<MEM_SIZE; i++){
		tmp = *((volatile unsigned int *) acc_mmap + i);
		if(mem[row + i] != tmp){
			printf("Error comparing memory with test %d vector at position %d:\n", n, i);
			printf("\t==>  mem[%d] =  %x != %x  = acc_mmap+%d\n", i, mem[row + i], tmp, i);
			return(1);
		}
	}
	return 0;
}