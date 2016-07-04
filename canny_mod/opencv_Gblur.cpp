#include "opencv_Gblur.h"

namespace my_Space
{
	void GaussianBlur( InputArray _src, OutputArray _dst, Size ksize, double sigma1, double sigma2, int borderType, int custom)
	{
		int i=0, j=0;
		//printf("GaussianBlur modified used!\n");

		//Create a destination image with the same size and type of the input one
		int type = _src.type();
		Size size = _src.size();
		_dst.create( size, type );

		//Check borderType parameter
		if( borderType != BORDER_CONSTANT && (borderType & BORDER_ISOLATED) != 0 )
		{
			if( size.height == 1 )
				ksize.height = 1;
			if( size.width == 1 )
				ksize.width = 1;
		}

		//If kernel_size is 1x1 then copy the source image to the destination one and returns
		if( ksize.width == 1 && ksize.height == 1 )
		{
			_src.copyTo(_dst);
			return;
		}

		Mat in_img = _src.getMat();
		
		int cols = in_img.cols;
		int rows = in_img.rows;
		
		int norm;
		Mat kx, ky;
		
		// Filter the image using separable property of the Gaussian function, 1D convolutions applied on the rows
		// and columns separately using kernel kx and ky.
		
		if(custom == 0){		// OpenCV standard Gaussian Separable 2D filter (floating coefficients)
			
			my_Space::createGaussianKernels(kx, ky, type, ksize, sigma1, sigma2);
			cv::sepFilter2D(_src, _dst, CV_MAT_DEPTH(type), kx, ky, Point(-1,-1), 0, borderType );
			//imwrite("src_blurred_0.bmp", _dst);

		}else if(custom == 1){	// OpenCV Gaussian Separable 2D filter with (integer-approximated coefficients)
			int h, i;
			int * tmp_kernel;
			float * kernel = (float*)malloc(ksize.width * sizeof(float));
			get_custom_coeff_vector(ksize.width, sigma1, &tmp_kernel, &norm);
			printf("norm=%d\n", norm);
			for(i=0; i<ksize.width; i++){
				kernel[i] = ((float)tmp_kernel[i]) / norm;
			}

			my_Space::createGaussianKernels(kx, ky, type, ksize, sigma1, sigma2);
			for(i=0; i<ksize.width; i++){
				printf("tmp_kernel[%d]=%d, kernel[%d]=%f, kx[%d]=%f, error= %2.2f\n", i, tmp_kernel[i], i, kernel[i], i, kx.at<float>(i), abs((1.0-kernel[i]/kx.at<float>(i))*100));
			}

			Mat K = Mat(1, ksize.width, CV_32FC1, kernel);
			cv::sepFilter2D(_src, _dst, -1 , K, K, Point( -1, -1 ), 0, BORDER_DEFAULT );

			//imwrite("src_blurred_1.bmp", _dst);
			free(kernel);
		}
		
	}


	void createGaussianKernels( Mat & kx, Mat & ky, int type, Size ksize, double sigma1, double sigma2 )
	{
		//Save the depth for each component of the image and set sigma2=sigma1 if sigma2 is lower than zero
		int depth = CV_MAT_DEPTH(type);
		if( sigma2 <= 0 )
			sigma2 = sigma1;

		// automatic detection of kernel size from sigma if kenel_size is set to a number lower or equal than zero
		if( ksize.width <= 0 && sigma1 > 0 )
			ksize.width = cvRound(2*sigma1*(depth == CV_8U ? 3 : 4) + 1)|1; //bitwise or with 1 to obtain an odd number
		if( ksize.height <= 0 && sigma2 > 0 )
			ksize.height = cvRound(2*sigma2*(depth == CV_8U ? 3 : 4) + 1)|1;

		//Check if the size of the kernel is greater than zero and an odd number
		CV_Assert( ksize.width > 0  && ksize.width % 2 == 1 &&
				   ksize.height > 0 && ksize.height % 2 == 1 );

		sigma1 = std::max( sigma1, 0. );
		sigma2 = std::max( sigma2, 0. );

		//Get 1D Gaussian kernel coefficients (CV_8U=0 CV_32F=5 CV_64F=6)
		kx = getGaussianKernel( ksize.width, sigma1, std::max(depth, CV_32F) );

		//If kernel height and weight are equal and the difference between sigma1 and sigma2 is lower than a certain epsilon,
		//then row kernel is equal to column kernel, otherwise computer the kernel for the y direction.
		if( ksize.height == ksize.width && std::abs(sigma1 - sigma2) < DBL_EPSILON )
			ky = kx;
		else
			ky = getGaussianKernel( ksize.height, sigma2, std::max(depth, CV_32F) );
	}

