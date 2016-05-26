#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "vdma.h"
#include "vdma_parameters.h"




int vdma_setup(vdma_handle *handle, unsigned int page_size, unsigned int baseAddr, int width, int height, int pixelChannels, int max_buffer_size, long fb1Addr_mm2s, long fb2Addr_mm2s, long fb3Addr_mm2s, long fb1Addr_s2mm, long fb2Addr_s2mm, long fb3Addr_s2mm) {
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

    /********************** VDMA ************************/
    //printf("\n-d: Trying to map VDMA base address (0x%08x)..\n", handle->baseAddr);
    handle->vdmaVirtualAddress = (unsigned int*)mmap(NULL, AXI_VDMA_REG_SPACE, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, handle->baseAddr);
    if(((unsigned int *)handle->vdmaVirtualAddress) == MAP_FAILED) {
      printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
      return -1;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->vdmaVirtualAddress);
    
    

    /********************** MM2S ************************/
    
    handle->fb1PhysicalAddress_mm2s = fb1Addr_mm2s;
    //printf("\n-d: Trying to map fb1PhysicalAddress_mm2s (0x%x)..\n", handle->fb1PhysicalAddress_mm2s);
    handle->fb1VirtualAddress_mm2s = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb1PhysicalAddress_mm2s);
    if(handle->fb1VirtualAddress_mm2s == MAP_FAILED) {
      printf("fb1VirtualAddress_mm2s mapping for absolute memory access failed.\n");
      return -2;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb1VirtualAddress_mm2s);

    handle->fb2PhysicalAddress_mm2s = fb2Addr_mm2s;
    //printf("\n-d: Trying to map fb2PhysicalAddress_mm2s (0x%x)..\n", handle->fb2PhysicalAddress_mm2s);
    handle->fb2VirtualAddress_mm2s = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb2PhysicalAddress_mm2s);
    if(handle->fb2VirtualAddress_mm2s == MAP_FAILED) {
      printf("fb2VirtualAddress_mm2s mapping for absolute memory access failed.\n");
      return -3;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb2VirtualAddress_mm2s);
    
    handle->fb3PhysicalAddress_mm2s = fb3Addr_mm2s;
    //printf("\n-d: Trying to map fb3PhysicalAddress_mm2s (0x%x)..\n", handle->fb3PhysicalAddress_mm2s);
    handle->fb3VirtualAddress_mm2s = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb3PhysicalAddress_mm2s);
    if(handle->fb3VirtualAddress_mm2s == MAP_FAILED){
      printf("fb3VirtualAddress_mm2s mapping for absolute memory access failed.\n");
      return -3;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb3VirtualAddress_mm2s);

    
    

    /********************** S2MM ************************/
    
    handle->fb1PhysicalAddress_s2mm = fb1Addr_s2mm;
    //printf("\n-d: Trying to map fb1PhysicalAddress_s2mm (0x%x)..\n", handle->fb1PhysicalAddress_s2mm);
    handle->fb1VirtualAddress_s2mm = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb1PhysicalAddress_s2mm);
    if(handle->fb1VirtualAddress_s2mm == MAP_FAILED) {
      printf("fb1VirtualAddress_s2mm mapping for absolute memory access failed.\n");
      return -2;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb1VirtualAddress_s2mm);

    handle->fb2PhysicalAddress_s2mm = fb2Addr_s2mm;
    //printf("\n-d: Trying to map fb2PhysicalAddress_s2mm (0x%x)..\n", handle->fb2PhysicalAddress_s2mm);
    handle->fb2VirtualAddress_s2mm = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb2PhysicalAddress_s2mm);
    if(handle->fb2VirtualAddress_s2mm == MAP_FAILED) {
      printf("fb2VirtualAddress_s2mm mapping for absolute memory access failed.\n");
      return -3;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb2VirtualAddress_s2mm);
    
    handle->fb3PhysicalAddress_s2mm = fb3Addr_s2mm;
    //printf("\n-d: Trying to map fb3PhysicalAddress_s2mm (0x%x)..\n", handle->fb3PhysicalAddress_s2mm);
    handle->fb3VirtualAddress_s2mm = (unsigned int*)mmap(NULL, handle->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t)handle->fb3PhysicalAddress_s2mm);
    if(handle->fb3VirtualAddress_s2mm == MAP_FAILED){
      printf("fb3VirtualAddress_s2mm mapping for absolute memory access failed.\n");
      return -3;
    }
    //printf("-d: ..mapped to 0x%x.\n", handle->fb3VirtualAddress_s2mm);



    //printf("\n\n");
    memset((void*)handle->fb1VirtualAddress_mm2s, 170, handle->width*handle->height*handle->pixelChannels); //AA
    //printf("-d: fb1 memset done\n");
    memset((void*)handle->fb2VirtualAddress_mm2s, 187, handle->width*handle->height*handle->pixelChannels); //BB
    //printf("-d: fb2 memset done\n");
    memset((void*)handle->fb3VirtualAddress_mm2s, 204, handle->width*handle->height*handle->pixelChannels); //CC
    //printf("-d: fb3 memset done\n");
    
    //printf("\n\n");
    memset((void*)handle->fb1VirtualAddress_s2mm, 255, handle->width*handle->height*handle->pixelChannels);
    //printf("-d: fb1 memset done\n");
    memset((void*)handle->fb2VirtualAddress_s2mm, 255, handle->width*handle->height*handle->pixelChannels);
    //printf("-d: fb2 memset done\n");
    memset((void*)handle->fb3VirtualAddress_s2mm, 255, handle->width*handle->height*handle->pixelChannels);
    //printf("-d: fb3 memset done\n");

    return 0;
}


