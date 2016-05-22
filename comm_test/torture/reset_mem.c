#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#define ACC_BASE_R 1073741824
#define ACC_BASE_V 536870912

#define ACC_BASE ACC_BASE_R

#define ACC_RANGE 16

int main(int argc, char ** argv){
	unsigned int page_size = sysconf(_SC_PAGESIZE);

	void *mem_mmap;
	int fd_mem;

	unsigned int mem_address = ACC_BASE;
	unsigned int offset = 0;
	unsigned int i=0;

	if(argc == 2)
		mem_address = atoi(argv[1]);

	printf("Debug: resetting memory at %x\n", mem_address);

	if((fd_mem = open("/dev/mem", O_RDWR|O_SYNC)) == -1){
		printf("Error opening /dev/mem.\n");
		exit(1);
	}

	if((mem_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, mem_address)) == (void*) -1){
		printf("Error mapping memory...\n");
		exit(1);
	}

	// Reset the memory slave
	for(i=0; i<ACC_RANGE; i++)
		*((volatile unsigned int *)mem_mmap+i) = 0;


	if(munmap(mem_mmap, page_size) == -1){
		printf("Error unmapping memory..\n");
		exit(1);
	}
	close(fd_mem);

	//printf("Memory written:\n");
	//for(i=offset; i<range+offset; i++)
		//printf("[%x]  %d\n", mem_add+i, *(mem_add+i));

	//printf("Terminate..\n");

	return 0;
}