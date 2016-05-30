#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
	// virual addresses returned by mmap
	void *mem_mmap;

	// file descriptors to manage /dev/mem file
	int fd_mem;

	unsigned int page_size = sysconf(_SC_PAGESIZE);
	unsigned int mem_address = 0x1F000000;
	unsigned int val = 1;


	int i = 0;

	if(argc >= 2){
		mem_address = strtoul(argv[1], NULL, 0);
	}
	if(argc >= 3){
		val = strtoul(argv[2], NULL, 0);
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

	for(i=0; i<16; i++)
		((volatile unsigned int *) mem_mmap)[i] = val;

	printf("-Written 0x%x at 0x%x.\n", val, mem_address);

	if (munmap(mem_mmap, page_size) == -1)	{
		printf("Can't unmap memory.\n");
		exit(0);
	}

	close(fd_mem);
  return 0;
}
