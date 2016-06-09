/*
 * opencv_Canny.h
 *
 *  Created on: 22 apr 2016
 *      Author: michele
 */

#ifndef OPENCV_CANNY_H_
#define OPENCV_CANNY_H_

//#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core.hpp"

#include "convolution_arm.h"


using namespace cv;

namespace my_Space {

/* Finds edges in an image using the [Canny86] algorithm.
 *	 Parameters:
 *		image – single-channel 8-bit input image.
 *		edges – output edge map; it has the same size and type as image .
 *		threshold1 – first threshold for the hysteresis procedure.
 *		threshold2 – second threshold for the hysteresis procedure.
 *		apertureSize – aperture size for the Sobel() operator.
 *		L2gradient – a flag, indicating whether a more accurate  L_2 norm  =\sqrt{(dI/dx)^2 + (dI/dy)^2} should be used to calculate the image gradient magnitude ( L2gradient=true ),
 *					 or whether the default  L_1 norm  =|dI/dx|+|dI/dy| is enough ( L2gradient=false ).
 *The function finds edges in the input image image and marks them in the output map edges using the Canny algorithm. The smallest value between threshold1 and threshold2 is used for edge linking. The largest value is used to find initial segments of strong edges. See http://en.wikipedia.org/wiki/Canny_edge_detector
 */
	void Canny( InputArray image, OutputArray edges,
                double threshold1, double threshold2,
                int apertureSize = 3, bool L2gradient = false, int custom = 0);

	void nonMaxSuppress(Mat src, int cn, Mat dx, Mat dy, Mat gradient, ptrdiff_t mapstep, int* mag_buf[], uchar* map, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom,
                double threshold1, double threshold2,
                bool L2gradient = false );

	void hysteresisThresh(ptrdiff_t mapstep, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom);

	void Sobel( InputArray src, OutputArray dst, int ddepth,
				int dx, int dy, int ksize = 3,
				double scale = 1, double delta = 0,
				int borderType = BORDER_DEFAULT, int custom = 0);
	void getDerivKernels( OutputArray kx, OutputArray ky,
						  int dx, int dy, int ksize,
						  bool normalize = false, int ktype = CV_32F );
	void getScharrKernels( OutputArray _kx, OutputArray _ky,
		                   int dx, int dy, bool normalize, int ktype );
	void getSobelKernels( OutputArray _kx, OutputArray _ky,
						  int dx, int dy, int _ksize, bool normalize, int ktype );
}

#endif /* OPENCV_CANNY_H_ */
