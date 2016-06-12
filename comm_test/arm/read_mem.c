#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#define BASE_ACC_ADDRESS  0x40000000	//AXI accelerator base address
#define MATRIX_SIZE	  	  6				//Square matrix of size MATRIX_SIZE*MATRIX_SIZE
//#define MEMORY_ADDRESS    0x0F000000	//Data starting address
//#define POLL_ADDRESS      0x0F100000	//Polling variable address
#define BLEN			  3				//Burst_Length - 1 (data range must be a multiple of Burst_Length)


int main(int argc, char **argv)
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

	unsigned int mem_address = 0;

	int i = 0;

	if(argc < 2){
		printf("Parameters must be at least 2:\n./read_mem <address> <range>\n\n");
		exit(1);
	}

	mem_address = strtoul(argv[1], NULL, 0);
	printf("mem_address is 0x%x\n", mem_address);

	if(argc>=3){
		range = strtoul(argv[2], NULL, 0);
	}
	printf("range is 0x%x\n", range);

	printf("size of unsigned int: %ld\n", sizeof(unsigned int));
	printf("page size: %d\n", page_size);
  /*======================================================================================
  Initialize the matrix
  ========================================================================================*/

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
  mem_mmap = mmap(NULL, 10*1024*1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cpy, mem_address);
	if (mem_mmap == (void *) -1)
	{
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	printf("Mem mmap Good... \r\n");
	printf("Memory mapped at address %p.\n", mem_mmap);


	printf("Values in memory (addresses are in byte):\n");
	for (i = 0; i < range; i++)
	{
		printf("[0x%x]:  %8x	%p\n", i*4, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}


	
	
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

