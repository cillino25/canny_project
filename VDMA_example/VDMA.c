/*
 * VDMA.c
 *
 *  Created on: 17.3.2013
 *      Author: Ales Ruda
 *      web: www.arbot.cz
 */
 
#include "VDMA.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
 
int VDMA_Init(VDMA_info *info, unsigned int baseAddr, int width, int height, int pixelLength, unsigned int fb1Addr, unsigned int fb2Addr)
{
    info->baseAddr=baseAddr;
    info->width=width;
    info->height=height;
    info->pixelLength=pixelLength;
    info->fbLength=pixelLength*width*height;
    info->vdmaHandler = open("/dev/mem", O_RDWR);
    info->vdmaVirtualAddress = (unsigned int*)mmap(NULL, VDMAMapLen, PROT_READ | PROT_WRITE, MAP_SHARED, info->vdmaHandler, (off_t)info->baseAddr);
    if(info->vdmaVirtualAddress == MAP_FAILED)
    {
     perror("vdmaVirtualAddress mapping for absolute memory access failed.\n");
     return -1;
    }
    info->fb1VirtualAddress = (unsigned int*)mmap(NULL, info->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, info->vdmaHandler, (off_t)fb1Addr);
    if(info->fb1VirtualAddress == MAP_FAILED)
    {
     perror("fb1VirtualAddress mapping for absolute memory access failed.\n");
     return -2;
    }
    info->fb2VirtualAddress = (unsigned int*)mmap(NULL, info->fbLength, PROT_READ | PROT_WRITE, MAP_SHARED, info->vdmaHandler, (off_t)fb2Addr);
    if(info->fb2VirtualAddress == MAP_FAILED)
    {
     perror("fb2VirtualAddress mapping for absolute memory access failed.\n");
     return -3;
    }

    printf("memset first buffer..\n");
    memset((void*)info->fb1VirtualAddress, 255, info->fbLength);
    printf("memset second buffer..\n");
    memset((void*)info->fb1VirtualAddress, 255, info->fbLength);
 
    return 0;
}
 
void VDMA_UnInit(VDMA_info *info)
{
    munmap((void *)info->vdmaVirtualAddress, VDMAMapLen);
    munmap((void *)info->fb1VirtualAddress, info->fbLength);
    munmap((void *)info->fb2VirtualAddress, info->fbLength);
    close(info->vdmaHandler);
}
 
unsigned int VDMA_Get(VDMA_info *info, int num)
{
    if(num>=0 && num<VDMACount)
    {
        return info->vdmaVirtualAddress[num];
    }
    return 0;
}
 
void VDMA_Set(VDMA_info *info, int num, unsigned int val)
{
    if(num>=0 && num<VDMACount)
    {
        info->vdmaVirtualAddress[num]=val;
    }
}
 
void VDMA_OutStart(VDMA_info *info, unsigned int adr, int circular)
{
    VDMA_Disp(info, "status ", 0x04/4);
    VDMA_Disp(info, "control ", 0x00/4);
 
    VDMA_Set(info, 0x00/4, circular==1?4+2:4);  // MM2S_DMACR: reset
 
    while((VDMA_Get(info, 0x00/4)&4)==4); // wait for reset end
 
    VDMA_Disp(info, "status ", 0x04/4);
    VDMA_Set(info, 0x04/4, 0xffffffff);  // S2MM_DMASR: remove errors
    VDMA_Disp(info, "status ", 0004/4);
 
    usleep(100000);
 
    VDMA_Set(info, 0x18/4, 1);  // MM2S_FRMSTORE: 1
 
    VDMA_Set(info, 0x00/4, circular==1?2+1:1);  // S2MM_DMACR: RS
    // wait for run
    while((VDMA_Get(info, 0x00/4)&1)==0 || (VDMA_Get(info, 0x04/4)&1)==1)
    {
        VDMA_Disp(info, "status ", 0004/4);
        VDMA_Disp(info, "control ", 0x00/4);
    }
 
    VDMA_Set(info, 0x28/4, 0); // this alters s2mm park ptr
 
    VDMA_Set(info, 0x5C/4, adr); //adr1
//  VDMA_Set(info, 0x60/4, adr); //adr2
 
    VDMA_Set(info, 0x58/4, info->pixelLength*info->width);  // stride length in bytes
 
    VDMA_Set(info, 0x54/4, info->pixelLength*info->width);  // length in bytes
 
    VDMA_Set(info, 0x50/4, info->height);  // height and start
 
    VDMA_Disp(info, "status ", 0x04/4);
    VDMA_Disp(info, "control ", 0x00/4);
    VDMA_Disp(info, "Park", 0x28/4);
}
 
void VDMA_Start(VDMA_info *info, unsigned int adr)
{
    VDMA_Set(info, 0x30/4, 64+4);  // S2MM_DMACR: sof=tuser, reset
 
    while((VDMA_Get(info, 0x30/4)&4)==4); // wait for reset end
 
    VDMA_Disp(info, "status ", 0x34/4);
    VDMA_Set(info, 0x34/4, 0xffffffff);  // S2MM_DMASR: remove errors
    VDMA_Disp(info, "status ", 0x34/4);
 
 
    VDMA_Set(info, 0x30/4, 64+1);  // S2MM_DMACR: sof=tuser, RS
// wait for run
    while((VDMA_Get(info, 0x30/4)&1)==0 || (VDMA_Get(info, 0x34/4)&1)==1)
    {
        VDMA_Disp(info, "status ", 0x34/4);
        VDMA_Disp(info, "control ", 0x30/4);
    }
 
    VDMA_Set(info, 0xAC/4, adr);
    VDMA_Set(info, 0x28/4, 0);
 
    VDMA_Set(info, 0xA8/4, info->pixelLength*info->width);  // stride length in bytes
 
    VDMA_Set(info, 0xA4/4, info->pixelLength*info->width);  // length in bytes
 
    VDMA_Set(info, 0xA0/4, info->height);  // height and start
    VDMA_Disp(info, "status ", 0x34/4);
    VDMA_Disp(info, "control ", 0x30/4);
}
 
int VDMA_IsRunning(VDMA_info *info)
{
    return (VDMA_Get(info, 0x34/4)&1)==1;
}
 
int VDMA_IsDone(VDMA_info *info)
{
    return (VDMA_Get(info, 0x34/4)&0x01000)!=0;
}
 
void VDMA_Disp(VDMA_info *info, char *str, int num)
{
    printf("%s(%02x)=%08x\n", str, num, VDMA_Get(info, num));
}