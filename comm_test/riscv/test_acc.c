#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#define BASE_ACC_ADDRESS  0x20000000	//AXI accelerator base address
#define MATRIX_SIZE	  	  6				//Square matrix of size MATRIX_SIZE*MATRIX_SIZE
#define MEMORY_ADDRESS    0x1F400000	//Data starting address
#define POLL_ADDRESS      0x1F500000	//Polling variable address
#define BLEN			  1				//Burst_Length - 1 (data range must be a multiple of Burst_Length)

int main(int argc, char **argv)
{
	// virual addresses returned by mmap
	void *HWacc_mmap;
	void *mem_mmap;
	void *poll_mmap;

	// file descriptors to manage /dev/mem file
	int fd_mem;

	unsigned int page_size = sysconf(_SC_PAGESIZE);

	unsigned int range = MATRIX_SIZE*MATRIX_SIZE;
	unsigned int blen = BLEN;
	unsigned int matrix[MATRIX_SIZE][MATRIX_SIZE];
	unsigned int poll_var = 9;

	unsigned int mem_address = 0;
	unsigned int poll_address = 0;


	int i = 0;

	if(argc != 2){
		printf("Parameters must be 2\n");
		exit(1);
	}
	mem_address = atoi(argv[1]);
	poll_address = mem_address + 1048576;


	printf("size of unsigned int: %d\n", sizeof(unsigned int));
	printf("page size: %d\n", page_size);

	printf("mem_address: %p\n", mem_address);
	printf("range: %d\n", range);
	printf("poll_address: %p\n", poll_address);
	printf("blen: %d\n\n", blen);
  
	for (i = 0; i < range; i++)	{
		*(matrix[0]+i) = i;
	}

  if ((fd_mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
  	printf("Can't open /dev/mem.\n");
    exit(0);
  }

  printf("/dev/mem opened.\n");

  mem_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, mem_address);
	if (mem_mmap == (void *) -1){
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	printf("Scratchpad memory mapped at address %p.\nCopying data...\n", mem_mmap);

	memcpy(mem_mmap, matrix[0], range*4);

	printf("Data copied. Data in memory:\n");
	for (i = 0; i < range; i++){
		printf("[%d]:  %8x	%p\n", i, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}

	poll_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, poll_address);
	if (poll_mmap == (void *) -1){
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	
	printf("Polling variable mapped at address %p.\n", poll_mmap);

	memcpy(poll_mmap, &poll_var, 4);
	printf("Polling variable: %8x   %p\n\n", *((volatile unsigned int *)poll_mmap), poll_mmap);

	
	HWacc_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, BASE_ACC_ADDRESS);
	if (HWacc_mmap == (void *) -1){
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	printf("HWacc mapped at address %p.\n\n", HWacc_mmap);

	
	printf("Writing acc registers.\n");
	printf("mem_address:  write %p at address %p\n", mem_address, (unsigned int *)HWacc_mmap);
	printf("range:        write %d at address %p\n", range, (unsigned int *)HWacc_mmap+1);
	printf("poll_address: write %p at address %p\n", poll_address, (unsigned int *)HWacc_mmap+2);
	printf("blen:         write %d at address %p\n\n", blen, (unsigned int *)HWacc_mmap+3);


	/*
	unsigned int params[8];
	params[0] = mem_address;
	params[1] = range;
	params[2] = poll_address;
	params[3] = blen;
	params[4] = 0;
	params[5] = 0;
	params[6] = 0;
	params[7] = 0;

	memcpy(HWacc_mmap, params, 8*4);	
	*/

	*((volatile unsigned int *)HWacc_mmap) = mem_address;
	//*((volatile unsigned int *)HWacc_mmap) = MEMORY_ADDRESS;
	*((volatile unsigned int *)HWacc_mmap+1) = range;
	*((volatile unsigned int *)HWacc_mmap+2) = poll_address;
	//*((volatile unsigned int *)HWacc_mmap+2) = POLL_ADDRESS;
	*((volatile unsigned int *)HWacc_mmap+3) = blen;

	//memcpy(HWacc_mmap, &mem_address, 4);
	//memcpy(HWacc_mmap+1*4, &range, 4);
	//memcpy(HWacc_mmap+2*4, &poll_address, 4);
	//memcpy(HWacc_mmap+3*4, &blen, 4);

	// Print accelerator register values
	printf("\nmem_add   %x\n", *((volatile unsigned int *)HWacc_mmap));
	printf("range     %x\n", *((volatile unsigned int *)HWacc_mmap+1));
	printf("poll_add  %x\n", *((volatile unsigned int *)HWacc_mmap+2));
	printf("blen      %x\n", *((volatile unsigned int *)HWacc_mmap+3));

	printf("\nWait the end of HW computation...\n");
	
	// Wait the end of HW computation
	while(*((volatile unsigned int *)poll_mmap) != 1);


	printf("Polling variable: %8x   %p\n", *((volatile unsigned int *)poll_mmap), poll_mmap);
	printf("End of computation...\n");

	printf("Updated values in memory:\n");
	for (i = 0; i < range; i++){
		printf("[%d]:  %8x	%p\n", i, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}

	/*======================================================================================
	Un-mapping
	========================================================================================*/
	if (munmap(poll_mmap, page_size) == -1){
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	if (munmap(HWacc_mmap, page_size) == -1){
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	if (munmap(mem_mmap, page_size) == -1) {
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	close(fd_mem);

	printf("BYE! \r\n");
  return 0;
}
