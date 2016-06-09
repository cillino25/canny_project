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

  printf("-d: ** VDMA status.\n");


  if((handle.vdmaHandler = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  handle.vdmaVirtualAddress = (unsigned int*)mmap(NULL, AXI_VDMA_REG_SPACE, PROT_READ | PROT_WRITE, MAP_SHARED, handle.vdmaHandler, AXI_VDMA_BASEADDR);
  if(((unsigned int *)handle.vdmaVirtualAddress) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }
  
  

  vdma_mm2s_status_dump(&handle);
  vdma_s2mm_status_dump(&handle);


 
  printf("=================== MM2S ===================\n");
  printf("MM2S_VDMACR:        0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER));
  printf("MM2S_VDMASR:        0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_STATUS_REGISTER));
  printf("MM2S_REG_INDEX:     0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_REG_INDEX));
  printf("MM2S_VSIZE:         0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_VSIZE));
  printf("MM2S_HSIZE:         0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_HSIZE));
  printf("MM2S_FRMDLY_STRIDE: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_FRMDLY_STRIDE));
  printf("VDMA MM2S Buffer 1: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_FRAMEBUFFER1));
  printf("VDMA MM2S Buffer 2: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_FRAMEBUFFER2));
  printf("VDMA MM2S Buffer 3: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_MM2S_FRAMEBUFFER3));

  printf("\n=================== S2MM ===================\n");
	printf("S2MM_VDMACR:        0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER));
  printf("S2MM_VDMASR:        0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_STATUS_REGISTER));
  printf("S2MM_IRQ_MASK:      0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_IRQ_MASK));
  printf("S2MM_REG_INDEX:     0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_REG_INDEX));
  printf("S2MM_VSIZE:         0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_VSIZE));
  printf("S2MM_HSIZE:         0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_HSIZE));
  printf("S2MM_FRMDLY_STRIDE: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_FRMDLY_STRIDE));
  printf("VDMA S2MM Buffer 1: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_FRAMEBUFFER1));
  printf("VDMA S2MM Buffer 2: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_FRAMEBUFFER2));
  printf("VDMA S2MM Buffer 3: 0x%08x\n", vdma_get(&handle, OFFSET_VDMA_S2MM_FRAMEBUFFER3));

  printf("\n================= Park reg ==================\n");
  int park_reg = vdma_get(&handle, OFFSET_PARK_PTR_REG);
  printf("PARK_REG:           0x%08x\n", park_reg);
  printf("PARK_REG Write fr:  %d\n", (park_reg & PARK_PTR_WRITE_FRAME_NUM) >> 24);
  printf("PARK_REG Read fr:   %d\n", (park_reg & PARK_PTR_READ_FRAME_NUM) >> 16);
  printf("PARK_REG Write ref: %d\n", (park_reg & PARK_PTR_WRITE_FRAME_REF) >> 8);
  printf("PARK_REG Read ref:  %d\n", (park_reg & PARK_PTR_READ_FRAME_REF));

  printf("Bye!\n");
}

