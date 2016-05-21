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
  void *va0, *va1, *va2, *va3;

  unsigned int page_size = sysconf(_SC_PAGESIZE);

  printf("-d:VDMA test.\n");
  // Setup VDMA handle and memory-mapped ranges
  //int vdma_setup(vdma_handle *handle, unsigned int page_size, unsigned int baseAddr, int width, int height, int pixelChannels, unsigned int fb1Addr, unsigned int fb2Addr, unsigned int fb3Addr);
  printf("Open AXI VDMA peripheral with\n");
  printf("  size=       %d\n", page_size);
  printf("  baseAddr=   0x%8x\n", AXI_DMA_BASEADDR);
  printf("  width=      %d\n", IN_FRAME_WIDTH);
  printf("  height=     %d\n", IN_FRAME_HEIGHT);
  printf("  channels=   %d\n", PIXEL_CHANNELS);
  printf("  fb1Addr=    0x%8x\n", MEM2VDMA_BUFFER1_BASEADDR);
  printf("  fb2Addr=    0x%8x\n", MEM2VDMA_BUFFER2_BASEADDR);
  printf("  fb3Addr=    0x%8x\n", MEM2VDMA_BUFFER3_BASEADDR);
  printf("\n");


  if((mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  printf("-d: /dev/mem opened.\n");


  printf("\n-d: Trying to map VDMA base address (0x%08x)..\n", AXI_DMA_BASEADDR);
  va0 = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem, AXI_DMA_BASEADDR);
  if(va0 == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }
  printf("-d: ..mapped to 0x%x.\n", va0);


  printf("Reading from VDMA control register: ");
  printf("0x%x\n", *((unsigned int*) va0));
  printf("Writing to VDMA registers:\n");
  *((volatile unsigned int*) va0) = 1;
  printf("Reading from VDMA control register: ");
  printf("0x%x\n", *((unsigned int*) va0));





  printf("\n-d: Trying to map buffer 1 (0x%x)..\n", MEM2VDMA_BUFFER1_BASEADDR);
  va1 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, mem, MEM2VDMA_BUFFER1_BASEADDR);
  if(va1 == MAP_FAILED) {
    printf("va1 mapping for absolute memory access failed.\n");
    return -2;
  }
  printf("-d: ..mapped to 0x%x.\n", va1);


  printf("\n-d: Trying to map buffer 2 (0x%x)..\n", MEM2VDMA_BUFFER2_BASEADDR);
  va2 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, mem, MEM2VDMA_BUFFER2_BASEADDR);
  if(va2 == MAP_FAILED) {
    printf("va1 mapping for absolute memory access failed.\n");
    return -2;
  }
  printf("-d: ..mapped to 0x%x.\n", va2);


  printf("\n-d: Trying to map buffer 3 (0x%x)..\n", MEM2VDMA_BUFFER3_BASEADDR);
  va3 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, mem, MEM2VDMA_BUFFER3_BASEADDR);
  if(va3 == MAP_FAILED) {
    printf("va1 mapping for absolute memory access failed.\n");
    return -2;
  }
  printf("-d: ..mapped to 0x%x.\n", va3);

  memset(va1, 255, len);
  printf("-d: fb1 memset done\n");
  memset(va2, 255, len);
  printf("-d: fb2 memset done\n");
  memset(va3, 255, len);
  printf("-d: fb3 memset done\n");

  printf("#Status register: %d\n", ((unsigned int *) va0)[OFFSET_VDMA_MM2S_STATUS_REGISTER>>2]);

  // macro in variabili locali
  vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, IN_FRAME_WIDTH, IN_FRAME_HEIGHT, PIXEL_CHANNELS, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR);
  printf("-d:2222\n");
  vdma_s2mm_status_dump(&handle);
  vdma_mm2s_status_dump(&handle);
    
  // Start triple buffering
  vdma_start_triple_buffering(&handle);
  printf("-d:3\n");
  // Run for 10 seconds, just monitor status registers
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

  // Halt VDMA and unmap memory ranges
  vdma_halt(&handle);
}
