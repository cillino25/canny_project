/*
 *  Single frame Canny Edge Detection application with sepImageFilter HW accelerator.
 *
 *  Author: Stefano Cillo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "../canny_mod/opencv_funcs.h"
#include "../canny_mod/global.h"

#include "openCV_HW_filter.h"

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"
#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"

using namespace my_Space;
using namespace cv;
using namespace std;

int ratio = 3;
struct timeval start, stop;


int main(int argc, char **argv) {
  int i=0;
  int threshold = 50;
  double sigma = 1.5;
  int gblur=5, canny=3;
  int custom = 0;
  char res[256] = "result.bmp";

  vdma_handle vdma_handle;
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

  //char in_file[]="lena_gray.bmp";
  char out_file[]="lena_blurred_2.bmp";
  int size=0, width=0, height=0;
  unsigned short *img_data;
  unsigned short *img_info;

  Mat dest;
  //Mat src_gray;

  unsigned char * src_data;

  if( argc < 3 ){
    printf("Not enough arguments.\nList of arguments: ./canny_mod_filter_sf <*source_img> <*threshold> <sigma> <gBlurMaskSize> <cannyMaskSize> <alg_type> <output_file>\n");
    return -1;
  }
  if(argc >= 2)
    threshold = atoi(argv[2]);

  if(argc > 3)
    sigma = (double)atof(argv[3]);

  if(argc > 4)
    gblur = atoi(argv[4]);

  if(argc > 5)
    canny = atoi(argv[5]);

  if(argc > 6)
    custom = atoi(argv[6]);

  if(argc > 7)
    strcpy(res, argv[7]);

  /************************************************************************/
  /* Load the first sacrifical frame in order to get video parameters
  /************************************************************************/

  // Load the image in the BGR format
  Mat src_gray_tmp = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  
  printf("Input image has %ld bytes per each pixel\n", src_gray_tmp.elemSize());
  if(!src_gray_tmp.data){ printf("No source data.\nExiting..\n"); return 1; }
  

  width = src_gray_tmp.cols;
  height = src_gray_tmp.rows;
  size = src_gray_tmp.cols * src_gray_tmp.rows * src_gray_tmp.channels();

  vdma_setup(&vdma_handle, page_size, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER);
  
  

  sepImageFilter_setupHandle(&filter_handle, &vdma_handle.vdmaHandler, page_size, AXI_SEPIMGFILTER);
  sepImageFilter_setupHandleParams(&filter_handle, width, height, k0_hz_coeffs, k0_vt_coeffs, norm0, k1_hz_coeffs, k1_vt_coeffs, norm1, k2_hz_coeffs, k2_vt_coeffs, norm2);
  sepImageFilter_setupFilter(&filter_handle);

  write_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, MEM2VDMA_BUFFER1_BASEADDR);
  if(((unsigned int *)write_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  read_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, VDMA2MEM_BUFFER1_BASEADDR);
  if(((unsigned int *)read_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }



  /************************************************************************/
  /* Load the frames that will be computed
  /************************************************************************/

  Mat src_gray(height, width, CV_16U);

  // Set image data at the write_fb address (to avoid another memory copy): next loaded frames will go directly to write_fb (?)
  src_gray.data = (unsigned char *)write_fb;
  dest.create(src_gray.size(), src_gray.type());
  dest.data = (unsigned char*)read_fb;
  

  /*********************************** LOOP BEGINS ******************************/

  src_gray_tmp = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

  // Final application will take frames from VideoCapture with pixels in CV_16U format, so this conversion will not be needed.
  // convertTo will put new frame into write_fb
  src_gray_tmp.convertTo(src_gray, CV_16U);


  GaussianBlur_HW(&vdma_handle, &filter_handle, width, height, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR);

  imwrite("lena_blurred_0.bmp", dest);

  return 0;






  //print_vdma_stats(&vdma_handle);
  printf("Setting up done.\n");
  

  memcpy(write_fb, src_data, src_gray.rows*src_gray.cols*2);
  
  
  

  // Write first image
  sepImageFilter_config(&filter_handle, 1, 0, 0);
  sepImageFilter_start(&filter_handle);
  
  while(sepImageFilter_running(&filter_handle) != 0);
  
  


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
  vdma_halt(&vdma_handle);

  munmap(read_fb, BUFFER_SIZE);
  munmap(write_fb, BUFFER_SIZE);
  printf("Bye!\n");

  return 0;
}

