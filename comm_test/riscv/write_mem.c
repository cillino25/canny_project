#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#define BASE_ACC_ADDRESS  0x20000000	//AXI accelerator base address
#define MATRIX_SIZE	  	  6				//Square matrix of size MATRIX_SIZE*MATRIX_SIZE
//#define MEMORY_ADDRESS    0x1F400000	//Data starting address
//#define POLL_ADDRESS      0x1F500000	//Polling variable address
#define BLEN			  1				//Burst_Length - 1 (data range must be a multiple of Burst_Length)

int main(int argc, char **argv)
{
	printf("Hello there!\n");
	void *mem_mmap;

	int fd_cpy;


	unsigned int page_size = sysconf(_SC_PAGESIZE);

	unsigned int range = MATRIX_SIZE*MATRIX_SIZE;
	unsigned int blen = BLEN;
	unsigned int matrix[MATRIX_SIZE][MATRIX_SIZE];
	unsigned int poll_var = 0;

	unsigned int mem_address = 0;
	unsigned int poll_address = 0;

	unsigned int custom_range = range;
	unsigned int custom_offset = 0;


	int i = 0;

	if(argc < 2){
		printf("Parameters must be at least 2\n");
		exit(1);
	}
	mem_address = atoi(argv[1]);

	if(argc == 3){
		custom_range = atoi(argv[2]);
	}
	if(argc == 4){
		custom_offset = atoi(argv[3]);
	}


	printf("size of unsigned int: %d\n", sizeof(unsigned int));
	printf("page size: %d\n", page_size);

	for (i = 0; i < range; i++)	{
		*(matrix[0]+i) = i;
	}

  if ((fd_cpy = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
  	printf("Can't open /dev/mem.\n");
    exit(0);
  }

  printf("/dev/mem opened.\n");


  mem_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cpy, mem_address);
	
	if (mem_mmap == (void *) -1){
		printf("Can't map the memory to user space.\n");
		exit(0);
	}
	
	//memcpy(mem_mmap, matrix[0], range*4);

	for(i=0; i<custom_range; i++)
		*((volatile unsigned int *) mem_mmap+i) = i;
	
	//memcpy(mem_mmap, matrix, custom_range*4);

	printf("Memory written:\n");

	for (i = 0; i < range; i++){
		printf("[%d]:  %8x	%p\n", i, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}

	*((unsigned int *)mem_mmap + custom_offset) = 255;


	printf("\n\nAfter single writes:\n");
	for (i = 0; i < range; i++){
		printf("[%d]:  %8x	%p\n", i, *((volatile unsigned int *)mem_mmap+i), mem_mmap+i);
	}

	

	printf("End of computation...\n");
	
	
	
	if (munmap(mem_mmap, page_size) == -1) {
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	close(fd_cpy);

	printf("BYE! \r\n");
  return 0;
}
