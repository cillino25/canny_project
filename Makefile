include Makefrag

cur_dir=.

opencv_arm_dir=/usr/share/opencv_arm
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
## Complete ARM-RC-VDMA pipeline

pipe_riscv:
	$(RV_CC) $(pipefr_dir)/pipe_riscv.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/pipe_riscv_$(exe_type)

pipe_arm:
	$(ARM_CC) $(pipefr_dir)/pipe_arm.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/pipe_arm

cp_pipe_fr: pipe_arm pipe_riscv
	scp $(pipefr_dir)/pipe_arm zedboard:~
	cp $(pipefr_dir)/pipe_riscv_$(exe_type) mnt/pipe_fr

####################################################################################
## Print VDMA registers

print-VDMA-arm:
	$(ARM_CC) $(pipefr_dir)/print_vdma_stats.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/print-VDMA

cp-print-VDMA-arm: print-VDMA-arm
	scp $(pipefr_dir)/print-VDMA zedboard:~

print-VDMA-rv:
	$(RV_CC) -DRC=1 $(pipefr_dir)/print_vdma_stats.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/print-VDMA_$(exe_type)

cp-print-VDMA-rv: print-VDMA-rv
	cp $(pipefr_dir)/print-VDMA_$(exe_type) mnt

.PHONY: print-VDMA-arm print-VDMA-rv
###################################################################################
## Basic VDMA test (alternative folder)

test-VDMA-arm:
	$(ARM_CC) $(test-VDMA_dir)/test-VDMA.c $(test-VDMA_dir)/vdma.c -o $(test-VDMA_dir)/test-VDMA

cp-test-VDMA-arm: test-VDMA-arm
	scp $(test-VDMA_dir)/test-VDMA zedboard:~

test-VDMA-rv:
	$(RV_CC) $(test-VDMA_dir)/test-VDMA.c $(test-VDMA_dir)/vdma.c -o $(test-VDMA_dir)/test-VDMA_$(exe_type)

cp-test-VDMA: test-VDMA-rv
	cp $(test-VDMA_dir)/test-VDMA_$(exe_type) mnt/

.PHONY: test-VDMA-rv cp-test-VDMA

####################################################################################
## Free running version of VDMA test

test-VDMA-arm-fr:
	$(ARM_CC) $(pipefr_dir)/test-VDMA.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/test-VDMA

cp-test-VDMA-arm-fr: cp_read_mem cp_write_mem test-VDMA-arm-fr
	scp $(pipefr_dir)/test-VDMA zedboard:~

test-VDMA-rv-fr:
	$(RV_CC) -DRC=1 $(pipefr_dir)/test-VDMA.c $(pipefr_dir)/vdma.c -o $(pipefr_dir)/test-VDMA_$(exe_type)

cp-test-VDMA-rv-fr: test-VDMA-rv-fr
	cp $(pipefr_dir)/test-VDMA_$(exe_type) mnt/

.PHONY: test-VDMA-rv-fr cp-test-VDMA-rv-fr test-VDMA-arm-fr cp-test-VDMA-arm-fr

####################################################################################
## VDMA test with fsync and filter

test-filter-arm-cv:
	$(ARM_CPP) $(pipefr_dir)/sepImageFilter.c $(pipefr_dir)/vdma.c $(pipefr_dir)/test-filter.cpp -o $(pipefr_dir)/test-filter-opencv -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib `pkg-config --cflags --libs opencv`
cp-test-filter-arm-cv: test-filter-arm-cv
	scp $(pipefr_dir)/test-filter-opencv zedboard:~/

test-filter-arm:
	$(ARM_CC) -c -std=c99 -o $(pipefr_dir)/vdma.o $(pipefr_dir)/vdma.c 
	$(ARM_CC) -c -std=c99 -o $(pipefr_dir)/sepImageFilter.o $(pipefr_dir)/sepImageFilter.c 
	$(ARM_CPP) -c -std=c++0x -o $(pipefr_dir)/test-filter-arm.o $(pipefr_dir)/test-filter.cpp
	$(ARM_CPP) -o $(pipefr_dir)/test-filter-arm $(pipefr_dir)/test-filter-arm.o $(pipefr_dir)/vdma.o $(pipefr_dir)/sepImageFilter.o
cp-test-filter-arm: test-filter-arm
	scp $(pipefr_dir)/test-filter-arm zedboard:~

cp-lena-gray:
	scp canny_mod/lena_gray.bmp zedboard:~
	
test-filter-rv:
	$(RV_CC) $(pipefr_dir)/test-filter.c $(pipefr_dir)/vdma.c $(pipefr_dir)/sepImageFilter.c -o $(pipefr_dir)/test-filter_$(exe_type)
cp-test-filter-rv: test-filter-rv
	cp $(pipefr_dir)/test-filter_$(exe_type) mnt/


.PHONY: test-filter-arm cp-test-filter-arm test-filter-rv cp-test-filter-rv
####################################################################################
## Write and read memory utilities

write_mem:
	$(ARM_CC) $(comm_dir)/write_mem.c -o $(comm_dir)/write_mem

cp_write_mem: write_mem
	scp $(comm_dir)/write_mem zedboard:~

write_mem-rv:
	$(RV_CC) $(comm_dir)/write_mem.c -o $(comm_dir)/write_mem_$(exe_type)

cp_write_mem-rv: write_mem-rv
	cp $(comm_dir)/write_mem_$(exe_type) mnt/


read_mem:
	$(ARM_CC) $(comm_dir)/read_mem.c -o $(comm_dir)/read_mem

cp_read_mem: read_mem
	scp $(comm_dir)/read_mem zedboard:~

.PHONY: read_mem cp_read_mem write_mem cp_write_mem write_mem-rv cp_write_mem-rv
####################################################################################

update_root: mount_root cp_pipe_fr cp_read_mem cp-test-VDMA cp-test-VDMA-rv-fr cp-print-VDMA-rv cp_write_mem-rv
	umount $(cur_dir)/mnt/

upload_root: update_root cp-print-VDMA-arm cp_read_mem cp-test-VDMA-arm-fr
	scp root.bin zedboard:~/mnt/root.bin

.PHONY: update_root cp_pipe_fr pipe_arm pipe_riscv update_root upload_root

####################################################################################

cp-all: cp_write_mem cp_read_mem cp-print-VDMA-arm
	scp canny_mod/lena_blurred_0.bmp zedboard:~

cp-opencv-libs:
	scp -r $(opencv_arm_dir)/bin/* zedboard:/usr/bin
	scp -r $(opencv_arm_dir)/include/* zedboard:/usr/include
	scp -r $(opencv_arm_dir)/lib/* zedboard:/usr/lib
	scp -r $(opencv_arm_dir)/share/* zedboard:/usr/share
	

clean:
	rm -rf $(pipefr_dir)/*.o

.PHONY: clean