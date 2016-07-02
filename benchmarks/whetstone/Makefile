#printout=-DPRINTOUT
printout=

whetstone:
	riscv64-unknown-elf-gcc whetstone.c -o whetstone.riscv -lm $(printout)

spike: whetstone
	spike pk whetstone.riscv

clean:
	rm -rf whetstone.riscv

.PHONY: whetstone