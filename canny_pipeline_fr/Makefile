include ../Makefrag
include ../Makefile

riscv:
	$(RV_CC) pipe_riscv.c vdma.c -o pipe_riscv_$(exe_type)

arm:
	$(ARM_CC) pipe_arm.c vdma.c -o pipe_arm

clean:
	rm -rf pipe_riscv_$(exe_type) pipe_arm *.o


.PHONY: riscv arm