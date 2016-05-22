#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"

int main(){
	int h, j, i;
  vdma_handle handle;

  int len = PIXEL_CHANNELS * IN_FRAME_WIDTH * IN_FRAME_HEIGHT;
  int mem;
  unsigned int *write_fb;

  unsigned int page_size = sysconf(_SC_PAGESIZE);

  printf("-d:VDMA test.\n");
  
  // open /dev/mem for writing a buffer
  if((mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  write_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem, MEM2VDMA_BUFFER1_BASEADDR);
  if(((unsigned int *)read_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, IN_FRAME_WIDTH, IN_FRAME_HEIGHT, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR);
  
  vdma_start_triple_buffering(&handle);

  

}