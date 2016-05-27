/*
 * VDMA.h
 *
 *  Created on: 16.3.2013
 *      Author: Ales
 *
 * VDMA core HW configuration: 
 * 
 * C_NUM_FSTORES = 2 
 * C_INCLUDE_MM2S = 1 
 * C_M_AXI_S2MM_DATA_WIDTH = 64 
 * C_S_AXIS_S2MM_TDATA_WIDTH = 64 
 * C_BASEADDR = 0x43000000 
 * C_HIGHADDR = 0x4300ffff 
 * C_S2MM_LINEBUFFER_THRESH = 128 
 * C_S2MM_SOF_ENABLE = 1 
 * C_M_AXI_MM2S_DATA_WIDTH = 64 
 * C_M_AXIS_MM2S_TDATA_WIDTH = 64 
 * C_INTERCONNECT_M_AXI_MM2S_READ_FIFO_DEPTH = 512 
 * C_MM2S_LINEBUFFER_THRESH = 128 
 * C_USE_FSYNC = 3 
 * C_FLUSH_ON_FSYNC = 3 
 * C_INTERCONNECT_M_AXI_S2MM_READ_FIFO_DEPTH = 512 
 * C_S2MM_LINEBUFFER_DEPTH = 256
 */

#ifndef VDMA_H_
#define VDMA_H_
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
 
#define VDMACount 64
#define VDMAMapLen VDMACount*4
 
typedef struct
{
    unsigned int baseAddr;
    int vdmaHandler;
    int width;
    int height;
    int pixelLength;
    int fbLength;
    unsigned int* vdmaVirtualAddress;
    unsigned int* fb1VirtualAddress;
    unsigned int* fb2VirtualAddress;
 
    pthread_mutex_t lock;
} VDMA_info;
 
int VDMA_Init(VDMA_info *info, unsigned int baseAddr, int width, int height, int pixelLength, unsigned int fb1Addr, unsigned int fb2Addr);
void VDMA_UnInit(VDMA_info *info);
unsigned int VDMA_Get(VDMA_info *info, int num);
void VDMA_Set(VDMA_info *info, int num, unsigned int val);
void VDMA_Start(VDMA_info *info, unsigned int adr);
void VDMA_OutStart(VDMA_info *info, unsigned int adr, int circular);
int VDMA_IsRunning(VDMA_info *info);
int VDMA_IsDone(VDMA_info *info);
void VDMA_Disp(VDMA_info *info, char *str, int num);
 
 
#endif /* VDMA_H_ */