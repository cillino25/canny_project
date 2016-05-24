write_mem:
	arm-xilinx-linux-gnueabi-gcc write_mem.c -o write_mem_arm

read_mem:
	arm-xilinx-linux-gnueabi-gcc read_mem.c -o read_mem

upload_write_mem: write_mem
	scp write_mem_arm zedboard:~

upload_read_mem: read_mem
	scp read_mem zedboard:~

.PHONY: write_mem upload_write_mem read_mem
