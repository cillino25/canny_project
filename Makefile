include Makefrag

cur_dir=.

canny_dir=$(cur_dir)/canny_mod
video_dir=$(cur_dir)/canny_video
opencv_arm_dir=/usr/share/opencv_armhf
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
	dd if=/dev/zero of=$(cur_dir)/rootcv.bin bs=1M count=256
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
## Canny_mod
canny_mod-arm:
	$(ARM_CPP) --static $(canny_dir)/gaussian_coefficients.c $(canny_dir)/Canny_test.cpp $(canny_dir)/opencv_Canny.cpp $(canny_dir)/opencv_Gblur.cpp $(canny_dir)/imageComparison.cpp -o $(canny_dir)/canny_mod_arm -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib `pkg-config --cflags --static --libs opencv`
cp-canny_mod-arm: canny_mod-arm
	scp $(canny_dir)/canny_mod_arm zedboard:/canny

canny_mod-arm_metrics:
	$(ARM_CPP) -DMETRICS=1 $(canny_dir)/gaussian_coefficients.c $(canny_dir)/Canny_test.cpp $(canny_dir)/opencv_Canny.cpp $(canny_dir)/opencv_Gblur.cpp $(canny_dir)/imageComparison.cpp -o $(canny_dir)/canny_mod_arm -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib `pkg-config --cflags --static --libs opencv`
cp-canny_mod-arm_metrics: canny_mod-arm_metrics
	scp $(canny_dir)/canny_mod_arm zedboard:/canny


canny_mod-rv:
	$(RV_CPP) -DRC=1 -I$(RISCV)/opencv/include -L$(RISCV)/opencv/lib -static $(canny_dir)/convolution.cpp $(canny_dir)/gaussian_coefficients.c $(canny_dir)/Canny_test.cpp $(canny_dir)/opencv_Canny.cpp $(canny_dir)/opencv_Gblur.cpp -o $(canny_dir)/canny_mod$(exe_type) `pkg-config --cflags --libs --static opencv`
cp-canny_mod-rv: canny_mod-rv
	cp $(canny_dir)/canny_mod$(exe_type) mnt-cv/

.PHONY: canny_mod-rv cp-canny_mod-rv  canny_mod-arm cp-canny_mod-arm

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
## Print or set VDMA registers

vdma-set-arm:
	$(ARM_CC) $(pipe_dir)/vdma_set.c $(pipe_dir)/vdma.c -o $(pipe_dir)/vdma_set

cp-vdma-set-arm: vdma-set-arm
	scp $(pipe_dir)/vdma_set zedboard:~

print-VDMA-arm:
	$(ARM_CC) $(pipe_dir)/print_vdma_stats.c $(pipe_dir)/vdma.c -o $(pipe_dir)/print-VDMA

cp-print-VDMA-arm: print-VDMA-arm
	scp $(pipe_dir)/print-VDMA zedboard:~

print-VDMA-rv:
	$(RV_CC) -DRC=1 $(pipe_dir)/print_vdma_stats.c $(pipe_dir)/vdma.c -o $(pipe_dir)/print-VDMA_$(exe_type)

cp-print-VDMA-rv: print-VDMA-rv
	cp $(pipe_dir)/print-VDMA_$(exe_type) mnt

.PHONY: print-VDMA-arm print-VDMA-rv cp-vdma-set-arm vdma-set-arm

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
	$(ARM_CPP) $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/test-filter-sf.cpp -o $(pipe_dir)/test-filter-arm-sf -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib `pkg-config --cflags --static --libs opencv`
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

canny-mod-filter-sf-rv:
	$(RV_CPP) -DRC=1 -I$RISCV/opencv/include -L$RISCV/opencv/lib -static $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/openCV_HW_filter.cpp $(pipe_dir)/gaussian_coefficients_arm.c $(pipe_dir)/canny_mod_filter_sf-rc.cpp -o $(pipe_dir)/canny_mod_filter_sf$(exe_type) `pkg-config --cflags --libs --static opencv`

cp-canny-mod-filter-sf-rv: canny-mod-filter-sf-rv
	cp $(pipe_dir)/canny_mod_filter_sf$(exe_type) mnt-cv/

run-canny-mod-filter-sf-rv: mount_rootcv cp-canny-mod-filter-sf-rv umount_rootcv
	spike --isa=RV64IMA +disk=rootcv.bin bbl_rv64ima /usr/share/riscv_src/linux-4.1.17/vmlinux_rv64ima

canny-mod-filter-sf-arm:
	$(ARM_CPP) -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/openCV_HW_filter.cpp $(pipe_dir)/gaussian_coefficients_arm.c $(pipe_dir)/canny_mod_filter_sf-arm.cpp -o $(pipe_dir)/canny_mod_filter_sf-arm `pkg-config --cflags --static --libs opencv`
