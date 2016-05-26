#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "vdma.h"
#include "vdma_parameters.h"




int vdma_setup(vdma_handle *handle, unsigned int page_size, unsigned int baseAddr, int width, int height, int pixelChannels, int max_buffer_size, long fb1Addr, long fb2Addr, long fb3Addr) {
    handle->baseAddr=baseAddr;
    handle->width=width;
    handle->height=height;
    handle->pixelChannels=pixelChannels;
    handle->fbLength=pixelChannels*width*height;

    if(handle->fbLength > max_buffer_size){
      printf("Frame buffer size is greater than maximum buffer size.\nExiting..\n");
      return -1;
    }

    
    if((handle->vdmaHandler = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
      printf("Can't open /dev/mem.\nExiting...\n");
      return 1;
    }
    //printf("-d: /dev/mem opened.\n");

    
    //printf("\n-d: Trying to map VDMA base address (0x%08x)..\n", handle->baseAddr);
    handle->vdmaVirtualAddress = (unsigned int*)mmap(NULL, AXI_VDMA_REG_SPACE, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, handle->baseAddr);
    if(((unsigned int *)handle->vdmaVirtualAddress) == MAP_FAILED) {
      printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
      return -1;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->vdmaVirtualAddress);
    
    
    
    handle->fb1PhysicalAddress = fb1Addr;
    //printf("\n-d: Trying to map fb1PhysicalAddress (0x%x)..\n", handle->fb1PhysicalAddress);
    handle->fb1VirtualAddress = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb1PhysicalAddress);
    if(handle->fb1VirtualAddress == MAP_FAILED) {
      printf("fb1VirtualAddress mapping for absolute memory access failed.\n");
      return -2;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb1VirtualAddress);



    handle->fb2PhysicalAddress = fb2Addr;
    //printf("\n-d: Trying to map fb2PhysicalAddress (0x%x)..\n", handle->fb2PhysicalAddress);
    handle->fb2VirtualAddress = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb2PhysicalAddress);
    if(handle->fb2VirtualAddress == MAP_FAILED) {
      printf("fb2VirtualAddress mapping for absolute memory access failed.\n");
      return -3;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb2VirtualAddress);


    
    handle->fb3PhysicalAddress = fb3Addr;
    //printf("\n-d: Trying to map fb3PhysicalAddress (0x%x)..\n", handle->fb3PhysicalAddress);
    handle->fb3VirtualAddress = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb3PhysicalAddress);
    if(handle->fb3VirtualAddress == MAP_FAILED){
      printf("fb3VirtualAddress mapping for absolute memory access failed.\n");
      return -3;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb3VirtualAddress);

    printf("\n\n");
    //printf("memsetting buffers..\n");
    memset((void*)handle->fb1VirtualAddress, 160, handle->width*handle->height*handle->pixelChannels);
    //printf("-d: fb1 memset REALLY done\n");
    memset((void*)handle->fb2VirtualAddress, 255, handle->width*handle->height*handle->pixelChannels);
    //printf("-d: fb2 memset done\n");
    memset((void*)handle->fb3VirtualAddress, 255, handle->width*handle->height*handle->pixelChannels);
    //printf("-d: fb3 memset done\n");

    return 0;
}


void vdma_halt(vdma_handle *handle) {
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    munmap(handle->vdmaVirtualAddress, AXI_VDMA_REG_SPACE);
    munmap(handle->fb1VirtualAddress, handle->fbLength);
    munmap(handle->fb2VirtualAddress, handle->fbLength);
    munmap(handle->fb3VirtualAddress, handle->fbLength);
    close(handle->vdmaHandler);
}

unsigned int vdma_get(vdma_handle *handle, int num) {
    if(num>=0)
      return handle->vdmaVirtualAddress[num>>2];

    return 0;
}

void vdma_set(vdma_handle *handle, int num, unsigned int val) {
    *((volatile unsigned int *)handle->vdmaVirtualAddress + (num>>2))=val;
}

void vdma_status_dump(int status) {
    if (status & VDMA_STATUS_REGISTER_HALTED) printf(" halted"); else printf("running");
    if (status & VDMA_STATUS_REGISTER_VDMAInternalError) printf(" vdma-internal-error");
    if (status & VDMA_STATUS_REGISTER_VDMASlaveError) printf(" vdma-slave-error");
    if (status & VDMA_STATUS_REGISTER_VDMADecodeError) printf(" vdma-decode-error");
    if (status & VDMA_STATUS_REGISTER_StartOfFrameEarlyError) printf(" start-of-frame-early-error");
    if (status & VDMA_STATUS_REGISTER_EndOfLineEarlyError) printf(" end-of-line-early-error");
    if (status & VDMA_STATUS_REGISTER_StartOfFrameLateError) printf(" start-of-frame-late-error");
    if (status & VDMA_STATUS_REGISTER_FrameCountIRQ) printf(" frame-count-interrupt");
    if (status & VDMA_STATUS_REGISTER_DelayCountIRQ) printf(" delay-count-interrupt");
    if (status & VDMA_STATUS_REGISTER_ErrorIRQ) printf(" error-interrupt");
    if (status & VDMA_STATUS_REGISTER_EndOfLineLateError) printf(" end-of-line-late-error");
    printf(" frame-count:%d", (status & VDMA_STATUS_REGISTER_IRQFrameCount) >> 16);
    printf(" delay-count:%d", (status & VDMA_STATUS_REGISTER_IRQDelayCount) >> 24);
    printf("\n");
}