void vdma_halt(vdma_handle *handle) {
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    munmap(handle->vdmaVirtualAddress, AXI_VDMA_REG_SPACE);
    munmap(handle->fb1VirtualAddress_mm2s, handle->fbLength);
    munmap(handle->fb2VirtualAddress_mm2s, handle->fbLength);
    munmap(handle->fb3VirtualAddress_mm2s, handle->fbLength);
    munmap(handle->fb1VirtualAddress_s2mm, handle->fbLength);
    munmap(handle->fb2VirtualAddress_s2mm, handle->fbLength);
    munmap(handle->fb3VirtualAddress_s2mm, handle->fbLength);
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


// Triple buffering - working version
void vdma_start_triple_buffering_mod(vdma_handle *handle) {
    // Reset VDMA
    printf("-d: resetting VDMA\n");
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);

    // Wait for reset to finish
    printf("-d: waiting for reset to finish...\n");
    while((vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER) & VDMA_CONTROL_REGISTER_RESET)==4);
    while((vdma_get(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER) & VDMA_CONTROL_REGISTER_RESET)==4);

    printf("-d: clear error bits in status registers\n");
    // Clear all error bits in status register
    vdma_set(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER, 0);
    vdma_set(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER, 0);

    // Do not mask interrupts
    vdma_set(handle, OFFSET_VDMA_S2MM_IRQ_MASK, 0xf);

    int interrupt_frame_count = 3;

    printf("-d: start s2mm\n");
    // Start both S2MM and MM2S in triple buffering mode
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER,
        (interrupt_frame_count << 16) |
        VDMA_CONTROL_REGISTER_START |
        VDMA_CONTROL_REGISTER_GENLOCK_ENABLE |
        VDMA_CONTROL_REGISTER_GenlockSrc |
        VDMA_CONTROL_REGISTER_CIRCULAR_PARK);
    printf("-d: start mm2s\n");
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
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER1, (unsigned int)handle->fb1PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER1, (unsigned int)handle->fb1PhysicalAddress_s2mm);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER2, (unsigned int)handle->fb2PhysicalAddress_mm2s);    
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER2, (unsigned int)handle->fb2PhysicalAddress_s2mm);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER3, (unsigned int)handle->fb3PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER3, (unsigned int)handle->fb3PhysicalAddress_s2mm);

    // Write Park pointer register
    vdma_set(handle, OFFSET_PARK_PTR_REG, 0);

    // Frame delay and stride (bytes)
    //vdma_set(handle, OFFSET_VDMA_S2MM_FRMDLY_STRIDE, 1<<24 | handle->width*handle->pixelChannels);
    //vdma_set(handle, OFFSET_VDMA_MM2S_FRMDLY_STRIDE, 1<<24 | handle->width*handle->pixelChannels);

    vdma_set(handle, OFFSET_VDMA_S2MM_FRMDLY_STRIDE, handle->width*handle->pixelChannels);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRMDLY_STRIDE, handle->width*handle->pixelChannels);

    
    // Write horizontal size (bytes)
    vdma_set(handle, OFFSET_VDMA_S2MM_HSIZE, handle->width*handle->pixelChannels);
    vdma_set(handle, OFFSET_VDMA_MM2S_HSIZE, handle->width*handle->pixelChannels);

    // Write vertical size (lines), this actually starts the transfer
    vdma_set(handle, OFFSET_VDMA_S2MM_VSIZE, handle->height);
    vdma_set(handle, OFFSET_VDMA_MM2S_VSIZE, handle->height);
}


// Triple buffering original version
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
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER1, (unsigned int)handle->fb1PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER1, (unsigned int)handle->fb1PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER2, (unsigned int)handle->fb2PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER2, (unsigned int)handle->fb2PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER3, (unsigned int)handle->fb3PhysicalAddress_mm2s);
    vdma_set(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER3, (unsigned int)handle->fb3PhysicalAddress_mm2s);

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

// Length parameter must be in bytes!
void fill_buffer(unsigned int * fbAddr, int length, unsigned int val){
  int i=0;
  for(i=0; i<length/4; i++)
    ((volatile unsigned int *)fbAddr)[i] = val;
}

// Length parameter must be in bytes!
int cmp_buffer(unsigned int * fbAddr, int length, unsigned int val){
  int i=0;
  for(i=0; i<length/4; i++){
    if(((volatile unsigned int *)fbAddr)[i] != val){
      printf("Error comparing buffer: fb[%d]=%x <> %x\n", i, ((volatile unsigned int *)fbAddr)[i], val);
      return 1;
    }
  }
  printf("Everything OK.\n");
  return 0;
}
