#ifndef _DATASET_H
#define _DATASET_H


#define ACC_ADDRESS_ARM 1073741824
#define ACC_ADDRESS_RC 536870912
#define POLL_ADDRESS_1 520093696
#define POLL_ADDRESS_2 521142272

#define MEM_SIZE 16

#define N_TESTS 24


static unsigned int test[N_TESTS*MEM_SIZE] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		10, 0, 10, 0, 0, 0, 0, 0, 10, 0, 0, 10, 10, 10, 10, 10,
		0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef,
		0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef,0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef,
		0xdeadbeef, 0, 0xdeadbeef, 0, 0xdeadbeef, 0, 0xdeadbeef, 0, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0xdeadbeef, 0, 0, 0, 0, 0xdeadbeef, 0, 0, 0xdeadbeef, 0, 0, 0xdeadbeef,
		0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef
	};

#endif