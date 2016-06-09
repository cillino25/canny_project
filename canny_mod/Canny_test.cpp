/*
 * Canny_test.cpp
 *
 *  Created on: 21 apr 2016
 *      Author: michele
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
//#include <sys/times.h>
//#include <time.h>
#include <unistd.h> 
#include <iostream>

#include "opencv_funcs.h"
#include "global.h"



using namespace my_Space;
using namespace cv;
using namespace std;

/// Global variables

//Mat src;
//Mat dst;

//Mat src_gray;
//Mat detected_edges;

int edgeThresh = 1;
int const max_lowThreshold = 100;
int ratio = 3;
char* window_name = (char*)"Edge Map";
struct timeval start, stop;


Scalar getMSSIM( const Mat& i1, const Mat& i2);
double getPSNR(const Mat& I1, const Mat& I2);

double sigma(Mat & m, int i, int j, int block_size);
double cov(Mat & m1, Mat & m2, int i, int j, int block_size);
double eqm(Mat & img1, Mat & img2);
double psnr(Mat & img_src, Mat & img_compressed);
double ssim(Mat & img_src, Mat & img_compressed, int block_size, bool show_progress);



/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(const Mat src, Mat *dst, int Threshold, double sigma, int gBlurMaskSize, int cannyMaskSize, int custom); 


/** @function main */
int main( int argc, char** argv )
{

  char res[256] = "result.bmp";
  struct timeval start, stop;
  int threshold = 50;
  double sigma = 1.5;
  int gblur=5, canny=3;
  int custom = 0;
  int i=0, j=0;

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

  
  if( argc < 3 ){
    printf("Not enough arguments.\nList of arguments: ./canny_detector <*source_img> <*threshold> <sigma> <gBlurMaskSize> <cannyMaskSize>\n");
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

  //printf("Result image will be %s\n", res);


  //printf("Print image values:\n");
  //Vec3b intensity;
  //for(i=0; i<src.rows; i++){
  //  for(j=0; j<src.cols; j++){
  //    intensity = src.at<Vec3b>(j, i);
  //    //printf("src.at<uchar>(%d,%d) (BGR)=(%d,%d,%d)\n", j, i, intensity.val[0], intensity.val[1], intensity.val[2]);
  //  }
  //}

  //printf("getTickFrequency() = %lf\n", getTickFrequency());
  //printf("CLOCKS_PER_SEC = %ld\n", CLOCKS_PER_SEC);
  //printf("CLK_TCK = %ld\n", sysconf(_SC_CLK_TCK));
  
  //lena_ref=imread("lena_ref/lena_1.5_40.bmp", 0);
  /// Load an image
  gettimeofday(&start, NULL);
  src = imread( argv[1], 1);
  gettimeofday(&stop, NULL);
  printf("Image read wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
  
  if( !src.data ){
    printf("No source data.\n"); 
    return -1;
  }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  gettimeofday(&start, NULL);
  cvtColor( src, src_gray, CV_BGR2GRAY );
  gettimeofday(&stop, NULL);
  printf("cvtColor wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
  
  
  imwrite("src_gray.bmp", src_gray);

  //Size s1 = src_gray.size();
  //printf("src_gray.height=%d, width=%d\n", s1.height, s1.width);
  
  //printf("CannyThreshold launched with\n  th=%d\n  sigma=%lf\n  gBlurMaskSize=%d\n  cannyMaskSize=%d\n\n", threshold, sigma, gblur, canny);
  //gettimeofday(&start, NULL);
  CannyThreshold(src_gray, &detected_edges, threshold, sigma, gblur, canny, custom);
  //gettimeofday(&stop, NULL);
  //printf("CannyThreshold total wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);



  //printf("Img write: %s\n", res);
  gettimeofday(&start, NULL);
  imwrite(res, detected_edges);
  gettimeofday(&stop, NULL);
  printf("Image write wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  


  /**************************************************************/
  // Image Quality Assessment

  // PSNR evaluation
  #ifdef METRICS
  img_blurred=imread("src_blurred.bmp", 0);
  printf("PSNR evaluation\n");
  //printf("getPSNR(src_gray, detected_edges)  = %lf\n", getPSNR(src_gray, detected_edges));
  //printf("getPSNR(src_gray, lena_ref/lena_1.5_40.bmp)  = %lf\n", getPSNR(src_gray, lena_ref));
  //printf("getMSSIM(src_gray, detected_edges) = %lf\n", getMSSIM(src_gray, detected_edges).val[0]); // NB: the two images must have THE SAME NUMBER OF CHANNELS!
  //printf("getMSSIM(src_gray, lena_ref/lena_1.5_40.bmp) = %lf\n", getMSSIM(src_gray, lena_ref).val[0]);
  //printf("\n");
  printf("getPSNR(src_gray, src_blurred)  = %lf\n", getPSNR(src_gray, img_blurred));
  printf("getMSSIM(src_gray, src_blurred) = %lf\n", getMSSIM(src_gray, img_blurred).val[0]); // NB: the two images must have THE SAME NUMBER OF CHANNELS!
  printf("\n");
  

  if(custom==2){
    Mat bl0 = imread("src_blurred_0.bmp", 0);
    Mat bl2 = imread("src_blurred_2.bmp", 0);
    printf("getPSNR(src_blurred_0, src_blurred_2)  = %lf\n", getPSNR(bl0, bl2));
    printf("getMSSIM(src_blurred_0, src_blurred_2) = %lf\n", getMSSIM(bl0, bl2).val[0]); // NB: the two images must have THE SAME NUMBER OF CHANNELS!
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
  clock_t t1, t2;
  //printf("**Gaussian blur start..\n");
  gettimeofday(&start, NULL);
  //t1=clock();
  my_Space::GaussianBlur(src, *dst, Size(gBlurMaskSize,gBlurMaskSize), sigma, sigma, BORDER_DEFAULT, custom);
  //t2=clock();
  gettimeofday(&stop, NULL);
  printf("Gaussian Blur wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  //printf("clock: %lf\n", (double)(t2 - t1)/CLOCKS_PER_SEC );

  //printf("Canny start..\n");
  gettimeofday(&start, NULL);
  my_Space::Canny( *dst, *dst, Threshold, Threshold*ratio, cannyMaskSize );
  gettimeofday(&stop, NULL);
  printf("Canny Edge Detector total wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
  
 }


double sigma(Mat & m, int i, int j, int block_size)
{
  double sd = 0;

  Mat m_tmp = m(Range(i, i + block_size), Range(j, j + block_size));
  Mat m_squared(block_size, block_size, CV_64F);

  multiply(m_tmp, m_tmp, m_squared);

  // E(x)
  double avg = mean(m_tmp)[0];
  // E(x²)
  double avg_2 = mean(m_squared)[0];

  sd = sqrt(avg_2 - avg * avg);

  return sd;
}

  // Covariance
double cov(Mat & m1, Mat & m2, int i, int j, int block_size)
{
  Mat m3 = Mat::zeros(block_size, block_size, m1.depth());
  Mat m1_tmp = m1(Range(i, i + block_size), Range(j, j + block_size));
  Mat m2_tmp = m2(Range(i, i + block_size), Range(j, j + block_size));


  multiply(m1_tmp, m2_tmp, m3);

  double avg_ro   = mean(m3)[0]; // E(XY)
  double avg_r  = mean(m1_tmp)[0]; // E(X)
  double avg_o  = mean(m2_tmp)[0]; // E(Y)


  double sd_ro = avg_ro - avg_o * avg_r; // E(XY) - E(X)E(Y)

  return sd_ro;
}

  // Mean squared error
double eqm(Mat & img1, Mat & img2)
{
  int i, j;
  double eqm = 0;
  int height = img1.rows;
  int width = img1.cols;

  //printf("-d: h=%d, w=%d\n", height, width);

  for (i = 0; i < height; i++){
    for (j = 0; j < width; j++){
      //printf("img1.at<double>(%d,%d)=%lf\n", i, j, img1.at<double>(i, j));
      //eqm += (img1.at<double>(i, j) - img2.at<double>(i, j)) * (img1.at<double>(i, j) - img2.at<double>(i, j));
      eqm += (img1.at<uchar>(i, j) - img2.at<uchar>(i, j)) * (img1.at<uchar>(i, j) - img2.at<uchar>(i, j));
    }
  }

  eqm /= height * width;

  return eqm;
}



  /**
   *  Compute the PSNR between 2 images
   */
double psnr(Mat & img_src, Mat & img_compressed)
{
  int D = 255;
  double e = eqm(img_src, img_compressed);
  //printf("--d: eqm=%lf\n", e);
  return (10 * log10((D*D)/e));
}


  /**
   * Compute the SSIM between 2 images
   */
double ssim(Mat & img_src, Mat & img_compressed, int block_size, bool show_progress)
{
  double ssim = 0;

  int nbBlockPerHeight  = img_src.rows / block_size;
  printf("nbBlockPerHeight=%d\n", nbBlockPerHeight);
  int nbBlockPerWidth   = img_src.cols / block_size;
  printf("nbBlockPerWidth=%d\n", nbBlockPerWidth);

  for (int k = 0; k < nbBlockPerHeight; k++)
  {
    for (int l = 0; l < nbBlockPerWidth; l++)
    {
      int m = k * block_size;
      int n = l * block_size;

      Scalar tmp = mean(img_src(Range(k, k + block_size), Range(l, l + block_size)))[0];
      //printf("# tmp = %f\n", tmp[0]);

      double avg_o  = mean(img_src(Range(k, k + block_size), Range(l, l + block_size)))[0];
      printf("-d:[%d][%d] avg_o=%lf\n", k, l, avg_o);
      double avg_r  = mean(img_compressed(Range(k, k + block_size), Range(l, l + block_size)))[0];
      printf("-d:[%d][%d] avg_r=%lf\n", k, l, avg_r);
      double sigma_o  = sigma(img_src, m, n, block_size);
      printf("-d:[%d][%d] sigma_o=%lf\n", k, l, sigma_o);
      double sigma_r  = sigma(img_compressed, m, n, block_size);
      printf("-d:[%d][%d] sigma_r=%lf\n", k, l, sigma_r);
      double sigma_ro = cov(img_src, img_compressed, m, n, block_size);
      printf("-d:[%d][%d] sigma_ro=%lf\n", k, l, sigma_ro);
      ssim += ((2 * avg_o * avg_r + C1) * (2 * sigma_ro + C2)) / ((avg_o * avg_o + avg_r * avg_r + C1) * (sigma_o * sigma_o + sigma_r * sigma_r + C2));
      
    }
    // Progress
    if (show_progress)
      cout << "\r>>SSIM [" << (int) ((( (double)k) / nbBlockPerHeight) * 100) << "%]";
  }
  ssim /= nbBlockPerHeight * nbBlockPerWidth;

  if (show_progress)
  {
    cout << "\r>>SSIM [100%]" << endl;
    cout << "SSIM : " << ssim << endl;
  }

  return ssim;
}

double getPSNR(const Mat& I1, const Mat& I2)
{
 Mat s1;
 absdiff(I1, I2, s1);       // |I1 - I2|
 s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
 s1 = s1.mul(s1);           // |I1 - I2|^2

 Scalar s = sum(s1);         // sum elements per channel

 double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

 if( sse <= 1e-10) // for small values return zero
     return 0;
 else
 {
     double  mse =sse /(double)(I1.channels() * I1.total());
     double psnr = 10.0*log10((255*255)/mse);
     return psnr;
 }
}

Scalar getMSSIM( const Mat& i1, const Mat& i2){
  //const double C1 = 6.5025, C2 = 58.5225;
  /***************************** INITS **********************************/
  int d     = CV_32F;

  Mat I1, I2;
  i1.convertTo(I1, d);           // cannot calculate on one byte large values
  i2.convertTo(I2, d);


  Mat I2_2   = I2.mul(I2);        // I2^2
  Mat I1_2   = I1.mul(I1);        // I1^2
  Mat I1_I2  = I1.mul(I2);        // I1 * I2

  /***********************PRELIMINARY COMPUTING ******************************/
  Mat mu1, mu2;   //
  cv::GaussianBlur(I1, mu1, Size(11, 11), 1.5);
  cv::GaussianBlur(I2, mu2, Size(11, 11), 1.5);


  Mat mu1_2   =   mu1.mul(mu1);
  Mat mu2_2   =   mu2.mul(mu2);
  Mat mu1_mu2 =   mu1.mul(mu2);

  Mat sigma1_2, sigma2_2, sigma12;

  cv::GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
  sigma1_2 -= mu1_2;

  cv::GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
  sigma2_2 -= mu2_2;

  cv::GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
  sigma12 -= mu1_mu2;

  ///////////////////////////////// FORMULA ////////////////////////////////
  Mat t1, t2, t3;

  t1 = 2 * mu1_mu2 + C1;
  t2 = 2 * sigma12 + C2;
  t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

  t1 = mu1_2 + mu2_2 + C1;
  t2 = sigma1_2 + sigma2_2 + C2;
  t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

  Mat ssim_map;
  divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

  Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
  
  return mssim;
}
