/*
 * opencv_funcs.h
 *
 *  Created on: 21 apr 2016
 *      Author: michele
 */

#ifndef OPENCV_GBLUR_H_
#define OPENCV_GBLUR_H_

//#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core.hpp"

#include "gaussian_coefficients.h"
#include "convolution.h"


using namespace cv;

namespace my_Space {

/*Blurs an image using a Gaussian filter.
 *	Parameters:
 *	src – input image; the image can have any number of channels, which are processed independently, but the depth should be CV_8U, CV_16U, CV_16S, CV_32F or CV_64F.
 *	dst – output image of the same size and type as src.
 *	ksize – Gaussian kernel size. ksize.width and ksize.height can differ but they both must be positive and odd.
 *			Or, they can be zero’s and then they are computed from sigma* .
 *	sigma1 – Gaussian kernel standard deviation in X direction.
 *	sigma2 – Gaussian kernel standard deviation in Y direction;
 *			 if sigmaY is zero, it is set to be equal to sigmaX,
 *			 if both sigmas are zeros, they are computed from ksize.width and ksize.height , respectively (see getGaussianKernel() for details);
 *			 to fully control the result regardless of possible future modifications of all this semantics, it is recommended to specify all of ksize, sigmaX, and sigmaY.
 *	borderType – pixel extrapolation method (see borderInterpolate() for details).
 *	The function convolves the source image with the specified Gaussian kernel. In-place filtering is supported.
 *
 *	BORDER_CONSTANT    = 0, //!< `iiiiii|abcdefgh|iiiiiii`  with some specified `i`
 *	BORDER_REPLICATE   = 1, //!< `aaaaaa|abcdefgh|hhhhhhh`
 *	BORDER_REFLECT     = 2, //!< `fedcba|abcdefgh|hgfedcb`
 *	BORDER_WRAP        = 3, //!< `cdefgh|abcdefgh|abcdefg`
 *	BORDER_REFLECT_101 = 4, //!< `gfedcb|abcdefgh|gfedcba`
 *	BORDER_TRANSPARENT = 5, //!< `uvwxyz|absdefgh|ijklmno`
 *
 *	BORDER_REFLECT101  = BORDER_REFLECT_101, //!< same as BORDER_REFLECT_101
 *	BORDER_DEFAULT     = BORDER_REFLECT_101, //!< same as BORDER_REFLECT_101
 *	BORDER_ISOLATED    = 16 //!< do not look outside of ROI
 *
 */
	void GaussianBlur( InputArray src, OutputArray dst, 
					   Size ksize, double sigma1, double sigma2 = 0, 
					   int borderType = BORDER_DEFAULT, int custom = 0);

	void createGaussianKernels( Mat & kx, Mat & ky,
								int type, Size ksize,
								double sigma1, double sigma2 );

/*Returns Gaussian filter coefficients.
 *  Parameters:
 *	ksize – Aperture size. It should be odd ( ksize mod 2 = 1 ) and positive.
 *	sigma – Gaussian standard deviation. If it is non-positive, it is computed from ksize as sigma = 0.3*((ksize-1)*0.5 - 1) + 0.8 .
 *	ktype – Type of filter coefficients. It can be CV_32f or CV_64F .
 *
 *	The function computes and returns the (ksize x 1) matrix of Gaussian filter coefficients:
 *
 *	G_i= alpha * e^{-(i-(ksize -1)/2)^2/(2*sigma^2)},
 *
 *	where i=0..ksize-1 and alpha is the scale factor chosen so that the sum of the coefficients is equal to one.
 *
 *	Two of such generated kernels can be passed to sepFilter2D() or to createSeparableLinearFilter().
 *	Those functions automatically recognize smoothing kernels (a symmetrical kernel with sum of weights equal to 1) and handle them accordingly.
 *	You may also use the higher-level GaussianBlur().
 */
	Mat getGaussianKernel( int ksize, double sigma, int ktype = CV_64F );

	void createCustomGaussianMask(int ksize, double sigma, Mat & kernel, long long * normalization, int type, int custom);
}

#endif /* OPENCV_GBLUR_H_ */