void vdma_s2mm_status_dump(vdma_handle *handle) {
    int status = vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER);
    printf("S2MM status register (%08x):", status);
    vdma_status_dump(status);
}

void vdma_mm2s_status_dump(vdma_handle *handle) {
    int status = vdma_get(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER);
    printf("MM2S status register (%08x):", status);
    vdma_status_dump(status);
}

void vdma_start_triple_buffering(vdma_handle *handle) {
    // Reset VDMA
    //printf("-d: 1\n");
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);

    // Wait for reset to finish
    //printf("-d: 2\n");
    while((vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER) & VDMA_CONTROL_REGISTER_RESET)==4);
    while((vdma_get(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER) & VDMA_CONTROL_REGISTER_RESET)==4);

    // Clear all error bits in status register
    //printf("-d: 3\n");
    vdma_set(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER, 0);
    vdma_set(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER, 0);

    // Do not mask interrupts
    vdma_set(handle, OFFSET_VDMA_S2MM_IRQ_MASK, 0xf);

    int interrupt_frame_count = 3;

    // Start both S2MM and MM2S in triple buffering mode
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER,
        (interrupt_frame_count << 16) |
        VDMA_CONTROL_REGISTER_START |
        VDMA_CONTROL_REGISTER_GENLOCK_ENABLE |
        VDMA_CONTROL_REGISTER_GenlockSrc |
        VDMA_CONTROL_REGISTER_CIRCULAR_PARK);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER,
        (interrupt_frame_count << 16) |
        VDMA_CONTROL_REGISTER_START |
        VDMA_CONTROL_REGISTER_GENLOCK_ENABLE |
        VDMA_CONTROL_REGISTER_GenlockSrc |
        VDMA_CONTROL_REGISTER_CIRCULAR_PARK);


    while((vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER)&1)==0 || (vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER)&1)==1) {
        printf("Waiting for VDMA to start running...\n");
        sleep(1);
    }

    // Extra register index, use first 16 frame pointer registers
    vdma_set(handle, OFFSET_VDMA_S2MM_REG_INDEX, 0);

    // Write physical addresses to control register
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER1, (unsigned int)handle->fb1PhysicalAddress);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER1, (unsigned int)handle->fb1PhysicalAddress);
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER2, (unsigned int)handle->fb2PhysicalAddress);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER2, (unsigned int)handle->fb2PhysicalAddress);
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER3, (unsigned int)handle->fb3PhysicalAddress);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER3, (unsigned int)handle->fb3PhysicalAddress);

    // Write Park pointer register
    vdma_set(handle, OFFSET_PARK_PTR_REG, 0);

    // Frame delay and stride (bytes)
    vdma_set(handle, OFFSET_VDMA_S2MM_FRMDLY_STRIDE, handle->width*handle->pixelChannels);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRMDLY_STRIDE, handle->width*handle->pixelChannels);

    // Write horizontal size (bytes)
    vdma_set(handle, OFFSET_VDMA_S2MM_HSIZE, handle->width*handle->pixelChannels);
    vdma_set(handle, OFFSET_VDMA_MM2S_HSIZE, handle->width*handle->pixelChannels);

    // Write vertical size (lines), this actually starts the transfer
    vdma_set(handle, OFFSET_VDMA_S2MM_VSIZE, handle->height);
    vdma_set(handle, OFFSET_VDMA_MM2S_VSIZE, handle->height);
}

int vdma_s2mm_running(vdma_handle *handle) {
    // Check whether VDMA is running, that is ready to start transfers
    return (vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER)&1)==1;
}

int vdma_s2mm_idle(vdma_handle *handle) {
    // Check whether VDMA is transferring
    return (vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER) & VDMA_STATUS_REGISTER_FrameCountIRQ)!=0;
}

int vdma_mm2s_running(vdma_handle *handle) {
    // Check whether VDMA is running, that is ready to start transfers
    return (vdma_get(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER)&1)==1;
}

int vdma_mm2s_idle(vdma_handle *handle) {
    // Check whether VDMA is transferring
    return (vdma_get(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER) & VDMA_STATUS_REGISTER_FrameCountIRQ)!=0;
}




/***********************************************************************************************************/


void vdma_fill_buffer(vdma_handle *handle, unsigned int * fbAddr, unsigned int val){
  int i=0;
  for(i=0; i<handle->fbLength; i++)
    ((volatile unsigned int *)fbAddr)[i] = val;
}

void vdma_cmp_buffer(vdma_handle *handle, unsigned int * fbAddr, unsigned int val){
  int i=0;
  for(i=0; i<handle->fbLength/4; i++){
    if(((volatile unsigned int *)fbAddr)[i] != val){
      printf("Error comparing buffer: fb[%d]=%x <> %x\n", i, ((volatile unsigned int *)fbAddr)[i], val);
      return;
    }
  }
  printf("Everything OK.\n");
}

