#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"


int main(){
	unsigned int page_size = sysconf(_SC_PAGESIZE);
	
	int i;
	int mem;
  unsigned int *read_fb;
  int len = PIXEL_CHANNELS * IN_FRAME_WIDTH * IN_FRAME_HEIGHT;

  printf("-d:VDMA test - ARM.\n");
  
  // open /dev/mem for writing a buffer
  if((mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  read_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem, VDMA2MEM_BUFFER1_BASEADDR);
  if(((unsigned int *)read_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  while(cmp_buffer(read_fb, len, 0xdeadbeef) != 0){ i++; }
  printf("\n\nBuffer comparing OK (failed %d times)\n", i);

}