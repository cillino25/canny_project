/*
 *  Single frame Canny Edge Detection application with sepImageFilter HW accelerator.
 *
 *  Author: Stefano Cillo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include "../canny_mod/global.h"


#include "openCV_HW_filter.h"

#include "gaussian_coefficients.h"

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"
#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"

//using namespace my_Space;
using namespace cv;
using namespace std;

int ratio = 3;
struct timeval start, stop;


int main(int argc, char **argv) {
  int i=0;
  int devmem = 0;
  int thresh = 50, ratio=3;
  double sigma = 1;
  int nGblur=5, nCanny=3;
  int custom = 0;
  int polls = 1;
  char in_img[256] = "640x480.bmp";
  char res[256] = "result.bmp";


  int mem;
  void *sobel_dx_out_fb, *sobel_dy_out_fb, *out_img_fb, *poll_mmap;
  unsigned int sobel_dx_out_fb_addr = MEM2VDMA_BUFFER3_BASEADDR;
  unsigned int sobel_dy_out_fb_addr = VDMA2MEM_BUFFER1_BASEADDR;
  unsigned int out_img_fb_addr      = VDMA2MEM_BUFFER2_BASEADDR;

  unsigned int page_size = 4096;//sysconf(_SC_PAGESIZE);


  //char in_file[]="lena_gray.bmp";
  char out_file[]="lena_blurred_2.bmp";
  int size=0, width=0, height=0;
  unsigned short *img_data;
  unsigned short *img_info;

  unsigned char * src_data;


  if(argc > 1)
    thresh = atoi(argv[1]);

  if(argc > 2)
    sigma = (double)atof(argv[2]);

  if(argc > 3)
    nGblur = atoi(argv[3]);

  if(argc > 4)
    nCanny = atoi(argv[4]);

  if(argc > 5)
    custom = atoi(argv[5]);
  
  if(argc > 6)
    polls = atoi(argv[6]);

  if((nGblur!=3)&&(nGblur!=5)&&(nGblur!=7)){ printf("nGblur mask size must be in {3,5,7}.\n"); return -1; }
  if((nGblur==3)&&((sigma<0.4)||(sigma>1))){ printf("With GBlur_size=3 sigma must be 0.4 < sigma < 1.\n"); return -1; }
  if((nGblur==5)&&((sigma<0.8)||(sigma>1.3))){ printf("With GBlur_size=5 sigma must be 0.8 < sigma < 1.3.\n"); return -1; }
  if((nGblur==7)&&((sigma<1)||(sigma>1.5))){ printf("With GBlur_size=7 sigma must be 1 < sigma < 1.5.\n"); return -1; }
  if((nCanny!=3)&&(nCanny!=5)&&(nCanny!=7)){ printf("nCanny mask size must be in {3,5,7}.\n"); return -1; }

  //printf("Input image will be %s\n", in_img);

  if((devmem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  printf("/dev/mem opened\n");
  
  

  /************************************************************************/
  /* Load the first sacrifical frame in order to get video parameters
  /************************************************************************/

  // Load the image in the BGR format
  Mat src_gray_tmp = imread(in_img, CV_LOAD_IMAGE_GRAYSCALE);
  
  printf("Input image has %ld bytes per each pixel\n", src_gray_tmp.elemSize());
  if(!src_gray_tmp.data){ printf("No source data.\nExiting..\n"); return 1; }
  

  width = src_gray_tmp.cols;
  height = src_gray_tmp.rows;
  size = src_gray_tmp.cols * src_gray_tmp.rows * src_gray_tmp.channels();

  

  sobel_dx_out_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, sobel_dx_out_fb_addr);
  if(((unsigned int *)sobel_dx_out_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  sobel_dy_out_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, sobel_dy_out_fb_addr);
  if(((unsigned int *)sobel_dy_out_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  out_img_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, out_img_fb_addr);
  if(((unsigned int *)out_img_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  
  poll_mmap = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, MEM_POLLING_VARIABLE_ADDR);
  //poll_mmap = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, AXI_POLLING_VARIABLE_ADDR);
  if(((unsigned int *)poll_mmap) == MAP_FAILED) {
    printf("poll_mmap mapping for absolute memory access failed.\n");
    return -1;
  }

  for(i=0; i<polls; i++)  ((volatile unsigned int *) poll_mmap)[i] = 0;

  printf("Frame buffers set up\n");

  
  
  /******************************************************************************/
  /*********************************** LOOP BEGINS ******************************/

  // wait until polling variable goes to 1
  printf("Waiting for polling variable to go to 1...\n");
  while(1){
    while( ((volatile unsigned int *)poll_mmap)[0] == 0);
    printf("Starting Canny_HW_RC\n");
  
    gettimeofday(&start, NULL);
    Canny_HW_RC(out_img_fb, sobel_dx_out_fb, sobel_dy_out_fb, width, height, (int)thresh, (int)(thresh*ratio), nCanny, false );
    gettimeofday(&stop, NULL);
    printf("Canny Edge Detector wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
    
    printf("Setting polling variable to 0..\n");
    for(i=0; i<polls; i++) ((volatile unsigned int *)poll_mmap)[i] = 0;  
  }

  printf("Exiting\n");
  // Halt VDMA and unmap memory ranges
  //munmap(sobel_dx_out_fb, BUFFER_SIZE);
  //munmap(sobel_dy_out_fb, BUFFER_SIZE);
  //munmap(out_img_fb, BUFFER_SIZE);
  //munmap(poll_mmap, BUFFER_SIZE);
  //close(devmem);
  //src_gray_tmp.release();

  printf("Bye!\n");

  return 0;
}

