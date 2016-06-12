/*
 * opencv_Canny_arm.cpp
 *
 *  Created on: 22 apr 2016
 *  
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sys/time.h>
#include "global.h"
#include "opencv_Canny_arm.h"



namespace my_Space
{
	void Canny( InputArray _src, OutputArray _dst,
					double low_thresh, double high_thresh,
					int aperture_size, bool L2gradient, int custom)
	{
		//printf("Canny modified used!\n");
		struct timeval start, stop;

		const int type = _src.type();			//type of a matrix element (pixel)
		const int depth = CV_MAT_DEPTH(type);	//type of each individual channel
		const int cn = CV_MAT_CN(type);			//number of channels of the image
		const Size size = _src.size();			//matrix size: Size(cols, rows) . When the matrix is more than 2-dimensional, the returned size is (-1, -1).

		//Check that channels are of type CV_8U (Unsigned 8bit) and create a destination matrix of the same type
		CV_Assert( depth == CV_8U );
		_dst.create(size, CV_8U);

		//if L1_norm and aperture_size is negative
		if (!L2gradient && (aperture_size & CV_CANNY_L2_GRADIENT) == CV_CANNY_L2_GRADIENT)
		{
			// backward compatibility
			aperture_size &= ~CV_CANNY_L2_GRADIENT;
			L2gradient = true;
		}

		//aperture_size must be an odd number and not greater than 7
		if ((aperture_size & 1) == 0 || (aperture_size != -1 && (aperture_size < 3 || aperture_size > 7)))
			CV_Error(CV_StsBadFlag, "Aperture size should be odd");

		//swap threshold values if they are inverted
		if (low_thresh > high_thresh)
			std::swap(low_thresh, high_thresh);

		int fd;
		if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1){
			printf("Error opening /dev/mem\nExiting..\n");
			return;
		}

		void *src_p, *dst_p, *dx_p, *dy_p;
		unsigned int src_addr = 0x1F800000;
		unsigned int dst_addr = 0x1F500000;
		unsigned int dx_addr  = 0x1F600000;
		unsigned int dy_addr  = 0x1F700000;

		unsigned int buffer_size = 0x100000;

		src_p = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, src_addr);
		if(src_p == MAP_FAILED) {printf("Error mapping src_p.\n"); return;}
		dst_p = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, dst_addr);
		if(dst_p == MAP_FAILED) {printf("Error mapping src_p.\n"); return;}
		dx_p  = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, dx_addr);
		if(dx_p == MAP_FAILED)  {printf("Error mapping src_p.\n"); return;}
		dy_p  = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, dy_addr);
		if(dy_p == MAP_FAILED)  {printf("Error mapping src_p.\n"); return;}


		Mat src;
		src.data = (unsigned char*) src_p;
		src = _src.getMat();

		Mat dst;
		dst.data = (unsigned char *) dst_p;
		dst = _dst.getMat();

		//create partial derivative matrices. Size is the same of the source image.
		// 1 channel matrices but each element is represented with 16bit signed (ex: -2*255 = -510 -> 16bit signed)
		Mat dx_tmp(src.rows, src.cols, CV_16SC(cn));
		dx_tmp.data = (unsigned char *) dx_p;
		Mat dy_tmp(src.rows, src.cols, CV_16SC(cn));
		dy_tmp.data = (unsigned char *) dy_p;


		//Mat dx(src.rows, src.cols, CV_16SC(cn));
		//dx.data = (unsigned char *) dx_p;
		//Mat dy(src.rows, src.cols, CV_16SC(cn));
		//dy.data = (unsigned char *) dy_p;
		

		Mat gradient(src.rows, src.cols, CV_16SC(cn));

		//Compute partial derivatives using Sobel operator/kernel
		gettimeofday(&start, NULL);
		my_Space::Sobel(src, dx_tmp, CV_16S, 1, 0, aperture_size, 1, 0, BORDER_REPLICATE, custom);		//x component of the gradient
		my_Space::Sobel(src, dy_tmp, CV_16S, 0, 1, aperture_size, 1, 0, BORDER_REPLICATE, custom);		//y component of the gradient
		//my_Space::Sobel(src, dx, CV_16S, 1, 0, aperture_size, 1, 0, BORDER_REPLICATE, custom);		//x component of the gradient
		//my_Space::Sobel(src, dy, CV_16S, 0, 1, aperture_size, 1, 0, BORDER_REPLICATE, custom);		//y component of the gradient
	  gettimeofday(&stop, NULL);
	  printf("SobelDx AND SobelDy wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
		
	  Mat dx;
	  dx_tmp.copyTo(dx);
	  Mat dy;
	  dy_tmp.copyTo(dy);

		if (L2gradient)
		{
			low_thresh = std::min(32767.0, low_thresh);		//max positive number on 16 bit = 2^15 - 1
			high_thresh = std::min(32767.0, high_thresh);

			if (low_thresh > 0) low_thresh *= low_thresh;
			if (high_thresh > 0) high_thresh *= high_thresh;
		}
		int low = cvFloor(low_thresh);
		int high = cvFloor(high_thresh);

		/*
		 * Temporary buffer.
		 * If a temporary buffer is usually small (a few K's of memory), but its size depends on the parameters,
		 * it makes sense to create a small fixed-size array on stack and use it if it's large enough.
		 * If the required buffer size is larger than the fixed size, another buffer of sufficient size is allocated dynamically and released after the processing.
		 * Therefore, in typical cases, when the buffer size is small, there is no overhead associated with malloc()/free().
		 * At the same time, there is no limit on the size of processed data.
		 */
		ptrdiff_t mapstep = src.cols + 2;	//long int type
		AutoBuffer<uchar> buffer((src.cols+2)*(src.rows+2) + cn * mapstep * 3 * sizeof(int));

		int* mag_buf[3];
		mag_buf[0] = (int*)(uchar*)buffer;
		mag_buf[1] = mag_buf[0] + mapstep*cn;
		mag_buf[2] = mag_buf[1] + mapstep*cn;
		memset(mag_buf[0], 0, /* cn* */mapstep*sizeof(int));

		uchar* map = (uchar*)(mag_buf[2] + mapstep*cn);
		memset(map, 1, mapstep);
		memset(map + mapstep*(src.rows + 1), 1, mapstep);

		int maxsize = std::max(1 << 10, src.cols * src.rows / 10);
		std::vector<uchar*> stack(maxsize);
		uchar **stack_top = &stack[0];
		uchar **stack_bottom = &stack[0];

		/* sector numbers
		   (Top-Left Origin)

			1   2   3
			 *  *  *
			  * * *
			0*******0
			  * * *
			 *  *  *
			3   2   1
		*/

		gettimeofday(&start, NULL);
		my_Space::nonMaxSuppress(src, cn, dx, dy, gradient, mapstep, mag_buf, map, &maxsize, &stack, &stack_top, &stack_bottom, low, high, L2gradient);
	  gettimeofday(&stop, NULL);
	  printf("nonMaxSuppress wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
		

		gettimeofday(&start, NULL);
		my_Space::hysteresisThresh(mapstep, &maxsize, &stack, &stack_top, &stack_bottom);
		gettimeofday(&stop, NULL);
	  printf("hysteresisThresh wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
		

		// the final step, form the final image
		const uchar* pmap = map + mapstep + 1;
		uchar* pdst = dst.ptr();
		for (int i = 0; i < src.rows; i++, pmap += mapstep, pdst += dst.step)
		{
			for (int j = 0; j < src.cols; j++)
				pdst[j] = (uchar)-(pmap[j] >> 1);
		}
	}

	void nonMaxSuppress(Mat src, int cn, Mat dx, Mat dy, Mat gradient, ptrdiff_t mapstep, int* mag_buf[], uchar* map, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom,
					double low, double high,
					bool L2gradient)
	{

		#define CANNY_PUSH(d)   *(d) = uchar(2), *(*stack_top)++ = (d)
		#define CANNY_POP(d)     (d) = *--(*stack_top)
		// calculate magnitude and angle of gradient, perform non-maxima suppression.
		// fill the map with one of the following values:
		//   0 - the pixel might belong to an edge
		//   1 - the pixel can not belong to an edge
		//   2 - the pixel does belong to an edge
		for (int i = 0; i <= src.rows; i++)
		{

			int* _norm = mag_buf[(i > 0) + 1] + 1;
			if (i < src.rows)
			{
				short* _dx = dx.ptr<short>(i);
				short* _dy = dy.ptr<short>(i);
				short* _gradient = gradient.ptr<short>(i);

				if (!L2gradient)
				{
					int j = 0, width = src.cols * cn;
					for ( ; j < width; ++j)
						_norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
				}
				else
				{
					int j = 0, width = src.cols * cn;
					for ( ; j < width; ++j)
						_norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
				}

				if (cn > 1)
				{
					for(int j = 0, jn = 0; j < src.cols; ++j, jn += cn)
					{
						int maxIdx = jn;
						for(int k = 1; k < cn; ++k)
							if(_norm[jn + k] > _norm[maxIdx]) maxIdx = jn + k;
						_norm[j] = _norm[maxIdx];
						_dx[j] = _dx[maxIdx];
						_dy[j] = _dy[maxIdx];
					}
				}
				_norm[-1] = _norm[src.cols] = 0;
			}
			else
				memset(_norm-1, 0, /* cn* */mapstep*sizeof(int));

			// at the very beginning we do not have a complete ring
			// buffer of 3 magnitude rows for non-maxima suppression
			if (i == 0)
				continue;

			uchar* _map = map + mapstep*i + 1;
			_map[-1] = _map[src.cols] = 1;

			int* _mag = mag_buf[1] + 1; // take the central row
			ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
			ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];

			const short* _x = dx.ptr<short>(i-1);
			const short* _y = dy.ptr<short>(i-1);

			if ((*stack_top - *stack_bottom) + src.cols > *maxsize)
			{
				int sz = (int)(*stack_top - *stack_bottom);
				*maxsize = std::max(*maxsize * 3/2, sz + src.cols);
				(*stack).resize(*maxsize);
				*stack_bottom = &(*stack)[0];
				*stack_top = *stack_bottom + sz;
			}

			int prev_flag = 0;
			for (int j = 0; j < src.cols; j++)
			{
				#define CANNY_SHIFT 15
				const int TG22 = (int)(0.4142135623730950488016887242097*(1<<CANNY_SHIFT) + 0.5);

				int m = _mag[j];

				if (m > low)
				{
					int xs = _x[j];
					int ys = _y[j];
					int x = std::abs(xs);
					int y = std::abs(ys) << CANNY_SHIFT;

					int tg22x = x * TG22;

					if (y < tg22x)
					{
						if (m > _mag[j-1] && m >= _mag[j+1]) goto __ocv_canny_push;
					}
					else
					{
						int tg67x = tg22x + (x << (CANNY_SHIFT+1));
						if (y > tg67x)
						{
							if (m > _mag[j+magstep2] && m >= _mag[j+magstep1]) goto __ocv_canny_push;
						}
						else
						{
							int s = (xs ^ ys) < 0 ? -1 : 1;
							if (m > _mag[j+magstep2-s] && m > _mag[j+magstep1+s]) goto __ocv_canny_push;
						}
					}
				}
				prev_flag = 0;
				_map[j] = uchar(1);
				continue;

				__ocv_canny_push:
				if (!prev_flag && m > high && _map[j-mapstep] != 2)
				{
					CANNY_PUSH(_map + j);
					prev_flag = 1;
				}
				else
					_map[j] = 0;
			}

			// scroll the ring buffer
			_mag = mag_buf[0];
			mag_buf[0] = mag_buf[1];
			mag_buf[1] = mag_buf[2];
			mag_buf[2] = _mag;
		}

		return;

	}

	void hysteresisThresh(ptrdiff_t mapstep, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom)
	{

		// now track the edges (hysteresis thresholding)
		while (*stack_top > *stack_bottom)
		{
			uchar* m;
			if ((*stack_top - *stack_bottom) + 8 > *maxsize)
			{
				int sz = (int)(*stack_top - *stack_bottom);
				*maxsize = *maxsize * 3/2;
				(*stack).resize(*maxsize);
				*stack_bottom = &(*stack)[0];
				*stack_top = *stack_bottom + sz;
			}

			CANNY_POP(m);

			if (!m[-1])         CANNY_PUSH(m - 1);
			if (!m[1])          CANNY_PUSH(m + 1);
			if (!m[-mapstep-1]) CANNY_PUSH(m - mapstep - 1);
			if (!m[-mapstep])   CANNY_PUSH(m - mapstep);
			if (!m[-mapstep+1]) CANNY_PUSH(m - mapstep + 1);
			if (!m[mapstep-1])  CANNY_PUSH(m + mapstep - 1);
			if (!m[mapstep])    CANNY_PUSH(m + mapstep);
			if (!m[mapstep+1])  CANNY_PUSH(m + mapstep + 1);
		}

	}


	void Sobel( InputArray _src, OutputArray _dst, int ddepth, int dx, int dy,
					int ksize, double scale, double delta, int borderType, int custom)
	{
		//printf("Sobel modified used!\n");

		int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), cn = CV_MAT_CN(stype);
		if (ddepth < 0)
			ddepth = sdepth;
		int dtype = CV_MAKE_TYPE(ddepth, cn);
		_dst.create( _src.size(), dtype );

		int ktype = std::max(CV_32F, std::max(ddepth, sdepth));

		Mat kx, ky;
		my_Space::getDerivKernels( kx, ky, dx, dy, ksize, false, ktype );

		

		if( scale != 1 )
		{
			// usually the smoothing part is the slowest to compute,
			// so try to scale it instead of the faster differenciating part
			if( dx == 0 )
				kx *= scale;
			else
				ky *= scale;
		}

		if(custom == 0){
			sepFilter2D( _src, _dst, ddepth, kx, ky, Point(-1, -1), delta, borderType );
		}else if(custom == 2){
			int i=0;
			float * kernel_x = (float*)malloc(ksize * sizeof(float));
			float * kernel_y = (float*)malloc(ksize * sizeof(float));
			//printf("printing kx kernel: ");
			for(i=0; i<ksize; i++){
				kernel_x[i]=kx.at<float>(0, i);
				kernel_y[i]=ky.at<float>(0, i);
				//printf("%f ", kx.at<float>(0, i));
			}
				

			//printf("printing ky kernel: ");
			//for(i=0; i<ksize; i++)
				//printf("%f ", ky.at<float>(0, i));
			
			// replace sepFilter2D with convolve2D
			//bool convolve2DSeparable(unsigned short* in, unsigned short* out, int dataSizeX, int dataSizeY, float* kernelX, int kSizeX, float* kernelY, int kSizeY)
			Mat in_img = _src.getMat();
			//Mat out_img;
			int cols = in_img.cols;
			int rows = in_img.rows;
			unsigned char * data_in = in_img.data;;
			unsigned short * data_out = (unsigned short *) malloc(cols*rows*sizeof(unsigned short));
			
			convolve2DSeparable(data_in, data_out, cols, rows, kernel_x, ksize, kernel_y, ksize);
			Mat out_img = Mat(rows, cols, CV_8UC1, data_out);
			out_img.copyTo(_dst);
		}
		
	}

	void getDerivKernels( OutputArray kx, OutputArray ky, int dx, int dy,
	                          int ksize, bool normalize, int ktype )
	{
		//printf("getDerivKernels modified used!\n");

	    if( ksize <= 0 )
	        getScharrKernels( kx, ky, dx, dy, normalize, ktype );
	    else
	        getSobelKernels( kx, ky, dx, dy, ksize, normalize, ktype );
	}

	void getScharrKernels( OutputArray _kx, OutputArray _ky,
	                              int dx, int dy, bool normalize, int ktype )
	{
		//printf("getScharrKernels modified used!\n");

		const int ksize = 3;

	    CV_Assert( ktype == CV_32F || ktype == CV_64F );
	    _kx.create(ksize, 1, ktype, -1, true);
	    _ky.create(ksize, 1, ktype, -1, true);
	    Mat kx = _kx.getMat();
	    Mat ky = _ky.getMat();

	    CV_Assert( dx >= 0 && dy >= 0 && dx+dy == 1 );

	    for( int k = 0; k < 2; k++ )
	    {
	        Mat* kernel = k == 0 ? &kx : &ky;
	        int order = k == 0 ? dx : dy;
	        int kerI[3];

	        if( order == 0 )
	            kerI[0] = 3, kerI[1] = 10, kerI[2] = 3;
	        else if( order == 1 )
	            kerI[0] = -1, kerI[1] = 0, kerI[2] = 1;

	        Mat temp(kernel->rows, kernel->cols, CV_32S, &kerI[0]);
	        double scale = !normalize || order == 1 ? 1. : 1./32;
	        temp.convertTo(*kernel, ktype, scale);
	    }
	}


	void getSobelKernels( OutputArray _kx, OutputArray _ky,
	                             int dx, int dy, int _ksize, bool normalize, int ktype )
	{
		//printf("getSobelKernels modified used!\n");

		int i, j, ksizeX = _ksize, ksizeY = _ksize;
	    if( ksizeX == 1 && dx > 0 )
	        ksizeX = 3;
	    if( ksizeY == 1 && dy > 0 )
	        ksizeY = 3;

	    CV_Assert( ktype == CV_32F || ktype == CV_64F );

	    _kx.create(ksizeX, 1, ktype, -1, true);
	    _ky.create(ksizeY, 1, ktype, -1, true);
	    Mat kx = _kx.getMat();
	    Mat ky = _ky.getMat();

	    if( _ksize % 2 == 0 || _ksize > 31 )
	        CV_Error( CV_StsOutOfRange, "The kernel size must be odd and not larger than 31" );
	    std::vector<int> kerI(std::max(ksizeX, ksizeY) + 1);

	    CV_Assert( dx >= 0 && dy >= 0 && dx+dy > 0 );

	    for( int k = 0; k < 2; k++ )
	    {
	        Mat* kernel = k == 0 ? &kx : &ky;
	        int order = k == 0 ? dx : dy;
	        int ksize = k == 0 ? ksizeX : ksizeY;

	        CV_Assert( ksize > order );

	        if( ksize == 1 )
	            kerI[0] = 1;
	        else if( ksize == 3 )
	        {
	            if( order == 0 )
	                kerI[0] = 1, kerI[1] = 2, kerI[2] = 1;
	            else if( order == 1 )
	                kerI[0] = -1, kerI[1] = 0, kerI[2] = 1;
	            else
	                kerI[0] = 1, kerI[1] = -2, kerI[2] = 1;
	        }
	        else
	        {
	            int oldval, newval;
	            kerI[0] = 1;
	            for( i = 0; i < ksize; i++ )
	                kerI[i+1] = 0;

	            for( i = 0; i < ksize - order - 1; i++ )
	            {
	                oldval = kerI[0];
	                for( j = 1; j <= ksize; j++ )
	                {
	                    newval = kerI[j]+kerI[j-1];
	                    kerI[j-1] = oldval;
	                    oldval = newval;
	                }
	            }

	            for( i = 0; i < order; i++ )
	            {
	                oldval = -kerI[0];
	                for( j = 1; j <= ksize; j++ )
	                {
	                    newval = kerI[j-1] - kerI[j];
	                    kerI[j-1] = oldval;
	                    oldval = newval;
	                }
	            }
	        }

	        Mat temp(kernel->rows, kernel->cols, CV_32S, &kerI[0]);
	        double scale = !normalize ? 1. : 1./(1 << (ksize-order-1));
	        temp.convertTo(*kernel, ktype, scale);
	    }
	}
}
