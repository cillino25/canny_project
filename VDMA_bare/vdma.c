#include <stdio.h>
#include <unistd.h>
#include <xil_printf.h>
#include <xil_types.h>
#include <xil_io.h>

#include "platform.h"

#include "vdma.h"
#include "vdma_parameters.h"



void vdma_halt(vdma_handle *handle) {
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
}

unsigned int vdma_get(vdma_handle *handle, int num) {
    if(num>=0)
      //return ((volatile unsigned int *)handle->vdmaVirtualAddress)[num>>2];
      return Xil_In32(handle->baseAddr + (num));

    return 0;
}

void vdma_set(vdma_handle *handle, int num, unsigned int val) {
    //((volatile unsigned int *)handle->vdmaVirtualAddress)[num>>2]=val;
	//xil_printf("Setting address 0x%x = 0x%x\r\n", handle->baseAddr + (num), val);
	Xil_Out32(handle->baseAddr + (num), val);

}

void vdma_status_dump(int status) {
    if (status & VDMA_STATUS_REGISTER_HALTED) xil_printf(" halted"); else xil_printf(" running");
    if (status & VDMA_STATUS_REGISTER_VDMAInternalError) xil_printf(" vdma-internal-error");
    if (status & VDMA_STATUS_REGISTER_VDMASlaveError) xil_printf(" vdma-slave-error");
    if (status & VDMA_STATUS_REGISTER_VDMADecodeError) xil_printf(" vdma-decode-error");
    if (status & VDMA_STATUS_REGISTER_StartOfFrameEarlyError) xil_printf(" start-of-frame-early-error");
    if (status & VDMA_STATUS_REGISTER_EndOfLineEarlyError) xil_printf(" end-of-line-early-error");
    if (status & VDMA_STATUS_REGISTER_StartOfFrameLateError) xil_printf(" start-of-frame-late-error");
    if (status & VDMA_STATUS_REGISTER_FrameCountIRQ) xil_printf(" frame-count-interrupt");
    if (status & VDMA_STATUS_REGISTER_DelayCountIRQ) xil_printf(" delay-count-interrupt");
    if (status & VDMA_STATUS_REGISTER_ErrorIRQ) xil_printf(" error-interrupt");
    if (status & VDMA_STATUS_REGISTER_EndOfLineLateError) xil_printf(" end-of-line-late-error");
    xil_printf(" frame-count:%d", (status & VDMA_STATUS_REGISTER_IRQFrameCount) >> 16);
    xil_printf(" delay-count:%d", (status & VDMA_STATUS_REGISTER_IRQDelayCount) >> 24);
    xil_printf("\n");
}
void vdma_s2mm_status_dump(vdma_handle *handle) {
    int status = vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER);
    xil_printf("S2MM status register (%08x):", status);
    vdma_status_dump(status);
}

void vdma_mm2s_status_dump(vdma_handle *handle) {
    int status = vdma_get(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER);
    xil_printf("MM2S status register (%08x):", status);
    vdma_status_dump(status);
}

void vdma_control_dump(int control){
    if (control & VDMA_CONTROL_REGISTER_START) xil_printf(" running"); else xil_printf(" stopped");
    if (control & VDMA_CONTROL_REGISTER_CIRCULAR_PARK) xil_printf(" circular-mode"); else xil_printf(" park-mode");
    if (control & VDMA_CONTROL_REGISTER_RESET) xil_printf(" reset-in-progress");
    if (control & VDMA_CONTROL_REGISTER_GENLOCK_ENABLE) xil_printf(" genlock");
    if (control & VDMA_CONTROL_REGISTER_FrameCntEn) xil_printf(" frame-counter-enabled(still transmitting)");
    if (control & VDMA_CONTROL_REGISTER_GenlockSrc) xil_printf(" internal-genlock"); else xil_printf(" external-genlock");
    xil_printf(" r/w-pointer:%d", (control & VDMA_CONTROL_REGISTER_RWrPntr) >> 8);
    if (control & VDMA_CONTROL_REGISTER_FrmCtn_IrqEn) xil_printf(" frame-count-irq-enabled");
    if (control & VDMA_CONTROL_REGISTER_DlyCnt_IrqEn) xil_printf(" delay-count-interrupt");
    if (control & VDMA_CONTROL_REGISTER_ERR_IrqEn) xil_printf(" error-interrupt-enabled");
    if (control & VDMA_CONTROL_REGISTER_Repeat_En) xil_printf(" repeat-frames-on-errors");
    xil_printf(" frame-count:%d", (control & VDMA_CONTROL_REGISTER_InterruptFrameCount) >> 16);
    xil_printf(" delay-count:%d", (control & VDMA_CONTROL_REGISTER_IRQDelayCount) >> 24);
    xil_printf("\n");
}


void vdma_s2mm_control_dump(vdma_handle *handle){
    int control = vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER);
    xil_printf("S2MM control register: (%08x):", control);
    vdma_control_dump(control);
}

void vdma_mm2s_control_dump(vdma_handle *handle){
    int control = vdma_get(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER);
    xil_printf("MM2S control register: (%08x):", control);
    vdma_control_dump(control);
}


