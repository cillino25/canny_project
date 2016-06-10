/*
 *  Single frame test application for sepImageFilter HW accelerator.
 *
 */



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>


#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"
#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"

using namespace cv;
using namespace std;


int main() {
  int h, j, i;
  vdma_handle handle;
  sepimgfilter_handle filter_handle;

 
  int mem;
  void *read_fb, *write_fb;
  unsigned int page_size = sysconf(_SC_PAGESIZE);

  // N=5, s=1
  unsigned char k0_hz_coeffs[] = {0, 1, 4, 7, 4, 1, 0};
  unsigned char k0_vt_coeffs[] = {0, 1, 4, 7, 4, 1, 0};
  unsigned int  norm0 = 289;

  // N=3, s=0.4
  unsigned char k1_hz_coeffs[] = {0, 0, 1, 23, 1, 0, 0};
  unsigned char k1_vt_coeffs[] = {0, 0, 1, 23, 1, 0, 0};
  unsigned int  norm1 = 625;

  // N=7, s=1
  unsigned char k2_hz_coeffs[] = {1, 12, 55, 90, 55, 12, 1};
  unsigned char k2_vt_coeffs[] = {1, 12, 55, 90, 55, 12, 1};
  unsigned int  norm2 = 51076;

  char in_file[]="lena_gray.bmp";
  char out_file[]="lena_blurred_2.bmp";
  int size=0, width=0, height=0;
  unsigned short *img_data;
  unsigned short *img_info;
  
  Mat src = imread(in_file, 0);
  //printf("src bytes/pixel: %d\n", src.elemSize());
  src.convertTo(src, CV_16U);
  //printf("src bytes/pixel: %d\n", src.elemSize());
  width = src.cols;
  height = src.rows;
  size = src.cols * src.rows * src.channels();
  printf("w=%d, h=%d, tot pixels=%d\n", width, height, size);
  Mat dest;
  dest.create(src.size(), src.type());

  unsigned char * src_data = src.data;


  int frame_len = 2 * width * height;
  printf("frame len = %d\n", frame_len);
  printf("%s opened: w=%d, h=%d, tot pixels=%d\n", in_file, width, height, size);

  vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER);
  
  write_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, handle.vdmaHandler, MEM2VDMA_BUFFER1_BASEADDR);
  if(((unsigned int *)write_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  read_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, handle.vdmaHandler, VDMA2MEM_BUFFER1_BASEADDR);
  if(((unsigned int *)read_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  sepImageFilter_setup_handle(&filter_handle, &handle.vdmaHandler, page_size, AXI_SEPIMGFILTER);
  sepImageFilter_setImageParams(&filter_handle, width, height);
  
  sepImageFilter_setKernel0_HzCoeffs(&filter_handle, k0_hz_coeffs);
  sepImageFilter_setKernel0_VtCoeffs(&filter_handle, k0_vt_coeffs);
  sepImageFilter_setKernel0_NormalizationFactor(&filter_handle, norm0);

  sepImageFilter_setKernel1_HzCoeffs(&filter_handle, k1_hz_coeffs);
  sepImageFilter_setKernel1_VtCoeffs(&filter_handle, k1_vt_coeffs);
  sepImageFilter_setKernel1_NormalizationFactor(&filter_handle, norm1);

  sepImageFilter_setKernel2_HzCoeffs(&filter_handle, k2_hz_coeffs);
  sepImageFilter_setKernel2_VtCoeffs(&filter_handle, k2_vt_coeffs);
  sepImageFilter_setKernel2_NormalizationFactor(&filter_handle, norm2);

  
  sepImageFilter_setup(&filter_handle);

  //print_vdma_stats(&handle);
  printf("Setting up done.\n");
  

  memcpy(write_fb, src_data, src.rows*src.cols*2);
  
  vdma_start_triple_buffering_mod(&handle);
  

  // Write first image
  sepImageFilter_config(&filter_handle, 1, 0, 0);
  sepImageFilter_start(&filter_handle);
  
  while(sepImageFilter_running(&filter_handle) != 0);
  
  dest.data = (unsigned char*)read_fb;
  imwrite("lena_blurred_0.bmp", dest);


  // Write second image
  sepImageFilter_config(&filter_handle, 1, 0, 1);
  sepImageFilter_start(&filter_handle);
  
  while(sepImageFilter_running(&filter_handle) != 0);
  
  dest.data = (unsigned char*)read_fb;
  imwrite("lena_blurred_1.bmp", dest);


  // Write third image
  sepImageFilter_config(&filter_handle, 1, 0, 2);
  sepImageFilter_start(&filter_handle);
  
  while(sepImageFilter_running(&filter_handle) != 0);
  
  dest.data = (unsigned char*)read_fb;
  imwrite("lena_blurred_2.bmp", dest);



  // Halt VDMA and unmap memory ranges
  vdma_halt(&handle);

  munmap(read_fb, BUFFER_SIZE);
  munmap(write_fb, BUFFER_SIZE);
  printf("Bye!\n");

  return 0;
}

