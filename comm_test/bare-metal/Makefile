opt=

#fast_math=-ffast-math


util:
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -D__ASSEMBLY__=1 -c -I./../env -I./common -I./bare_metal ./common/crt.S -o crt.o
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -c -I./../env -I./common -I./bare_metal ./common/syscalls.c -o syscalls.o

reset_mem: util
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -c -I./../env -I./common -I./bare_metal ./reset_mem.c -o reset_mem.o
	riscv64-unknown-elf-gcc -T ./common/test.ld -I./../env -I./common -I./bare_write  reset_mem.o  syscalls.o  crt.o -o reset_mem -nostdlib -nostartfiles -ffast-math -lc -lgcc

bare_write: util
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -c -I./../env -I./common -I./bare_metal ./bare_write.c -o bare_write.o
	riscv64-unknown-elf-gcc -T ./common/test.ld -I./../env -I./common -I./bare_write  bare_write.o  syscalls.o  crt.o -o bare_write -nostdlib -nostartfiles -ffast-math -lc -lgcc


upload_zed: bare_write reset_mem
	scp bare_write zedboard:~
	scp reset_mem zedboard:~

clean:
	rm -f bare_write *.o reset_mem

.PHONY: util bare_write reset_mem util