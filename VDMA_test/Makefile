#CC=arm-xilinx-linux-gnueabi-gcc
CC=riscv64-unknown-linux-gnu-gcc
#CC=gcc

hello:
	$(CC) hello.c -o hello

vdma-only:
	$(CC) vdma.c -o vdma.o

test-VDMA-arm:
	arm-xilinx-linux-gnueabi-gcc test-VDMA.c vdma.c -o test-VDMA

test-VDMA-rv:
	riscv64-unknown-linux-gnu-gcc -DRC=1 test-VDMA.c vdma.c -o test-VDMA_rv64ima


test-DMA:
	$(CC) test-DMA.c vdma.c -o test-DMA	

scp-test: test-VDMA-arm
	scp test-VDMA zedboard:~/

scp-test-dma: test-DMA
	scp test-DMA zedboard:~/

.PHONY: vdma-only test hello scp-test scp-test-dma test-DMA

clean:
	rm -rf *.o test-VDMA hello
