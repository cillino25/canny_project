#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#define BASE_ACC_ADDRESS  0x40000000	//AXI accelerator base address
#define MATRIX_SIZE	  	  6				//Square matrix of size MATRIX_SIZE*MATRIX_SIZE
#define MEMORY_ADDRESS    0x1F000000	//Data starting address
#define POLL_ADDRESS      0x1F100000	//Polling variable address
#define BLEN			  3				//Burst_Length - 1 (data range must be a multiple of Burst_Length)

int main()
{
	// virual addresses returned by mmap
	void *HWacc_mmap;
	void *mem_mmap;
	void *poll_mmap;

	// file descriptors to manage /dev/mem file
	int fd_cpy, fd_HWacc, fd_poll;

	unsigned int page_size = sysconf(_SC_PAGESIZE);

	unsigned int range = MATRIX_SIZE*MATRIX_SIZE;
	unsigned int blen = BLEN;
	unsigned int matrix[MATRIX_SIZE][MATRIX_SIZE];
	unsigned int poll_var = 0;

	int i = 0;

	printf("size of unsigned int: %d\n", sizeof(unsigned int));
	printf("page size: %d\n", page_size);
    /*======================================================================================
    Initialize the matrix
    ========================================================================================*/
	for (i = 0; i < range; i++)
	{
		*(matrix[0]+i) = i;
	}

    printf("TEST HW ACCELERATOR \n");
 	/*======================================================================================
  	Map the kernel memory location starting from MEMORY_ADDRESS to the User layer
  	========================================================================================*/
  	fd_cpy = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd_cpy == -1)
    {
    	printf("Can't open /dev/mem.\n");
        exit(0);
    }

    printf("/dev/mem opened.\n");

    // Map one page of memory into user space such that the device is in that page, but it may not
    // be at the start of the page.
    mem_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cpy, MEMORY_ADDRESS);
	if (mem_mmap == (void *) -1)
	{
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	printf("Mem mmap Good... \r\n");
	printf("Memory mapped at address %p.\n", mem_mmap);


	/*======================================================================================
	Copy the Data to the DDR Memory at location MEMORY_ADDRESS
 	========================================================================================*/

	memcpy(mem_mmap, matrix[0], range*4);

	/*================================================================================================
	Map the polling variable register to the User layer.
	==================================================================================================*/
	fd_poll = open("/dev/mem", O_RDWR | O_SYNC);

	if (fd_poll == -1)
	{
		printf("Can't open /dev/mem.\n");
		exit(0);
	}

	printf("/dev/mem opened.\n");

	// Map one page of memory into user space such that the device is in that page, but it may not
	// be at the start of the page.
	poll_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_poll, POLL_ADDRESS);
	if (poll_mmap == (void *) -1)
	{
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	printf("Poll mmap Good... \r\n");
	printf("Memory mapped at address %p.\n", poll_mmap);

	memcpy(poll_mmap, &poll_var, 4);

	/*================================================================================================
	Map the HW accelerator Register memory to the User layer. Do the Register Setting for HW transfer
	==================================================================================================*/
	fd_HWacc = open("/dev/mem", O_RDWR | O_SYNC);

	if (fd_HWacc == -1)
	{
		printf("Can't open /dev/mem.\n");
		exit(0);
	}

	printf("/dev/mem opened.\n");

	// Map one page of memory into user space such that the device is in that page, but it may not
	// be at the start of the page.
	HWacc_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_HWacc, BASE_ACC_ADDRESS);
	if (HWacc_mmap == (void *) -1)
	{
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	printf("HWacc mmap Good... \r\n");
	printf("Memory mapped at address %p.\n", HWacc_mmap);

	// Print matrix values and polling variable status
	printf("Values in memory:\n");
	for (i = 0; i < range; i++)
	{
		printf("[%d]:  %8x	%p\n", i, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}
	printf("Polling variable: %8x   %p\n", *((volatile unsigned int *)poll_mmap), poll_mmap);

	// Set HW accelerator registers
	*((volatile unsigned int *)HWacc_mmap) = MEMORY_ADDRESS;		//First reg:  data starting address
	*((volatile unsigned int *)HWacc_mmap+1) = range;				//Second reg: data range
	*((volatile unsigned int *)HWacc_mmap+2) = POLL_ADDRESS;		//Third reg:  polling variable address
	*((volatile unsigned int *)HWacc_mmap+3) = blen;				//Forth reg:  burst length

	// Print accelerator register values
	printf("\nmem_add %x\n", *((volatile unsigned int *)HWacc_mmap));
	printf("range %x\n", *((volatile unsigned int *)HWacc_mmap+1));
	printf("poll_add %x\n", *((volatile unsigned int *)HWacc_mmap+2));
	printf("blen %x\n", *((volatile unsigned int *)HWacc_mmap+3));

	printf("\nWait the end of HW computation...\n");
	
	// Wait the end of HW computation
	while(*((volatile unsigned int *)poll_mmap) != 1);

	printf("Polling variable: %8x   %p\n", *((volatile unsigned int *)poll_mmap), poll_mmap);
	printf("End of computation...\n");

	printf("Updated values in memory:\n");
	for (i = 0; i < range; i++)
	{
		printf("[%d]:  %8x	%p\n", i, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}

	/*======================================================================================
	Un-map the HW accelerator and polling memory from the User layer.
	========================================================================================*/
	if (munmap(poll_mmap, page_size) == -1)
	{
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	close(fd_poll);

	if (munmap(HWacc_mmap, page_size) == -1)
	{
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	close(fd_HWacc);

	/*======================================================================================
	Un-map the kernel memory from the User layer.
 	========================================================================================*/
	if (munmap(mem_mmap, page_size) == -1) {
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	close(fd_cpy);

	printf("BYE! \r\n");
    return 0;
}
