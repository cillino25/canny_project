prec=SP
#prec=DP
roll=ROLL
#roll=UNROLL

linpack:
	riscv64-unknown-elf-gcc -D$(prec) -D$(roll) linpack.c -o linpack.riscv

spike:
	spike pk linpack.riscv

clean:
	rm -rf linpack.riscv