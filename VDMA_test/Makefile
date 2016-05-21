CC=arm-xilinx-linux-gnueabi-gcc
#CC=gcc

hello:
	$(CC) hello.c -o hello

vdma-only:
	$(CC) vdma.c -o vdma.o
test:
	$(CC) test-VDMA.c vdma.c -o test-VDMA

test-DMA:
	$(CC) test-DMA.c vdma.c -o test-DMA	

scp-test: test
	scp test-VDMA zedboard:~/

scp-test-dma: test-DMA
	scp test-DMA zedboard:~/

.PHONY: vdma-only test hello scp-test scp-test-dma test-DMA

clean:
	rm -rf *.o test-VDMA hello
