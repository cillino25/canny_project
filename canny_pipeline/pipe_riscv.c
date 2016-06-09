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
  void *poll_mmap_1, *poll_mmap_2;

  unsigned int page_size = sysconf(_SC_PAGESIZE);

  printf("-d:VDMA test - Rocketchip -- .\n");
  printf("Open AXI VDMA peripheral with\n");
  printf("  size=                 %d\n", page_size);
  printf("  baseAddr=             0x%x\n", AXI_VDMA_BASEADDR);
  printf("  width=                %d\n", IN_FRAME_WIDTH);
  printf("  height=               %d\n", IN_FRAME_HEIGHT);
  printf("  channels=             %d\n", PIXEL_CHANNELS);
  printf("  fb1_MEM2VDMA_Addr=    0x%x\n", MEM2VDMA_BUFFER1_BASEADDR);
  printf("  fb2_MEM2VDMA_Addr=    0x%x\n", MEM2VDMA_BUFFER2_BASEADDR);
  printf("  fb3_MEM2VDMA_Addr=    0x%x\n", MEM2VDMA_BUFFER3_BASEADDR);
  printf("  fb1_VDMA2MEM_Addr=    0x%x\n", VDMA2MEM_BUFFER1_BASEADDR);
  printf("  fb2_VDMA2MEM_Addr=    0x%x\n", VDMA2MEM_BUFFER2_BASEADDR);
  printf("  fb3_VDMA2MEM_Addr=    0x%x\n", VDMA2MEM_BUFFER3_BASEADDR);
  
  printf("\n");
  
  // open /dev/mem for writing a buffer
  if((mem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  write_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem, MEM2VDMA_BUFFER1_BASEADDR);
  if(((unsigned int *)write_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, IN_FRAME_WIDTH, IN_FRAME_HEIGHT, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR);
  printf("-d: VDMA set up.\n");

  vdma_start_triple_buffering_mod(&handle);
  printf("-d: started triple buffering..\n");

  fill_buffer(write_fb, handle.fbLength, 0xdeadbeef);
  printf("-d: buffer filled.\n");

  printf("wasting time..\n");

  printf("s2mm is working: %d\n", vdma_s2mm_running(&handle));
  printf("mm2s is working: %d\n", vdma_mm2s_running(&handle));

  printf("wasting time..\n");

  printf("s2mm is working: %d\n", vdma_s2mm_running(&handle));
  printf("mm2s is working: %d\n", vdma_mm2s_running(&handle));

  printf("Bye\n\n");
}