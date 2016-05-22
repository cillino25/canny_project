ifeq ($(RISCV_TYPE),riscv)
	EXE_TYPE=_rv64g
else
	EXE_TYPE=_rv64ima
endif


opt=-O2
#opt=

torture_objs=torture_test_arm-bare torture_test_arm-linux torture_test_riscv-bare

mount_root:
	mount -o loop $(TOP)/root.bin $(TOP)/mnt/
umount_root:
	umount $(TOP)/mnt/
update_root:
	cp torture_test_riscv-linux$(EXE_TYPE) $(TOP)/mnt/torture/

util:
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -D__ASSEMBLY__=1 -c -I./../env -I./common -I./torture ./common/crt.S -o crt.o
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -c -I./../env -I./common -I./torture ./common/syscalls.c -o syscalls.o

reset_mem:
	arm-xilinx-linux-gnueabi-gcc reset_mem.c -o reset_mem_arm

torture_arm:
	arm-xilinx-linux-gnueabi-gcc torture_arm-bare.c -o torture_test_arm-bare
	arm-xilinx-linux-gnueabi-gcc torture_arm-linux.c -o torture_test_arm-linux

torture_riscv: util
	riscv64-unknown-elf-gcc -static -std=gnu99 $(opt) -ffast-math -fno-common -fno-builtin-printf -DPREALLOCATE=1 -DHOST_DEBUG=0 -c -I./../env -I./common -I./torture ./torture_riscv.c -o torture_test_riscv.o
	riscv64-unknown-elf-gcc -T ./common/test.ld -I./../env -I./common -I./torture  torture_test_riscv.o  syscalls.o  crt.o -o torture_test_riscv-bare -nostdlib -nostartfiles -ffast-math -lc -lgcc

torture_riscv-linux:
ifeq ($(RISCV_TYPE),riscv)
	riscv64-unknown-linux-gnu-gcc torture_riscv-linux.c -o torture_test_riscv-linux$(EXE_TYPE)
else
	riscv64-unknown-linux-gnu-gcc torture_riscv-linux.c -o torture_test_riscv-linux$(EXE_TYPE)
endif

torture: torture_arm torture_riscv torture_riscv-linux

upload_torture: torture
	scp $(torture_objs) zedboard:~/mnt/torture

upload_reset: reset_mem
	scp reset_mem_arm zedboard:~/mnt/torture

upload_root: mount_root torture_riscv-linux update_root umount_root
	scp $(TOP)/root.bin zedboard:~/mnt

upload: upload_reset upload_torture upload_root

clean:
	rm -f reset_mem_arm torture_test_arm-bare torture_test_arm-linux torture_test_riscv-bare torture_test_riscv-linux$(EXE_TYPE) *.o

.PHONY: clean upload upload_torture upload_reset torture torture_arm torture_riscv torture_riscv-linux util reset_mem