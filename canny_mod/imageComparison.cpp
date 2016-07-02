/*	
 * Image comparison between two input images
 *	
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "global.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core.hpp"


using namespace cv;
using namespace std;



/*
int main(int argc, char ** argv){
	int nonZero=0, sum=0;
	double max=0.0;
	char in1[256] = "img1.bmp";
	char in2[256] = "img2.bmp";
	char diff_file[256] = "difference.bmp";

	Mat a;
	Mat b;
	Mat diff;

	if(argc < 3){
		printf("Input parameters must be 2: ./imageComparison <img1> <img2> <diff_file*>\n");
		return -1;
	}

	strcpy(in1, argv[1]);
	strcpy(in2, argv[2]);

	if(argc >= 4)
		strcpy(diff_file, argv[3]);

	// Images will be converted in grayscale domain
	a = imread(in1, 0);
	b = imread(in2, 0);
	if(a.size() != b.size()){
		printf("Images must have the same size.\n");
		return -1;
	}

	diff.create(a.size(), a.type());
	absdiff(a, b, diff);
	imwrite(diff_file, diff);
	minMaxLoc(diff, NULL, &max, NULL, NULL);
	nonZero = countNonZero(diff);
	Scalar s = cv::sum(diff);


	printf("Comparison between images %s and %s:\n", in1, in2);
	printf("  Image size:         %dx%d (%d pixels)\n", a.rows, a.cols, a.rows*a.cols);
	printf("  Maximum difference: %d\n", (int)max);
	printf("  Different pixels:   %d (%.2f%%)\n", nonZero, ((float)nonZero)/(a.rows*a.cols));
	printf("  Sum of all errors:  %ld\n", (long)s[0]);
	printf("\n");

	printf("Metrics evaluation:\n");
	printf("  PSNR:  %lf\n", getPSNR(a, b));
	printf("  MSSIM: %lf\n", getMSSIM(a, b)[0]);
	printf("\n");

}
*/



/**********************************************************************************************************/

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
