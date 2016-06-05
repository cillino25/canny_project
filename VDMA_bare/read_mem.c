#include <stdio.h>
#include <unistd.h>
#include <xil_printf.h>
#include <xil_types.h>
#include <xil_io.h>
#include "platform.h"

#define BASE_ACC_ADDRESS  0x40000000	//AXI accelerator base address
#define MATRIX_SIZE	  	  6				//Square matrix of size MATRIX_SIZE*MATRIX_SIZE
//#define MEMORY_ADDRESS    0x0F000000	//Data starting address
//#define POLL_ADDRESS      0x0F100000	//Polling variable address
#define BLEN			  3				//Burst_Length - 1 (data range must be a multiple of Burst_Length)

/*
int main(int argc, char **argv)
{

	unsigned int range = MATRIX_SIZE*MATRIX_SIZE;

	int i = 0;



	printf("Values in memory:\n");
	for (i = 0; i < range; i++)
	{
		xil_printf("[%d]:  %8x	%p\n", i, Xil_In32(BASE_ACC_ADDRESS + (i<<2)), BASE_ACC_ADDRESS+(i<<2));
	}

	xil_printf("BYE! \r\n");
    return 0;
}
*/

