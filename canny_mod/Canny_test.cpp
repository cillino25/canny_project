
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
//#include <sys/times.h>
#include <time.h>
#include <unistd.h> 
#include <iostream>

#include "opencv_funcs.h"
#include "global.h"
#include "imageComparison.h"


using namespace my_Space;
using namespace cv;
using namespace std;

/// Global variables


int ratio = 3;
struct timeval start, stop, start1, stop1;


void CannyThreshold(const Mat src, Mat *dst, int Threshold, double sigma, int gBlurMaskSize, int cannyMaskSize, int custom); 


/** @function main */
int main( int argc, char **argv )
{
  int i=0, j=0;
  //struct timeval start, stop;
  int threshold = 50;
  double sigma = 1.5;
  int gblur=5, canny=3;
  int custom = 0;
  char in_img[256] = "lena_secret.bmp";
  char res[256] = "result.bmp";

  printf("\n## cmd: ");
  for(i=0;i<argc;i++)
  	printf("%s ", argv[i]);
  printf("\n");

  Mat lena_ref;
  Mat src;
  Mat dst;
  Mat src_gray;
  Mat detected_edges;
  Mat img_blurred;

  
  //if( argc < 3 ){
  //  printf("Not enough arguments.\nList of arguments: ./canny_detector <*source_img> <*threshold> <sigma> <gBlurMaskSize> <cannyMaskSize>\n");
  //  return -1;
  //}
  if(argc > 1)
    strcpy(in_img, argv[1]);

  if(argc > 2)
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

  

  //gettimeofday(&start, NULL);
  src = imread( in_img, 1);
  //gettimeofday(&stop, NULL);
  //printf("Image read wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
  
  if( !src.data ){
    printf("No source data.\n"); 
    return -1;
  }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  //gettimeofday(&start, NULL);
  cvtColor( src, src_gray, CV_BGR2GRAY );
  //gettimeofday(&stop, NULL);
  //printf("cvtColor wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
  
  
  //imwrite("src_gray.bmp", src_gray);

  //Size s1 = src_gray.size();
  //printf("src_gray.height=%d, width=%d\n", s1.height, s1.width);
  
  //printf("CannyThreshold launched with\n  th=%d\n  sigma=%lf\n  gBlurMaskSize=%d\n  cannyMaskSize=%d\n\n", threshold, sigma, gblur, canny);
  gettimeofday(&start1, NULL);
  CannyThreshold(src_gray, &detected_edges, threshold, sigma, gblur, canny, custom);
  gettimeofday(&stop1, NULL);
  printf("CannyThreshold total wall time: %lf s\n\n", ((stop1.tv_sec + stop1.tv_usec*0.000001)-(start1.tv_sec + start1.tv_usec*0.000001))*PRESC);



  //printf("Img write: %s\n", res);
  //gettimeofday(&start, NULL);
  imwrite(res, detected_edges);
  //gettimeofday(&stop, NULL);
  //printf("Image write wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  


  /**************************************************************/
  // Image Quality Assessment

  // PSNR evaluation
  #ifdef METRICS
  img_blurred=imread("src_blurred_0.bmp", 0);
  if(!img_blurred.data){ printf("No input image!\n"); return 1; }
  printf("PSNR evaluation\n");
  printf("getPSNR(src_gray, src_blurred)  = %lf\n", getPSNR(src_gray, img_blurred));
  printf("getMSSIM(src_gray, src_blurred) = %lf\n", getMSSIM(src_gray, img_blurred).val[0]); // NB: the two images must have THE SAME NUMBER OF CHANNELS!
  printf("\n");
  

  if(custom==1){
    Mat bl0 = imread("src_blurred_0.bmp", 0);
    Mat bl1 = imread("src_blurred_1.bmp", 0);
    if(!bl0.data){ printf("No input image!\n"); return 1; }
    if(!bl1.data){ printf("No input image!\n"); return 1; }
    printf("getPSNR(src_blurred_0, src_blurred_1)  = %lf\n", getPSNR(bl0, bl1));
    printf("getMSSIM(src_blurred_0, src_blurred_1) = %lf\n", getMSSIM(bl0, bl1).val[0]); // NB: the two images must have THE SAME NUMBER OF CHANNELS!
    printf("\n");
  }


  #endif
  src.release();
  src_gray.release();
  dst.release();
  detected_edges.release();


  return 0;
}



/*************************************************************************************************************/
/*************************************************************************************************************/
/*************************************************************************************************************/

void CannyThreshold(const Mat src, Mat *dst, int Threshold, double sigma, int gBlurMaskSize, int cannyMaskSize, int custom)
{
  //printf("**Gaussian blur start..\n");
  gettimeofday(&start, NULL);
  my_Space::GaussianBlur(src, *dst, Size(gBlurMaskSize,gBlurMaskSize), sigma, sigma, BORDER_DEFAULT, custom);
  gettimeofday(&stop, NULL);
  printf("Gaussian Blur wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  

  //printf("Canny start..\n");
  gettimeofday(&start, NULL);
  my_Space::Canny( *dst, *dst, Threshold, Threshold*ratio, cannyMaskSize, false, custom);
  gettimeofday(&stop, NULL);
  printf("Canny Edge Detector total wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
 }