	Mat getGaussianKernel( int n, double sigma, int ktype )
	{
    const int SMALL_GAUSSIAN_SIZE = 7;

    //Che cazzo di numeri sono questi??????
    static const float small_gaussian_tab[][SMALL_GAUSSIAN_SIZE] =
    {
        {1.f},
        {0.25f, 0.5f, 0.25f},
        {0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f},
        {0.03125f, 0.109375f, 0.21875f, 0.28125f, 0.21875f, 0.109375f, 0.03125f}
    };

    //If kernel size is an odd number, <= 7 and sigma is <= 0, then take the correspondent coefficients in small_gaussian_tab, otherwise zero.
    const float* fixed_kernel = ((n % 2 == 1) && (n <= SMALL_GAUSSIAN_SIZE) && (sigma <= 0)) ?
    							small_gaussian_tab[n>>1] : 0;

    //Check that each channel is CV_32F or CV_64F
    CV_Assert( ktype == CV_32F || ktype == CV_64F );

    //Create a matrix for the kernel of size nx1 of CV_32F (CV_64F) type
    Mat kernel(n, 1, ktype);

    //Returns a pointer to the specified matrix row.
    //	template<typename _Tp> _Tp* Mat::ptr(int i0=0)
    //The methods return uchar* or typed pointer to the specified matrix row. See the sample in Mat::isContinuous() to know how to use these methods.
    float* cf = kernel.ptr<float>();
    double* cd = kernel.ptr<double>();

    //If sigma is greater than zero take this value, otherwise compute sigma from ksize n as ((n-1)*0.5 - 1)*0.3 + 0.8
    double sigmaX = sigma > 0 ? sigma : ((n-1)*0.5 - 1)*0.3 + 0.8;

    //multiplicand -1/(2*sigma^2)
    double scale2X = -0.5/(sigmaX*sigmaX);
    double sum = 0;

    //extract kernel coefficients sampling the gaussian expression:
    //	exp( -1/(2*sigma^2) * (i - (n-1)/2)^2 ) = exp( scale2X * (i - (n-1/2)^2) ) = exp ( scale2X * x^2 )
    int i;
    for( i = 0; i < n; i++ )
    {
        double x = i - (n-1)*0.5;

        //if fizxed_kernel pointer is 0 compute the coefficient using exp function,
        //otherwise take the predefined coefficient inside the array pointed by fixed_kernel
        double t = fixed_kernel ? (double)fixed_kernel[i] : std::exp(scale2X*x*x);

        //if type is 32 bit float use the float pointer cf for the kernel matrix,
        //otherwise (64 bit) take the double pointer cd
        //Save the coefficient in the kernel matrix and update the sum of the coefficients
        if( ktype == CV_32F )
        {
            cf[i] = (float)t;
            sum += cf[i];
        }
        else
        {
            cd[i] = t;
            sum += cd[i];
        }
    }

    //compute the normalized factor N=1/sum and multiply all coefficients by N
    //Notes that here all the coefficients are float or double so they are not approximated to integer values
    
    sum = 1./sum;
    for( i = 0; i < n; i++ )
    {
        if( ktype == CV_32F )
            cf[i] = (float)(cf[i]*sum);
        else
            cd[i] *= sum;
    }

    return kernel;
	}

}// end namespace my_Space 
