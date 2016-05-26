include Makefrag

cur_dir=.

test-VDMA_dir=$(cur_dir)/VDMA_test
pipefr_dir=$(cur_dir)/canny_pipeline_fr
comm_dir=$(cur_dir)/comm_test/arm


create_root:
	dd if=/dev/zero of=root.bin bs=1M count=64
	mkfs.ext2 -F root.bin
	mkdir mnt
	mount -o loop root.bin mnt
	mkdir -p mnt/bin mnt/etc mnt/dev mnt/lib mnt/proc mnt/sbin mnt/sys mnt/tmp mnt/usr mnt/usr/bin mnt/usr/lib mnt/usr/sbin mnt/usr/include mnt/home mnt/home/root
	cp inittab mnt/etc/
	cp busybox-1.24.1/busybox mnt/bin/
	ln mnt/bin/busybox mnt/sbin/init
	umount mnt

root.bin:
	dd if=/dev/zero of=$(cur_dir)/root.bin bs=1M count=64
	mkfs.ext2 -F $(cur_dir)/root.bin
rootcv.bin:
	dd if=/dev/zero of=$(cur_dir)/rootcv.bin bs=1M count=128
	mkfs.ext2 -F $(cur_dir)/rootcv.bin


mount_root:
	mount -o loop $(cur_dir)/root.bin $(cur_dir)/mnt/
umount_root:
	umount $(cur_dir)/mnt/

.PHONY: create_root root.bin rootcv.bin mount umount_root


####################################################################################

pipe_riscv:
	$(RV_CC) $(pipefr_dir)/pipe_riscv.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/pipe_riscv_$(exe_type)

pipe_arm:
	$(ARM_CC) $(pipefr_dir)/pipe_arm.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/pipe_arm

cp_pipe_fr: pipe_arm pipe_riscv
	scp $(pipefr_dir)/pipe_arm zedboard:~
	cp $(pipefr_dir)/pipe_riscv_$(exe_type) mnt/pipe_fr

####################################################################################

test-VDMA-arm-fr:
	$(ARM_CC) $(pipefr_dir)/test-VDMA.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/test-VDMA

cp-test-VDMA-arm-fr: cp_read_mem test-VDMA-arm-fr
	scp $(pipefr_dir)/test-VDMA zedboard:~

test-VDMA-rv-fr:
	$(RV_CC) -DRC=1 $(pipefr_dir)/test-VDMA.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/test-VDMA_$(exe_type)

cp-test-VDMA-rv-fr: test-VDMA-rv-fr
	cp $(pipefr_dir)/test-VDMA_$(exe_type) mnt/
	umount $(cur_dir)/mnt/

.PHONY: test-VDMA-rv-fr cp-test-VDMA-rv-fr test-VDMA-arm-fr cp-test-VDMA-arm-fr

####################################################################################


test-VDMA-arm:
	$(ARM_CC) $(test-VDMA_dir)/test-VDMA.c $(test-VDMA_dir)/vdma.c -o $(test-VDMA_dir)/test-VDMA

cp-test-VDMA-arm: test-VDMA-arm
	scp $(test-VDMA_dir)/test-VDMA zedboard

test-VDMA-rv:
	$(RV_CC) $(test-VDMA_dir)/test-VDMA.c $(test-VDMA_dir)/vdma.c -o $(test-VDMA_dir)/test-VDMA_$(exe_type)

cp-test-VDMA: test-VDMA-rv
	cp $(test-VDMA_dir)/test-VDMA_$(exe_type) mnt/

.PHONY: test-VDMA-rv cp-test-VDMA

####################################################################################

read_mem:
	$(ARM_CC) $(comm_dir)/read_mem.c -o $(comm_dir)/read_mem

cp_read_mem: read_mem
	scp $(comm_dir)/read_mem zedboard:~

.PHONY: read_mem cp_read_mem
####################################################################################

update_root: mount_root cp_pipe_fr cp_read_mem cp-test-VDMA cp-test-VDMA-rv-fr
	umount $(cur_dir)/mnt/

upload_root: update_root	
	scp root.bin zedboard:~/mnt/root.bin

.PHONY: update_root cp_pipe_fr pipe_arm pipe_riscv update_root upload_root

####################################################################################

