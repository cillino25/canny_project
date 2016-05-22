write_mem:
	arm-xilinx-linux-gnueabi-gcc write_mem.c -o write_mem_arm

upload_write_mem: write_mem
	scp write_mem_arm zedboard:~

.PHONY: write_mem upload_write_mem