cp-canny-mod-filter-sf-arm: canny-mod-filter-sf-arm
	scp $(pipe_dir)/canny_mod_filter_sf-arm zedboard:/canny
	#scp $(pipe_dir)/canny_mod_filter_sf-arm zedboard:~

canny-mod-filter-pipe: canny-mod-filter-sf-rv canny-mod-filter-sf-arm



canny-mod-filter-sf-arm-only:
	$(ARM_CPP) -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib $(pipe_dir)/sepImageFilter.c $(pipe_dir)/vdma.c $(pipe_dir)/openCV_HW_filter.cpp $(pipe_dir)/gaussian_coefficients_arm.c $(pipe_dir)/canny_mod_filter_sf__arm-only.cpp -o $(pipe_dir)/canny_mod_filter_sf__arm-only `pkg-config --cflags --static --libs opencv`
cp-canny-mod-filter-sf-arm-only: canny-mod-filter-sf-arm-only
	scp $(pipe_dir)/canny_mod_filter_sf__arm-only zedboard:/canny

.PHONY: canny-mod-filter-sf-rv cp-canny-mod-filter-sf-rv canny-mod-filter-sf-arm cp-canny-mod-filter-sf-arm canny-mod-filter-sf-arm-only cp-canny-mod-filter-sf-arm-only img-comparison
.PHONY: run-canny-mod-filter-sf-rv



####################################################################################
## Canny Video application

canny-video-arm_only:
	$(ARM_CPP) -I$(opencv_arm_dir)/include -L$(opencv_arm_dir)/lib $(video_dir)/canny_video__arm-only.cpp $(video_dir)/sepImageFilter.c $(video_dir)/vdma.c $(video_dir)/gaussian_coefficients.c $(video_dir)/opencv_Canny.cpp $(video_dir)/opencv_Gblur.cpp $(video_dir)/openCV_HW_filter.cpp -o $(video_dir)/canny_video__arm-only `pkg-config --cflags --static --libs opencv`

cp-canny-video-arm_only: canny-video-arm_only
	scp canny_video__arm-only zedboard:/canny

canny-video-rv:
	$(RV_CPP) -DRC=1 -I$(RISCV)/opencv/include -L$(RISCV)/opencv/lib -static $(video_dir)/sepImageFilter.c $(video_dir)/vdma.c $(video_dir)/openCV_HW_filter.cpp $(video_dir)/gaussian_coefficients.c $(video_dir)/canny_video-rc.cpp -o $(video_dir)/canny_video$(exe_type) `pkg-config --cflags --libs --static opencv`

cp-canny-video-rv: canny-video-rv
	cp $(video_dir)/canny_video$(exe_type) mnt-cv/


.PHONY: canny-video-arm_only cp-canny-video-arm_only canny-video-rv cp-canny-video-rv

####################################################################################
## Memory read and write utilities

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


#update_rootcv: mount_rootcv cp-canny_mod-rv umount_rootcv
#update_rootcv: mount_rootcv cp-canny-mod-filter-sf-rv umount_rootcv
#
#upload_rootcv: update_rootcv
#	scp $(cur_dir)/rootcv.bin zedboard:~/mnt/rootcv.bin

# Update ROOTCV.bin for single frame testing
#update_rootcv: mount_rootcv cp-canny-mod-filter-sf-rv umount_rootcv

#update_rootcv: mount_rootcv cp-canny_mod-rv umount_rootcv

update_rootcv: mount_rootcv cp-canny-video-rv umount_rootcv
upload_rootcv: update_rootcv
	scp $(cur_dir)/rootcv.bin zedboard:/canny/rootcv.bin

.PHONY: update_root cp_pipe_fr pipe_arm pipe_riscv update_root upload_root update_rootcv upload_rootcv

####################################################################################

cp-all: cp_write_mem cp_read_mem cp-print-VDMA-arm cp-vdma-set-arm
	scp canny_mod/lena_blurred_0.bmp canny_mod/lena_secret.bmp canny_mod/lena_gray.bmp zedboard:~
	
cp-opencv-libs:
	scp -r $(opencv_arm_dir)/bin/* zedboard:/usr/bin
	scp -r $(opencv_arm_dir)/include/* zedboard:/usr/include
	scp -r $(opencv_arm_dir)/lib/* zedboard:/usr/lib
	scp -r $(opencv_arm_dir)/share/* zedboard:/usr/share
	

clean:
	rm -rf $(pipe_dir)/*.o

.PHONY: clean