#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char ** argv){

	unsigned int range = 16;
	unsigned int offset = 0;
	//unsigned int * mem_add = (unsigned int *)536870912;
	unsigned int * mem_add = (unsigned int *)520093696;
	unsigned int i=0;

	printf("Bare metal test.\n\n");
	

	//for(i=0; i<30; i++)
	//	*((volatile unsigned int *)mem_add+i)=i;
	
	
	//for(i=0; i<range; i++)
		//*((volatile unsigned int *)mem_add+i) = i;
	
	
	*((volatile unsigned int *)mem_add)=1;
	//*((volatile unsigned int *)mem_add+12)=110;
	//*((volatile unsigned int *)mem_add+13)=111;
	//*((volatile unsigned int *)mem_add+14)=112;
	//*((volatile unsigned int *)mem_add+15)=113;
	/*
	*((volatile unsigned int *)mem_add+4)=20;
	*((volatile unsigned int *)mem_add+5)=21;
	*((volatile unsigned int *)mem_add+6)=22;
	*((volatile unsigned int *)mem_add+7)=23;
	*((volatile unsigned int *)mem_add+8)=30;
	*((volatile unsigned int *)mem_add+9)=31;
	*((volatile unsigned int *)mem_add+10)=32;
	*((volatile unsigned int *)mem_add+11)=33;
	*/
	//*((volatile unsigned int *)mem_add+12)=40;
	//*((volatile unsigned int *)mem_add+13)=41;
	//*((volatile unsigned int *)mem_add+14)=42;
	//*((volatile unsigned int *)mem_add+15)=43;
	/*((volatile unsigned int *)mem_add+16)=416;
	*((volatile unsigned int *)mem_add+17)=417;
  */

	//*((volatile unsigned int *)mem_add+5)=245;



	//printf("Reading at %x: %d\n\n", mem_add, *mem_add);

	printf("Memory written:\n");
	for(i=0; i<range; i++)
		printf("[%x]  %d\n", mem_add+i, *(mem_add+i));

	printf("Terminate..\n");

	return 0;
}