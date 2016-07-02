dhrystone:
	riscv64-unknown-elf-gcc -c dhry_1.c -o dhry_1.o -DHZ=25000000
	riscv64-unknown-elf-gcc -c dhry_2.c -o dhry_2.o -DHZ=25000000
	riscv64-unknown-elf-gcc dhry_1.o dhry_2.o -o dhrystone_mod.riscv

spike: dhrystone
	spike pk dhrystone_mod.riscv

clean:
	rm *.o dhrystone_mod.riscv

.PHONY: dhrystone