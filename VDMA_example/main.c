
/* changes the devicetree
bootargs = "consoleblank=0 root=/dev/mmcblk0p2 rw rootwait earlyprintk mem=224M";
*/

#include <stdio.h>

#include "VDMA.h"
 
#define VDMAWidth 752
#define VDMAHeight 480
#define VDMAPixelWidth 3
#define VDMAFB1Adr 0x1f400000
#define VDMAFB2Adr 0x1f500000
#define VDMABaseAddr 0x25000000
 
 
int main()
{
    VDMA_info vdma;
 
    VDMA_Init(&vdma, VDMABaseAddr, VDMAWidth, VDMAHeight, VDMAPixelWidth, VDMAFB1Adr, VDMAFB2Adr);

    printf("Reg[0]: %x\n", VDMA_Get(&vdma, 0x54/4)); 
    VDMA_Set(&vdma, 0x54/4, 0xdead);
    printf("Reg[0]: %x\n", VDMA_Get(&vdma, 0x54/4)); 

//    VDMA_Start(&vdma, VDMAFB1Adr);
 
    printf("Waiting for end...\n");
//    while(VDMA_IsDone(&vdma)==0);
 
    VDMA_UnInit(&vdma);
}