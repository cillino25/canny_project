/*
 * opencv_funcs.cpp
 *
 *  Created on: 21 apr 2016
 *      Author: michele
 */

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
		//Mat out_img;
		int cols = in_img.cols;
		int rows = in_img.rows;
		unsigned char * data_in = in_img.data;
		unsigned char * data_out = (unsigned char *) malloc(cols*rows*sizeof(unsigned char));
		// Useless instructions since we read only from data_in	//unsigned char * data_in  = (unsigned char *) malloc(cols*rows*sizeof(unsigned char));
																//memcpy(data_in, in_img.data, cols*rows*sizeof(unsigned char));
		
		int sx = ksize.width;  //printf("-d: sx = ksize.width = %d\n", sx);  (={3,5,7})
		int sy = ksize.height; //printf("-d: sy = ksize.height = %d\n", sy); (={3,5,7})

		long long norm;
		Mat kx, ky;
		Mat custom_kernel(sx, sy, CV_32F);
		



		// Filter the image using separable property of the Gaussian function, 1D convolutions applied on the rows
		// and columns separately using kernel kx and ky.
		
		if(custom == 0){		// OpenCV Gaussian Separable 2D filter
			// Create 1D kernels (float)
			my_Space::createGaussianKernels(kx, ky, type, ksize, sigma1, sigma2);

			// printf("### kx rows = %d, kx cols = %d\n", kx.rows, kx.cols);
			// kx = (5,1) elements => column vector
			// printf("### ky rows = %d, ky cols = %d\n", ky.rows, ky.cols);
			// ky = (5,1) elements => column vector
						
			//printf(" -- Separable filter used\n");
			cv::sepFilter2D(_src, _dst, CV_MAT_DEPTH(type), kx, ky, Point(-1,-1), 0, borderType );
			

			imwrite("src_blurred_0.bmp", _dst);
			// resize image (custom convolution does not handle borders)
			/*Mat tmp;
		  Size src_size=_dst.size();
		  src_size.height=src_size.height-(sx/2);
		  src_size.width=src_size.width-(sx/2);
		  resize(_dst, tmp, src_size);
		  
			imwrite("src_blurred_0.bmp", tmp);
			*/

		}else if(custom == 1){	// Custom Gaussian Separable 2D filter (float <1 kernel coefficients, uchar data)
			int ii;
			my_Space::createGaussianKernels(kx, ky, type, ksize, sigma1, sigma2);
			

			// Emulate custom == 0 for testing purposes
			//cv::sepFilter2D(_src, _dst, CV_MAT_DEPTH(type), kx, ky, Point(-1,-1), 0, borderType );
			//unsigned char * tmp = _dst.getMatRef().data;
			
			float * xKernel = (float*)malloc(sx * sizeof(float));
			float * yKernel = (float*)malloc(sy * sizeof(float));
			
			for(ii=0; ii<sx; ii++) xKernel[ii] = kx.at<float>(ii, 0);
			for(ii=0; ii<sy; ii++) yKernel[ii] = ky.at<float>(ii, 0);
			
			// bool convolve2DSeparable(unsigned char* in, unsigned char* out, int sizeX, int sizeY, float* xKernel, int kSizeX, float* yKernel, int kSizeY);
			convolve2DSeparable(data_in, data_out, cols, rows, xKernel, sx, yKernel, sy);
			
			// printf("\n\n\nTest equality:\n");
			// Test blurred images equality
			//for(i=0; i<rows*cols; i++){
			//	if(abs(tmp[i] - data_out[i]) > 2) printf("Different at %d (%d, %d): sepFilter2D = %d != %d = convolve2DSeparable\n", i, i/rows, i%rows, tmp[i], data_out[i]);
			//}


			Mat out_img = Mat(rows, cols, CV_8UC1, data_out);

			out_img.copyTo(_dst);
			imwrite("src_blurred_1.bmp", out_img);

			/*
			Mat tmp;
		  Size src_size=_dst.size();
		  src_size.height=src_size.height-(sx/2);
		  src_size.width=src_size.width-(sx/2);
		  resize(_dst, tmp, src_size);
		  
			imwrite("src_blurred_1.bmp", tmp);
			*/

			free(xKernel); free(yKernel);

			
		}else if(custom == 2){	// Custom Gaussian Separable 2D filter (integer >1 kernel coefficients, uchar data)
			int h, i;
			long long * tmp_kernel;
			float * kernel = (float*)malloc(sx * sizeof(float));
			get_custom_coeff_vector(sx, sigma1, &tmp_kernel, &norm);

			for(i=0; i<sx; i++){
				kernel[i] = tmp_kernel[i]/((float)norm);
				//printf("-d: kernel[%d] = %f  =  tmp_kernel[%d] (=%lld) / norm (%lld)\n", i, kernel[i], i, tmp_kernel[i], norm);
			}
			//convolve2DSeparable(data_in, data_out, cols, rows, tmp_kernel, sx, tmp_kernel, sx, norm*norm, DIV_NORMAL);			
			//Mat out_img = Mat(rows, cols, CV_8UC1, data_out);
			//out_img.copyTo(_dst);
			//imwrite("src_blurred_2.bmp", out_img);


			//Mat K = (Mat_<double>(ksize,1) << 1./17, 4./17, 7./17, 4./17, 1./17);
			Mat K = Mat(1, sx, CV_32FC1, kernel);
			//for(i=0; i<sx; i++)
			//	printf("K[%d] = %f\n", i, K.at<float>(0, i));

			cv::sepFilter2D(_src, _dst, -1 , K, K, Point( -1, -1 ), 0, BORDER_REPLICATE );
			imwrite("src_blurred_2.bmp", _dst);


			free(kernel);
			

		}else if(custom == 3){	// Custom Gaussian Separable 2D filter (integer >1 kernel coefficients, uchar data, SHIFT instead of division)
			int h;
			long long * tmp_kernel;

			get_custom_coeff_vector(sx, sigma1, &tmp_kernel, &norm);
			
			long long norm2 = round_to_pow2(norm*norm);
			//printf("norm2_rnd=%ld, log2(norm2)=%ld\n", norm2,  (long)log2(norm2));
			//printf("tmp_kernel: ");
			//for(h=0; h<sx; h++) printf("%ld ", tmp_kernel[h]);
			//printf("\n");
			convolve2DSeparable(data_in, data_out, cols, rows, tmp_kernel, sx, tmp_kernel, sx, (long long) log2(norm2), DIV_SHIFT);
			

			Mat out_img = Mat(rows, cols, CV_8UC1, data_out);

			out_img.copyTo(_dst);
			imwrite("src_blurred_3.bmp", _dst);

			/*
			Mat tmp;
		  Size src_size=_dst.size();
		  src_size.height=src_size.height-(sx/2);
		  src_size.width=src_size.width-(sx/2);
		  resize(_dst, tmp, src_size);
			imwrite("src_blurred_3.bmp", tmp);
			*/

		}else{					// Custom Linear2D filter applied

			// Create 2D custom kernel (float)
			createCustomGaussianMask(sx, sigma1, custom_kernel, &norm, CV_32F, custom);
			
			
			printf(" -- Linear2D filter used with coefficients (norm=%lld)\n", norm);
			for(i=0; i<sx; i++){
				for(j=0; j<sx; j++){
					printf("%6ld ", (long)(custom_kernel.at<float>(j,i)*norm));
				}
				printf("\n");
			}
			cv::filter2D(_src, _dst, CV_MAT_DEPTH(type), custom_kernel, Point(-1,-1), 0, borderType);
		}
		
		imwrite("src_blurred.bmp", _dst);
		free(data_out);
	}


	void createGaussianKernels( Mat & kx, Mat & ky, int type, Size ksize, double sigma1, double sigma2 )
	{
		//printf("createGaussianKernels modified used!\n");

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
		//printf("getGaussianKernel modified used!\n");

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
	    // 


	    // alfa = 1 / {e^-2};
	    // for
	    //   G[i]=ci[i]=(int) alfa*cf[i]; Gi interi approssimati

	    // Normalizzazione
	    // for i
	    //   sum += Gi
	    //
	    // 1 / 17 e' 


	    

	    return kernel;
	}

	void createCustomGaussianMask(int ksize, double sigma, Mat & kernel, long long *normalization, int type, int custom){
		/*
		int i=0, j=0;
		long *coeffs;
		long norm;
		if(custom == 4){
			get_custom_coeff_matrix(ksize, sigma, &coeffs, &norm);
		}else if(custom == 5){
			get_custom_pow2_coeff_matrix(ksize, sigma, &coeffs, &norm);
		}else if(custom == 6){
			get_custom_pow2_coeff_matrix(ksize, sigma, &coeffs, &norm);
			long tmp = round_to_pow2(norm);
			printf("-debug: Normalization factor = %ld, rounded = %ld, error = %lf\n", norm, tmp, (norm-tmp)/((double)norm));
			norm = tmp;
		}else{
			get_custom_coeff_matrix(ksize, sigma, &coeffs, &norm);
			long tmp = round_to_pow2(norm);
			printf("-debug: Normalization factor = %ld, rounded = %ld, error = %lf\n", norm, tmp, (norm-tmp)/((double)norm));
			norm = tmp;
		}

		for(i=0; i<ksize; i++){
			for(j=0; j<ksize; j++){
				if(type == CV_32F)
					kernel.at<float>(j, i) = (float)((double)coeffs[i*ksize + j] / (double)norm);
			}
		}
		
		*normalization=norm;
		*/
	}

}// end namespace my_Space 