// Triple buffering - working version
void vdma_start_triple_buffering_mod(vdma_handle *handle) {
    // Reset VDMA
    //xil_printf("-d: resetting VDMA\r\n");
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER, VDMA_CONTROL_REGISTER_RESET);

    // Wait for reset to finish
    //xil_printf("-d: waiting for reset to finish...\r\n");
    while((vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER) & VDMA_CONTROL_REGISTER_RESET)==4);
    while((vdma_get(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER) & VDMA_CONTROL_REGISTER_RESET)==4);

    //xil_printf("-d: clear error bits in status registers\r\n");
    // Clear all error bits in status register
    vdma_set(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER, 0);
    //xil_printf("done\r\n");
    vdma_set(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER, 0);

    // Do not mask interrupts
    //xil_printf("Set interrupt mask\r\n");
    vdma_set(handle, OFFSET_VDMA_S2MM_IRQ_MASK, 0xf);

    int interrupt_frame_count = 1;

    //xil_printf("-d: start s2mm\r\n");
    // Start both S2MM and MM2S in triple buffering mode
    vdma_set(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER,
        (interrupt_frame_count << 16) |
        VDMA_CONTROL_REGISTER_START |
        VDMA_CONTROL_REGISTER_GENLOCK_ENABLE |
        VDMA_CONTROL_REGISTER_GenlockSrc |
        VDMA_CONTROL_REGISTER_CIRCULAR_PARK);
    //xil_printf("-d: start mm2s\r\n");
    vdma_set(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER,
        (interrupt_frame_count << 16) |
        VDMA_CONTROL_REGISTER_START |
        VDMA_CONTROL_REGISTER_GENLOCK_ENABLE |
        VDMA_CONTROL_REGISTER_GenlockSrc |
        VDMA_CONTROL_REGISTER_CIRCULAR_PARK);


    while((vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER)&1)==0 || (vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER)&1)==1) {
        xil_printf("Waiting for VDMA to start running...\r\n");
        #ifndef RC
          sleep(1);
        #else
          xil_printf("..........................................................");
        #endif
    }

    // Extra register index, use first 16 frame pointer registers
    vdma_set(handle, OFFSET_VDMA_S2MM_REG_INDEX, 0);

    //xil_printf("Setting frame buffer addresses..\r\n");
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
/*
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

    int interrupt_frame_count = 1;

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
        #ifndef RC
          sleep(1);
        #else
          printf("..........................................................");
        #endif
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
*/

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

void print_vdma_stats(vdma_handle *handle) {

  //xil_printf("-d: ** VDMA status.\n");

  vdma_mm2s_status_dump(handle);
  vdma_s2mm_status_dump(handle);
 
  xil_printf("=================== MM2S ===================\n");
  xil_printf("MM2S_VDMACR:        0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_CONTROL_REGISTER));
  xil_printf("MM2S_VDMASR:        0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_STATUS_REGISTER));
  xil_printf("MM2S_REG_INDEX:     0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_REG_INDEX));
  xil_printf("MM2S_VSIZE:         0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_VSIZE));
  xil_printf("MM2S_HSIZE:         0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_HSIZE));
  xil_printf("MM2S_FRMDLY_STRIDE: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_FRMDLY_STRIDE));
  xil_printf("VDMA MM2S Buffer 1: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER1));
  xil_printf("VDMA MM2S Buffer 2: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER2));
  xil_printf("VDMA MM2S Buffer 3: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_MM2S_FRAMEBUFFER3));

  xil_printf("\n=================== S2MM ===================\n");
    xil_printf("S2MM_VDMACR:        0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_CONTROL_REGISTER));
  xil_printf("S2MM_VDMASR:        0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_STATUS_REGISTER));
  xil_printf("S2MM_IRQ_MASK:      0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_IRQ_MASK));
  xil_printf("S2MM_REG_INDEX:     0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_REG_INDEX));
  xil_printf("S2MM_VSIZE:         0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_VSIZE));
  xil_printf("S2MM_HSIZE:         0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_HSIZE));
  xil_printf("S2MM_FRMDLY_STRIDE: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_FRMDLY_STRIDE));
  xil_printf("VDMA S2MM Buffer 1: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER1));
  xil_printf("VDMA S2MM Buffer 2: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER2));
  xil_printf("VDMA S2MM Buffer 3: 0x%08x\n", vdma_get(handle, OFFSET_VDMA_S2MM_FRAMEBUFFER3));

  xil_printf("\n================= Park reg ==================\n");
  int park_reg = vdma_get(handle, OFFSET_PARK_PTR_REG);
  xil_printf("PARK_REG:           0x%08x\n", park_reg);
  xil_printf("PARK_REG Write fr:  %d\n", (park_reg & PARK_PTR_WRITE_FRAME_NUM) >> 24);
  xil_printf("PARK_REG Read fr:   %d\n", (park_reg & PARK_PTR_READ_FRAME_NUM) >> 16);
  xil_printf("PARK_REG Write ref: %d\n", (park_reg & PARK_PTR_WRITE_FRAME_REF) >> 8);
  xil_printf("PARK_REG Read ref:  %d\n", (park_reg & PARK_PTR_READ_FRAME_REF));

  xil_printf("Bye!\n");
}

void vdma_send_fsync(vdma_handle *handle){
  *((volatile unsigned int *)handle->pulserVirtualAddress) = 1;
  //*((volatile unsigned int *)handle->pulserVirtualAddress) = 0;
}

// Length parameter must be in bytes!
void fill_buffer(unsigned int * fbAddr, int length, unsigned int val){
  int i=0;
  for(i=0; i<length/4; i++)
    //((volatile unsigned int *)fbAddr)[i] = val;
    Xil_Out32(*fbAddr + i*4, val);
}

// Length parameter must be in bytes!
int cmp_buffer(unsigned int * fbAddr, int length, unsigned int val){
  int i=0;
  xil_printf("-dbg: length= %d = 0x%x\n", length, length);
  for(i=0; i<length/4; i++){
    if(((volatile unsigned int *)fbAddr)[i] != val){
      xil_printf("Error comparing buffer: fb[%d]=%x <> %x\n", i, ((volatile unsigned int *)fbAddr)[i], val);
      return 1;
    }
  }
  xil_printf("Everything OK.\n");
  return 0;
}
