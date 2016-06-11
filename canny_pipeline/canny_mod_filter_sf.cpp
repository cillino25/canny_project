/*
 *  Single frame Canny Edge Detection application with sepImageFilter HW accelerator.
 *
 *  Author: Stefano Cillo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
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
  int thresh = 50, ratio=3;
  double sigma = 1.5;
  int gblur=5, canny=3;
  int custom = 0;
  char res[256] = "result.bmp";

  vdma_handle vdma_handle;
  sepimgfilter_handle filter_handle;
 
  int mem;
  void *gb_in_fb, *gb_out_fb, *sobel_dx_out_fb, *sobel_dy_out_fb, *out_img_fb;
  unsigned int gb_in_fb_addr        = MEM2VDMA_BUFFER1_BASEADDR;
  unsigned int gb_out_fb_addr       = MEM2VDMA_BUFFER2_BASEADDR;
  unsigned int sobel_dx_out_fb_addr = MEM2VDMA_BUFFER3_BASEADDR;
  unsigned int sobel_dy_out_fb_addr = VDMA2MEM_BUFFER1_BASEADDR;
  unsigned int out_img_fb_addr      = VDMA2MEM_BUFFER2_BASEADDR;

  unsigned int page_size = sysconf(_SC_PAGESIZE);

  // N=5, s=1
  int k0_hz_coeffs[] = {0, 1, 4, 7, 4, 1, 0};
  int k0_vt_coeffs[] = {0, 1, 4, 7, 4, 1, 0};
  unsigned int  norm0 = 289;

  // N=3, s=0.4
  int k1_hz_coeffs[] = {0, 0, -1, 0, 1, 0, 0};
  int k1_vt_coeffs[] = {0, 0, 1, 2, 1, 0, 0};
  unsigned int  norm1 = 0;

  // N=7, s=1
  int k2_hz_coeffs[] = {0, 0, 1, 2, 1, 0, 0};
  int k2_vt_coeffs[] = {0, 0, -1, 0, 1, 0, 0};
  unsigned int  norm2 = 0;

  //char in_file[]="lena_gray.bmp";
  char out_file[]="lena_blurred_2.bmp";
  int size=0, width=0, height=0;
  unsigned short *img_data;
  unsigned short *img_info;

  
  

  unsigned char * src_data;

  if( argc < 3 ){
    printf("Not enough arguments.\nList of arguments: ./canny_mod_filter_sf <*source_img> <*threshold> <sigma> <gBlurMaskSize> <cannyMaskSize> <alg_type> <output_file>\n");
    return -1;
  }
  if(argc >= 2)
    thresh = atoi(argv[2]);

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

  gb_in_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, gb_in_fb_addr);
  if(((unsigned int *)gb_in_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  gb_out_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, gb_out_fb_addr);
  if(((unsigned int *)gb_out_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  sobel_dx_out_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, sobel_dx_out_fb_addr);
  if(((unsigned int *)sobel_dx_out_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  sobel_dy_out_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, sobel_dy_out_fb_addr);
  if(((unsigned int *)sobel_dy_out_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  out_img_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vdma_handle.vdmaHandler, out_img_fb_addr);
  if(((unsigned int *)out_img_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  /************************************************************************/
  /* Load the frames that will be computed
  /************************************************************************/
  Mat gb_dest;
  Mat src_gray(height, width, CV_16U);
  Mat dx(height, width, CV_16SC(1));
  Mat dy(height, width, CV_16SC(1));
  Mat dest(height, width, CV_8U);

  // Set image data at the gb_in_fb address (to avoid another memory copy): next loaded frames will go directly to gb_in_fb (?)
  src_gray.data = (unsigned char *)gb_in_fb;
  gb_dest.create(src_gray.size(), src_gray.type());
  gb_dest.data = (unsigned char*)gb_out_fb;

  dx.data = (unsigned char*)sobel_dx_out_fb;
  dy.data = (unsigned char*)sobel_dy_out_fb;

  //dest.data = (unsigned char*)out_img_fb;
  
  /******************************************************************************/
  /*********************************** LOOP BEGINS ******************************/

  src_gray_tmp = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

  // Final application will take frames from VideoCapture with pixels in CV_16U format, so this conversion will not be needed.
  // convertTo will put new frame into gb_in_fb
  src_gray_tmp.convertTo(src_gray, CV_16U);

  gettimeofday(&start, NULL);
  GaussianBlur_HW(&vdma_handle, &filter_handle, width, height, gb_in_fb_addr, gb_out_fb_addr);
  gettimeofday(&stop, NULL);
  printf("\nGaussianBlur_HW wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);

  imwrite("lena_blurred_0.bmp", gb_dest);

  gettimeofday(&start, NULL);
  Canny_HW(&vdma_handle, &filter_handle, dest, dx, dy, width, height, gb_out_fb_addr, sobel_dx_out_fb_addr, sobel_dy_out_fb_addr, (double)thresh, ((double)thresh*ratio), canny, false );
  gettimeofday(&stop, NULL);
  printf("Canny Edge Detector wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  //SobelDx_HW(&vdma_handle, &filter_handle, width, height, gb_out_fb_addr, sobel_dx_out_fb_addr);
  //SobelDy_HW(&vdma_handle, &filter_handle, width, height, gb_out_fb_addr, sobel_dy_out_fb_addr);  

  imwrite("result.bmp", dest);
  imwrite("dx.bmp", dx);
  imwrite("dy.bmp", dy);





  



  // Halt VDMA and unmap memory ranges
  vdma_halt(&vdma_handle);

  munmap(gb_out_fb, BUFFER_SIZE);
  munmap(gb_in_fb, BUFFER_SIZE);
  printf("Bye!\n");

  return 0;
}

