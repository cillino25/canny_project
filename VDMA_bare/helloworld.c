/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <unistd.h>
#include <xil_printf.h>
#include <xil_types.h>
#include <xil_io.h>

#include "platform.h"


#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"
#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"

//void print(char *str);


int main() {

  int h, j, i;
  vdma_handle handle;
  sepimgfilter_handle filter_handle;
  char img[] = {0, 0, 0, 0, 1, 1, 1, 0, 2, 2, 2, 0, 3, 3, 3, 0, 4, 4, 4, 0, 5, 5, 5, 0, 6, 6, 6, 0, 7, 7, 7, 0, 8, 8, 8, 0, 9, 9, 9, 0, 10, 10, 10, 0, 11, 11, 11, 0, 12, 12, 12, 0, 13, 13, 13, 0, 14, 14, 14, 0, 15, 15, 15, 0};
  unsigned char hz_coeffs[] = {1, 4, 7, 4, 1};
  unsigned char vt_coeffs[] = {1, 4, 7, 4, 1};
  unsigned int norm = 289;

  int size=64, width=8, height=8;
  unsigned char *img_data;
  unsigned char *img_info;

  int frame_len = 1 * width * height;


  init_platform();

  xil_printf("Setting I/O buffers\r\n");

  // memset input buffer 1
  for(i=0; i<size; i++) Xil_Out32(handle.fb1PhysicalAddress_mm2s, img[i]);
  // memset input buffer 2
  for(i=0; i<BUFFER_SIZE/4; i++) Xil_Out32(MEM2VDMA_BUFFER2_BASEADDR + i*4, 0xBBBBBBBB);
  // memset input buffer 3
  for(i=0; i<BUFFER_SIZE/4; i++) Xil_Out32(MEM2VDMA_BUFFER3_BASEADDR + i*4, 0xCCCCCCCC);
  // memset output buffer 1
  for(i=0; i<BUFFER_SIZE/4; i++) Xil_Out32(VDMA2MEM_BUFFER1_BASEADDR + i*4, 0xFFFFFFFF);
  // memset output buffer 2
  for(i=0; i<BUFFER_SIZE/4; i++) Xil_Out32(VDMA2MEM_BUFFER2_BASEADDR + i*4, 0xFFFFFFFF);
  // memset output buffer 3
  for(i=0; i<BUFFER_SIZE/4; i++) Xil_Out32(VDMA2MEM_BUFFER3_BASEADDR + i*4, 0xFFFFFFFF);

  xil_printf("Setting VDMA registers\r\n");

  //vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, width, height, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER_ADDR);
  handle.baseAddr = AXI_VDMA_BASEADDR;
  handle.width = 8;
  handle.height = 8;
  handle.pixelChannels = 1;
  handle.fbLength = handle.width * handle.height * handle.pixelChannels;
  handle.fb1PhysicalAddress_mm2s = 0x1f400000;
  handle.fb2PhysicalAddress_mm2s = 0x1f500000;
  handle.fb3PhysicalAddress_mm2s = 0x1f600000;
  handle.fb1PhysicalAddress_s2mm = 0x1f700000;
  handle.fb2PhysicalAddress_s2mm = 0x1f800000;
  handle.fb3PhysicalAddress_s2mm = 0x1f900000;
  handle.pulserPhysicalAddress   = 0x46000000;


  xil_printf("Setting sepImageFilter registers\r\n");
  //sepImageFilter_setup_handle(&filter_handle, &handle.vdmaHandler, page_size, AXI_SEPIMGFILTER);
  filter_handle.baseAddr = AXI_SEPIMGFILTER;
  sepImageFilter_setImageParams(&filter_handle, width, height);
  sepImageFilter_setHzKernelCoeffs(&filter_handle, hz_coeffs[0], hz_coeffs[1], hz_coeffs[2], hz_coeffs[3], hz_coeffs[4]);
  sepImageFilter_setVtKernelCoeffs(&filter_handle, vt_coeffs[0], vt_coeffs[1], vt_coeffs[2], vt_coeffs[3], vt_coeffs[4]);
  sepImageFilter_setNormalizationFactor(&filter_handle, norm);
  sepImageFilter_setup(&filter_handle);

  xil_printf("Setting up done.\r\n");

  xil_printf("Starting VDMA triple buffering..\r\n");
  vdma_start_triple_buffering_mod(&handle);

  xil_printf("Starting sepImageFilter..\r\n");
  sepImageFilter_start(&filter_handle);

  xil_printf("VDMA and filter started.\r\n");

  // Read the image



  xil_printf("sepImageFilter status register: %d\r\n", Xil_In32(0x47000000));

  // wait for filter to finish
  xil_printf("Waiting the filter to finish..\r\n");
  while(sepImageFilter_running(&filter_handle) == 0);

  xil_printf("Print output buffer (0x%x):\r\n", handle.fb1PhysicalAddress_s2mm);
  for(i=0; i<size; i++) xil_printf("[0x%x] 0x%x\r\n", handle.fb1PhysicalAddress_s2mm + (i<<2), Xil_In32(handle.fb1PhysicalAddress_s2mm + (i<<2) ));

  xil_printf("Bye!\r\n");


  while(1);


  cleanup_platform();
  return 0;
}

