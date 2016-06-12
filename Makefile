include Makefrag

cur_dir=.

canny_dir=$(cur_dir)/canny_mod
opencv_arm_dir=/usr/share/opencv_arm
test-VDMA_dir=$(cur_dir)/VDMA_test
pipe_dir=$(cur_dir)/canny_pipeline
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

mount_rootcv:
	mount -o loop $(cur_dir)/rootcv.bin $(cur_dir)/mnt-cv/
umount_rootcv:
	umount $(cur_dir)/mnt-cv/

.PHONY: create_root root.bin rootcv.bin mount umount_root mount_rootcv umount_rootcv



####################################################################################
## Canny_mod Rocketchip
canny_mod-rv:
	riscv64-unknown-linux-gnu-g++ -DRC=1 -I$RISCV/opencv/include -L$RISCV/opencv/lib -static $(canny_dir)/convolution_arm.cpp $(canny_dir)/gaussian_coefficients_arm.c $(canny_dir)/Canny_test_arm.cpp $(canny_dir)/opencv_Canny_arm.cpp $(canny_dir)/opencv_Gblur_arm.cpp -o $(canny_dir)/canny_mod$(exe_type) `pkg-config --cflags --libs --static opencv`
cp-canny_mod-rv: canny_mod-rv
	cp $(canny_dir)/canny_mod$(exe_type) mnt-cv/

update_rootcv: mount_rootcv cp-canny_mod-rv umount_rootcv

upload_rootcv: update_rootcv
	scp $(cur_dir)/rootcv.bin zedboard:~/mnt/rootcv.bin

.PHONY: canny_mod-rv cp-canny_mod-rv update_rootcv upload_rootcv

####################################################################################
## Complete ARM-RC-VDMA pipeline

pipe_riscv:
	$(RV_CC) $(pipe_dir)/pipe_riscv.c $(pipe_dir)/vdma.c -o $(pipe_dir)/pipe_riscv_$(exe_type)

pipe_arm:
	$(ARM_CC) $(pipe_dir)/pipe_arm.c $(pipe_dir)/vdma.c -o $(pipe_dir)/pipe_arm

cp_pipe_fr: pipe_arm pipe_riscv
	scp $(pipe_dir)/pipe_arm zedboard:~
	cp $(pipe_dir)/pipe_riscv_$(exe_type) mnt/pipe_fr

####################################################################################
## Print VDMA registers

print-VDMA-arm:
	$(ARM_CC) $(pipe_dir)/print_vdma_stats.c $(pipe_dir)/vdma.c -o $(pipe_dir)/print-VDMA

cp-print-VDMA-arm: print-VDMA-arm
	scp $(pipe_dir)/print-VDMA zedboard:~

print-VDMA-rv:
	$(RV_CC) -DRC=1 $(pipe_dir)/print_vdma_stats.c $(pipe_dir)/vdma.c -o $(pipe_dir)/print-VDMA_$(exe_type)

cp-print-VDMA-rv: print-VDMA-rv
	cp $(pipe_dir)/print-VDMA_$(exe_type) mnt

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
	$(ARM_CC) $(pipe_dir)/test-VDMA.c $(pipe_dir)/vdma.c -o $(pipe_dir)/test-VDMA

cp-test-VDMA-arm-fr: cp_read_mem cp_write_mem test-VDMA-arm-fr
	scp $(pipe_dir)/test-VDMA zedboard:~

test-VDMA-rv-fr:
	$(RV_CC) -DRC=1 $(pipe_dir)/test-VDMA.c $(pipe_dir)/vdma.c -o $(pipe_dir)/test-VDMA_$(exe_type)

cp-test-VDMA-rv-fr: test-VDMA-rv-fr
	cp $(pipe_dir)/test-VDMA_$(exe_type) mnt/

.PHONY: test-VDMA-rv-fr cp-test-VDMA-rv-fr test-VDMA-arm-fr cp-test-VDMA-arm-fr

####################################################################################
## sepImageFilter single frame complete application.
## 
## Main purpose is to test filter correct functionalities (three different configurations)
##

test-filter-sf:
	$(CPP) -I/usr/local/include -L/usr/local/lib -I/home/stefano/TESI/openCV/include $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/test-filter-sf.cpp -o $(pipe_dir)/test-filter-sf `pkg-config --cflags --libs opencv`

test-filter-sf-arm-cv:
	$(ARM_CPP) $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/test-filter-sf.cpp -o $(pipe_dir)/test-filter-arm-sf -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib `pkg-config --cflags --libs opencv`
cp-test-filter-sf-arm-cv: test-filter-sf-arm-cv
	scp $(pipe_dir)/test-filter-arm-sf zedboard:~/


cp-lena-gray:
	scp canny_mod/lena_gray.bmp zedboard:~
	
test-filter-rv:
	$(RV_CC) $(pipe_dir)/test-filter.c $(pipe_dir)/vdma.c $(pipe_dir)/sepImageFilter.c -o $(pipe_dir)/test-filter_$(exe_type)
cp-test-filter-rv: test-filter-rv
	cp $(pipe_dir)/test-filter_$(exe_type) mnt/

.PHONY: test-filter-arm cp-test-filter-arm test-filter-rv cp-test-filter-rv

#####################################################################################################
## Modified Canny Edge Detection algorithm with sepImageFilter acceleration - single frame version.
## 
## All convolutions will be executed from the HW accelerator (sepImageFilter), with 3 different calls:
## first the global setting is done, and at each call the correct kernel is selected through a single-register write.
##
img-comparison:
	$(CPP) -I/usr/local/include -L/usr/local/lib -I/home/stefano/TESI/openCV/include $(canny_dir)/imageComparison.cpp -o $(canny_dir)/imageComparison `pkg-config --cflags --libs opencv`

canny-mod-filter-sf-arm:
	$(ARM_CPP) -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/openCV_HW_filter.cpp $(pipe_dir)/gaussian_coefficients_arm.c $(pipe_dir)/canny_mod_filter_sf.cpp -o $(pipe_dir)/canny_mod_filter_sf `pkg-config --cflags --libs opencv`
cp-canny-mod-filter-sf-arm: canny-mod-filter-sf-arm
	scp $(pipe_dir)/canny_mod_filter_sf zedboard:~

.PHONY: canny-mod-filter-sf canny-mod-filter-sf-arm cp-canny-mod-filter-sf-arm img-comparison

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
	scp canny_mod/lena_blurred_0.bmp canny_mod/lena_secret.bmp canny_mod/lena_gray.bmp zedboard:~
	
cp-opencv-libs:
	scp -r $(opencv_arm_dir)/bin/* zedboard:/usr/bin
	scp -r $(opencv_arm_dir)/include/* zedboard:/usr/include
	scp -r $(opencv_arm_dir)/lib/* zedboard:/usr/lib
	scp -r $(opencv_arm_dir)/share/* zedboard:/usr/share
	

clean:
	rm -rf $(pipe_dir)/*.o

.PHONY: clean