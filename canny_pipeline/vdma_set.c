#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "global_parameters.h"
#include "vdma.h"

int main(int argc, char **argv)
{
	void *mem_mmap;
	int mem;
	unsigned int mem_address = AXI_VDMA_BASEADDR;
	int i = 0;

	int width=0, height=0, pixel_channels=0, fb_in=0, fb_out=0;

	vdma_handle vdma;

	if(argc < 6){
		printf("Parameters must be 5:\n./vdma_set <width> <height> <pixel_channels> <fb_in> <fb_out>\n\n");
		exit(1);
	}

	
	width = strtoul(argv[1], NULL, 0);
	height = strtoul(argv[2], NULL, 0);
	pixel_channels = strtoul(argv[3], NULL, 0);
	fb_in = strtoul(argv[4], NULL, 0);
	fb_out = strtoul(argv[5], NULL, 0);
	

	
	mem = open("/dev/mem", O_RDWR | O_SYNC);

  if (mem == -1){
  	printf("Can't open /dev/mem.\n");
    exit(0);
  }

  
  mem_mmap = mmap(NULL, BUFFER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem, mem_address);
	if (mem_mmap == (void *) -1)
	{
		printf("Can't map the memory to user space.\n");
		exit(0);
	}



	vdma.vdmaHandler = mem;
	
	printf("Setting VDMA (0x%x) with:\n", mem_address);
	printf("  width: %d\n", width);
	printf("  height: %d\n", height);
	printf("  pixel_channels: %d\n", pixel_channels);
	printf("  fb_in: 0x%x\n", fb_in);
	printf("  fb_out: 0x%x\n", fb_out);
	vdma_setup(&vdma, 4096, AXI_VDMA_BASEADDR, width, height, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER);
	vdma_setParams(&vdma, AXI_VDMA_BASEADDR, width, height, pixel_channels, BUFFER_SIZE, fb_in, 0, 0, fb_out, 0, 0);
	vdma_start_triple_buffering_mod(&vdma);

	
	
	if (munmap(mem_mmap, 4096) == -1) {
		printf("Can't unmap memory from user space.\n");
		exit(0);
	}

	close(mem);

	printf("BYE!\n");
  return 0;
}

