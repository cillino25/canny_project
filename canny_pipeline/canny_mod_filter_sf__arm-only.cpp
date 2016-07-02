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

#include "gaussian_coefficients_arm.h"

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
  char in_img[256] = "lena_secret.bmp";
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

  unsigned int page_size = 4096;//sysconf(_SC_PAGESIZE);


  //char in_file[]="lena_gray.bmp";
  char out_file[]="lena_blurred_2.bmp";
  int size=0, width=0, height=0;
  unsigned short *img_data;
  unsigned short *img_info;
  unsigned char * src_data;

  /*Video types*/
//  Mat frame, frame_out; 
  const char win_name_in[]="Live Video...";
  const char win_name_out[]="Real Time Canny...";
  const char *Mat_types[] = {"CV_8U", "CV_8S", "CV_16U", "CV_16S", "CV_32S", "CV_32F", "CV_64F"};
  int set_height = 0, set_width = 0, video_width, video_height;
  int cam_id = 0;       // Webcam connected to USB port
  double fps;


  //if( argc < 3 ){
  //  printf("Not enough arguments.\nList of arguments: ./canny_mod_filter_sf <*source_img> <*threshold> <sigma> <gBlurMaskSize> <cannyMaskSize> <alg_type> <output_file>\n");
  //  return -1;
  //}
  if(argc > 1)
    strcpy(in_img, argv[1]);

  if(argc > 2)
    thresh = atoi(argv[2]);

  if(argc > 3)
    sigma = (double)atof(argv[3]);

  if(argc > 4)
    nGblur = atoi(argv[4]);

  if(argc > 5)
    nCanny = atoi(argv[5]);

  if(argc > 6)
    custom = atoi(argv[6]);

  if((nGblur!=3)&&(nGblur!=5)&&(nGblur!=7)){ printf("nGblur mask size must be in {3,5,7}.\n"); return -1; }
  if((nGblur==3)&&((sigma<0.4)||(sigma>1))){ printf("With GBlur_size=3 sigma must be 0.4 < sigma < 1.\n"); return -1; }
  if((nGblur==5)&&((sigma<0.8)||(sigma>1.3))){ printf("With GBlur_size=5 sigma must be 0.8 < sigma < 1.3.\n"); return -1; }
  if((nGblur==7)&&((sigma<1)||(sigma>1.5))){ printf("With GBlur_size=7 sigma must be 1 < sigma < 1.5.\n"); return -1; }
  if((nCanny!=3)&&(nCanny!=5)&&(nCanny!=7)){ printf("nCanny mask size must be in {3,5,7}.\n"); return -1; }

  printf("Input image will be %s\n", in_img);

  if((devmem = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
    printf("Can't open /dev/mem.\nExiting...\n");
    return 1;
  }
  printf("/dev/mem opened\n");
  
  // GaussianBlur kernel coefficient computation
  int *k0_hz_coeffs = (int*) malloc(KERNEL_COEFFS * sizeof(int));
  int *k0_vt_coeffs = (int*) malloc(KERNEL_COEFFS * sizeof(int));
  int norm0=0;
  
  if(nGblur != KERNEL_COEFFS){
    int * tmp_kernel = (int*) malloc(KERNEL_COEFFS*sizeof(int));
    get_custom_coeff_vector(nGblur, sigma, &tmp_kernel, &norm0);
    norm0 *= norm0;
    for(i=0; i<nGblur; i++){
      k0_hz_coeffs[i+(KERNEL_COEFFS-nGblur)/2] = tmp_kernel[i];
      k0_vt_coeffs[i+(KERNEL_COEFFS-nGblur)/2] = tmp_kernel[i];
    }
    free(tmp_kernel);
  }else{
    get_custom_coeff_vector(nGblur, sigma, &k0_hz_coeffs, &norm0);
    get_custom_coeff_vector(nGblur, sigma, &k0_vt_coeffs, &norm0);
    norm0 *= norm0;
  }


  // SobelDx and SobelDy kernel coefficients computation
  int k1_hz_coeffs[KERNEL_COEFFS]; for(i=0; i<KERNEL_COEFFS; i++) k1_hz_coeffs[i]=0;
  int k1_vt_coeffs[KERNEL_COEFFS]; for(i=0; i<KERNEL_COEFFS; i++) k1_vt_coeffs[i]=0;
  int norm1=0;
  int k2_hz_coeffs[KERNEL_COEFFS]; for(i=0; i<KERNEL_COEFFS; i++) k2_hz_coeffs[i]=0;
  int k2_vt_coeffs[KERNEL_COEFFS]; for(i=0; i<KERNEL_COEFFS; i++) k2_vt_coeffs[i]=0;
  int norm2=0;

  
  Mat Sx_hz, Sx_vt, Sy_hz, Sy_vt;
  getDerivKernels(Sx_hz, Sx_vt, 1, 0, nCanny, false, CV_32F);
  getDerivKernels(Sy_hz, Sy_vt, 0, 1, nCanny, false, CV_32F);

  //printf("Sx_hz: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", (int)Sx_hz.at<float>(i));
  //printf("\nSx_vt: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", (int)Sx_vt.at<float>(i));
  //printf("\nSy_hz: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", (int)Sy_hz.at<float>(i));
  //printf("\nSy_vt: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", (int)Sy_vt.at<float>(i));
  //printf("\n\n");

  printf("nC: %d\n", nCanny);
  for(i=0; i<nCanny; i++){
    printf("index=%d\n", i+(KERNEL_COEFFS-nCanny/2));
    k1_hz_coeffs[i+(KERNEL_COEFFS-nCanny)/2] = (int)Sx_hz.at<float>(i);
    k1_vt_coeffs[i+(KERNEL_COEFFS-nCanny)/2] = (int)Sx_vt.at<float>(i);
    k2_hz_coeffs[i+(KERNEL_COEFFS-nCanny)/2] = (int)Sy_hz.at<float>(i);
    k2_vt_coeffs[i+(KERNEL_COEFFS-nCanny)/2] = (int)Sy_vt.at<float>(i);
  }
  

  /*
  Mat Sx_hz, Sx_vt, Sy_hz, Sy_vt;
  getDerivKernels(Sx_hz, Sx_vt, 1, 0, nCanny, false, CV_32F);
  getDerivKernels(Sy_hz, Sy_vt, 0, 1, nCanny, false, CV_32F);

  float *Sx_p_hz, *Sx_p_vt, *Sy_p_hz, *Sy_p_vt;
  Sx_p_hz = Sx_hz.ptr<float>(0);
  Sx_p_vt = Sx_vt.ptr<float>(0);
  Sy_p_hz = Sy_hz.ptr<float>(0);
  Sy_p_vt = Sy_vt.ptr<float>(0);

  int zeros = KERNEL_COEFFS - nCanny;
  int j=0;
  for(int i=0; i<KERNEL_COEFFS; i++)
  {
    if( (i < zeros/2)  ||  (i >= KERNEL_COEFFS - zeros/2))
    {
      k1_hz_coeffs[i] = 0;
      k1_vt_coeffs[i] = 0;
      k2_hz_coeffs[i] = 0;
      k2_vt_coeffs[i] = 0;
    }
    else
    {
      k1_hz_coeffs[i] = (int)Sx_p_hz[j];
      k1_vt_coeffs[i] = (int)Sx_p_vt[j];
      k2_hz_coeffs[i] = (int)Sy_p_hz[j];
      k2_vt_coeffs[i] = (int)Sy_p_vt[j];
      j++;    
    }
  }
  */

  //printf("k0_hz_coeffs: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", k0_hz_coeffs[i]); printf("\n");
  //printf("k0_vt_coeffs: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", k0_vt_coeffs[i]); printf("\n");
  //printf("norm0 = %d\n", norm0);
  //printf("k1_hz_coeffs: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", k1_hz_coeffs[i]); printf("\n");
  //printf("k1_vt_coeffs: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", k1_vt_coeffs[i]); printf("\n");
  //printf("norm1 = %d\n", norm1);
  //printf("k2_hz_coeffs: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", k2_hz_coeffs[i]); printf("\n");
  //printf("k2_vt_coeffs: "); for(i=0; i<KERNEL_COEFFS; i++) printf("%d ", k2_vt_coeffs[i]); printf("\n");
  //printf("norm2 = %d\n", norm2);

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

  vdma_handle.vdmaHandler = devmem;
  //vdma_setup(&vdma_handle, page_size, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER);
  vdma_setup(&vdma_handle, page_size, AXI_VDMA_BASEADDR, width, height, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER);
  
  printf("VDMA set up\n");

  sepImageFilter_setupHandle(&filter_handle, &vdma_handle.vdmaHandler, page_size, AXI_SEPIMGFILTER);
  sepImageFilter_setupHandleParams(&filter_handle, width, height, k0_hz_coeffs, k0_vt_coeffs, norm0, k1_hz_coeffs, k1_vt_coeffs, norm1, k2_hz_coeffs, k2_vt_coeffs, norm2);
  sepImageFilter_setupFilter(&filter_handle);

  printf("sepImageFilter set up\n");

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

  printf("Frame buffers set up\n");

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

  printf("Mat created\n");
  
  /******************************************************************************/
  /*********************************** LOOP BEGINS ******************************/

  src_gray_tmp = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

  printf("Image read\n");

  // Final application will take frames from VideoCapture with pixels in CV_16U format, so this conversion will not be needed.
  // convertTo will put new frame into gb_in_fb
  src_gray_tmp.convertTo(src_gray, CV_16U);

  //imwrite("lena_gray_16U.bmp", src_gray);

  printf("Image converted\n");

  gettimeofday(&start, NULL);
  GaussianBlur_HW(&vdma_handle, &filter_handle, width, height, gb_in_fb_addr, gb_out_fb_addr);
  gettimeofday(&stop, NULL);
  printf("\nGaussianBlur_HW wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);

  imwrite("lena_blurred_0.bmp", gb_dest);

  gettimeofday(&start, NULL);
  Canny_HW_ARM_only(&vdma_handle, &filter_handle, dest, dx, sobel_dx_out_fb, dy, sobel_dy_out_fb, width, height, gb_out_fb_addr, sobel_dx_out_fb_addr, sobel_dy_out_fb_addr, (double)thresh, ((double)thresh*ratio), nCanny, false );
  gettimeofday(&stop, NULL);
  printf("Canny Edge Detector wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
  imwrite("result.bmp", dest);
  imwrite("dx.bmp", dx);
  imwrite("dy.bmp", dy);



  // Halt VDMA and unmap memory ranges
  vdma_halt(&vdma_handle);
  free(k0_hz_coeffs);
  free(k0_vt_coeffs);
  munmap(gb_out_fb, BUFFER_SIZE);
  munmap(gb_in_fb, BUFFER_SIZE);
  munmap(sobel_dx_out_fb, BUFFER_SIZE);
  munmap(sobel_dy_out_fb, BUFFER_SIZE);
  munmap(out_img_fb, BUFFER_SIZE);
  close(devmem);
  printf("Bye!\n");

  return 0;
}

