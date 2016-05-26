#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"


int main() {
  int h, j, i;
  vdma_handle handle;

  int len = PIXEL_CHANNELS * IN_FRAME_WIDTH * IN_FRAME_HEIGHT;
  int mem;
  unsigned int *read_fb;

  unsigned int page_size = sysconf(_SC_PAGESIZE);

  printf("-d:VDMA test.\n");
  // Setup VDMA handle and memory-mapped ranges
  //int vdma_setup(vdma_handle *handle, unsigned int page_size, unsigned int baseAddr, int width, int height, int pixelChannels, unsigned int fb1Addr, unsigned int fb2Addr, unsigned int fb3Addr);
  printf("Open AXI VDMA peripheral with\n");
  printf("  size=       %d\n", page_size);
  printf("  baseAddr=   0x%x\n", AXI_VDMA_BASEADDR);
  printf("  width=      %d\n", IN_FRAME_WIDTH);
  printf("  height=     %d\n", IN_FRAME_HEIGHT);
  printf("  channels=   %d\n", PIXEL_CHANNELS);
  printf("  fb1Addr=    0x%x\n", MEM2VDMA_BUFFER1_BASEADDR);
  printf("  fb2Addr=    0x%x\n", MEM2VDMA_BUFFER2_BASEADDR);
  printf("  fb3Addr=    0x%x\n", MEM2VDMA_BUFFER3_BASEADDR);
  printf("\n");


  vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, IN_FRAME_WIDTH, IN_FRAME_HEIGHT, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR);
  

  if((mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  read_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem, VDMA2MEM_BUFFER2_BASEADDR);
  if(((unsigned int *)read_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }


  vdma_set(&handle, OFFSET_VDMA_MM2S_HSIZE, 512);
  printf("Reading from VDMA_MM2S_HSIZE: %d\n", vdma_get(&handle, OFFSET_VDMA_MM2S_HSIZE));
  

  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
    
  // Start triple buffering
  vdma_start_triple_buffering(&handle);
  

  

  vdma_cmp_buffer(&handle, read_fb, 0xA0A0A0A0);

  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
  
  //sleep(1);
  
  // Run for 10 seconds, just monitor status registers
  
  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
  printf("\n");
  sleep(1);

  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
  printf("\n");

  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
  printf("\n");

  sleep(2);

  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
  printf("\n");

  
  
  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
  printf("\n");
  /*
  for(i=0; i<10; i++) {
  	printf("-d:%d\n", i);
    vdma_s2mm_status_dump(&handle);
    vdma_mm2s_status_dump(&handle);
    printf("FB1:\n");
    for(j=0; j<IN_FRAME_HEIGHT; j++){
    	for(h=0; h<IN_FRAME_WIDTH; h++){
    		printf(" %02x", ((unsigned char *)handle.fb1VirtualAddress)[j]); printf("\n");
    	}
    }
    sleep(1);
  }
  */

  // Halt VDMA and unmap memory ranges
  vdma_halt(&handle);

  printf("Bye!\n");
}
