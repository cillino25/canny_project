test-arm:
	arm-xilinx-linux-gnueabi-gcc main.c VDMA.c -o VDMA-test

scp-test: test-arm
	scp VDMA-test zedboard:~

test-rv:
	riscv64-unknown-linux-gnu-gcc main.c VDMA.c -o VDMA_test_rv64ima


.PHONY: test scp-test
