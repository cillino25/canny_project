#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#define BASE_ACC_ADDRESS  0x40000000	//AXI accelerator base address
#define POLL_ADDRESS      0x1F000000	//Polling variable address

int main(int argc, char **argv)
{
	// virual addresses returned by mmap
	void *mem_mmap;

	// file descriptors to manage /dev/mem file
	int fd_mem;

	unsigned int page_size = sysconf(_SC_PAGESIZE);


	unsigned int mem_address = POLL_ADDRESS;


	int i = 0;

	if(argc == 2){
		mem_address = atoi(argv[1]);
	}

  if ((fd_mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
  	printf("Can't open /dev/mem.\n");
    exit(0);
  }

  mem_mmap = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, mem_address);
	if (mem_mmap == (void *) -1){
		printf("Can't mmap.\n");
		exit(0);
	}

	*((volatile unsigned int *) mem_mmap) = 1;

	if (munmap(mem_mmap, page_size) == -1)	{
		printf("Can't unmap memory.\n");
		exit(0);
	}

	close(fd_mem);
  return 0;
}
