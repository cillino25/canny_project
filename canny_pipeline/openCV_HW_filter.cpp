
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>

#include "openCV_HW_filter.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core.hpp"

using namespace cv;
using namespace std;

// mm2s_fb1: 0x1f400000
// s2mm_fb1: 0x1f500000
// sepImageFilter_config(filter_handle, 1, 0, 0);
void GaussianBlur_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out){
	
	printf("GaussianBlur_HW launched.\n");

	struct timeval start1, start2, stop;

	//gettimeofday(&start1, NULL);
	// Set VDMA fb in and out
	vdma_setParams(vdma_handle, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, fb_in, 0, 0, fb_out, 0, 0);
	//vdma_setFbInOut(vdma_handle, fb_in, fb_out);

	vdma_start_triple_buffering_mod(vdma_handle);
	vdma_reset_fsync(vdma_handle);
	// Set sepImageFilter config register
	sepImageFilter_config(filter_handle, 1, 0, 0);

	// Send sepImageFilter start
	sepImageFilter_start(filter_handle);

	// Send fsync
	//printf("Sending fsync..\n");
	vdma_send_fsync(vdma_handle);

	//gettimeofday(&start2, NULL);

	// Wait for VDMA complete memory write
	// while(vdma_mm2s_running(vdma_handle));
	printf("Waiting for filter..\n");
	while(sepImageFilter_running(filter_handle) != 0);

	//gettimeofday(&stop, NULL);

	//printf("Time profiling: parameter setting:  %lf ms\n", ((start2.tv_sec + start2.tv_usec*0.000001)-(start1.tv_sec + start1.tv_usec*0.000001))*PRESC * 1000);
	//printf("Time profiling: filter computation: %lf ms\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start2.tv_sec + start2.tv_usec*0.000001))*PRESC * 1000);


}


// mm2s_fb1: 0x1f500000
// s2mm_fb1: 0x1f600000
// sepImageFilter_config(filter_handle, 0, 0, 1);
void SobelDx_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out){
	// Set VDMA fb in and out
	vdma_setParams(vdma_handle, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, fb_in, 0, 0, fb_out, 0, 0);
	//vdma_setFbInOut(vdma_handle, fb_in, fb_out);
	
	vdma_start_triple_buffering_mod(vdma_handle);
	vdma_reset_fsync(vdma_handle);
	// Set sepImageFilter config register
	sepImageFilter_config(filter_handle, 0, 0, 1);

	// Send sepImageFilter start
	sepImageFilter_start(filter_handle);

	// Send fsync 
	vdma_send_fsync(vdma_handle);

	// Wait for VDMA complete memory write
	// while(vdma_mm2s_running(vdma_handle));
	while(sepImageFilter_running(filter_handle) != 0);
}


// mm2s_fb1: 0x1f500000
// s2mm_fb1: 0x1f700000
// sepImageFilter_config(filter_handle, 0, 0, 2);
void SobelDy_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out){
	// Set VDMA fb in and out
	vdma_setParams(vdma_handle, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, fb_in, 0, 0, fb_out, 0, 0);
	//vdma_setFbInOut(vdma_handle, fb_in, fb_out);

	vdma_start_triple_buffering_mod(vdma_handle);
	vdma_reset_fsync(vdma_handle);
	// Set sepImageFilter config register
	sepImageFilter_config(filter_handle, 0, 0, 2);

	// Send sepImageFilter start
	sepImageFilter_start(filter_handle);

	// Send fsync 
	vdma_send_fsync(vdma_handle);

	// Wait for VDMA complete memory write
	// while(vdma_mm2s_running(vdma_handle));
	while(sepImageFilter_running(filter_handle) != 0);
}


void Canny_HW_ARM_only( vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, Mat dst_img, Mat sobelDxOut, void * dx_virtAddr, Mat sobelDyOut, void * dy_virtAddr, unsigned int width, unsigned int height, unsigned int sobelIn_fbAddr, unsigned int sobelDxOut_fbAddr, unsigned int sobelDyOut_fbAddr, double low_thresh, double high_thresh, int aperture_size, bool L2gradient )
{
	//printf("Canny HARDWARE used!\n");
	struct timeval start, stop;

	const int depth = CV_16U;	//type of each individual channel
	const int cn = 1;			//number of channels of the image
	

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

	

	//Compute partial derivatives using Sobel operator/kernel
	gettimeofday(&start, NULL);
	SobelDx_HW(vdma_handle, filter_handle, width, height, sobelIn_fbAddr, sobelDxOut_fbAddr);
	SobelDy_HW(vdma_handle, filter_handle, width, height, sobelIn_fbAddr, sobelDyOut_fbAddr); 
  gettimeofday(&stop, NULL);
  printf("SobelDx AND SobelDy wall time: %lf ms\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	
  // polling variable to 1 --> send execution to rocketchip

	// Copy the images back into OS memory (cached), so nonMaxSuppress will benefit of cacheable memory
	gettimeofday(&start, NULL);
  memcpy((void*)sobelDxOut.data, dx_virtAddr, width*height*sizeof(short));
  memcpy((void*)sobelDyOut.data, dy_virtAddr, width*height*sizeof(short));
  gettimeofday(&stop, NULL);
  printf("...copying wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	

	//swap threshold values if they are inverted
	if (low_thresh > high_thresh)
		std::swap(low_thresh, high_thresh);


	if (L2gradient)
	{
		low_thresh = std::min(32767.0, low_thresh);		//max positive number on 16 bit = 2^15 - 1
		high_thresh = std::min(32767.0, high_thresh);

		if (low_thresh > 0) low_thresh *= low_thresh;
		if (high_thresh > 0) high_thresh *= high_thresh;
	}
	int low = cvFloor(low_thresh);
	int high = cvFloor(high_thresh);

	
	ptrdiff_t mapstep = width + 2;	//long int type
	AutoBuffer<uchar> buffer((width+2)*(height+2) + cn * mapstep * 3 * sizeof(int));

	int* mag_buf[3];
	mag_buf[0] = (int*)(uchar*)buffer;
	mag_buf[1] = mag_buf[0] + mapstep*cn;
	mag_buf[2] = mag_buf[1] + mapstep*cn;
	memset(mag_buf[0], 0, /* cn* */mapstep*sizeof(int));

	uchar* map = (uchar*)(mag_buf[2] + mapstep*cn);
	memset(map, 1, mapstep);
	memset(map + mapstep*(height + 1), 1, mapstep);

	int maxsize = std::max(1 << 10, (int)(width * height / 10));
	std::vector<uchar*> stack(maxsize);
	uchar **stack_top = &stack[0];
	uchar **stack_bottom = &stack[0];

	

	gettimeofday(&start, NULL);
	nonMaxSuppress(height, width, cn, sobelDxOut, sobelDyOut, mapstep, mag_buf, map, &maxsize, &stack, &stack_top, &stack_bottom, low, high, L2gradient);
  gettimeofday(&stop, NULL);
  printf("nonMaxSuppress wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
	

	gettimeofday(&start, NULL);
	hysteresisThresh(mapstep, &maxsize, &stack, &stack_top, &stack_bottom);
	gettimeofday(&stop, NULL);
  printf("hysteresisThresh wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
	

	// the final step, form the final image
	gettimeofday(&start, NULL);
	const uchar* pmap = map + mapstep + 1;
	uchar* pdst = dst_img.ptr();
	for (int i = 0; i < height; i++, pmap += mapstep, pdst += dst_img.step)
	{
		for (int j = 0; j < width; j++)
			pdst[j] = (uchar)-(pmap[j] >> 1);
	}
	gettimeofday(&stop, NULL);
  printf("Final image creation wall time: %lf s\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC);
	
}



void Canny_HW_ARM( vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int sobelIn_fbAddr, unsigned int sobelDxOut_fbAddr, unsigned int sobelDyOut_fbAddr )
{
	struct timeval start, stop;
	//Compute partial derivatives using Sobel operator/kernel
	gettimeofday(&start, NULL);
	SobelDx_HW(vdma_handle, filter_handle, width, height, sobelIn_fbAddr, sobelDxOut_fbAddr);
	SobelDy_HW(vdma_handle, filter_handle, width, height, sobelIn_fbAddr, sobelDyOut_fbAddr); 
  gettimeofday(&stop, NULL);
  printf("SobelDx AND SobelDy wall time: %lf ms\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	
}

void Canny_HW_RC( void * destImage_virtAddr, void * dx_virtAddr, void * dy_virtAddr, unsigned int width, unsigned int height, double low_thresh, double high_thresh, int aperture_size, bool L2gradient )
{
	printf("Canny HW RocketChip part\nInitialize variables..\n");
	int i=0;
	struct timeval start, stop;
	const int depth = CV_16U;	//type of each individual channel
	const int cn = 1;			//number of channels of the image

	Mat sobelDxOut(height, width, CV_16SC(1));
	Mat sobelDyOut(height, width, CV_16SC(1));
	Mat dst_img(height, width, CV_8U);

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

  // Last steps of Canny algorithm are done by Rocketchip; ARM 

	//char * dx_data = (char*) malloc(width*height*sizeof(short));
	//char * dy_data = (char*) malloc(width*height*sizeof(short));

	// Copy the images back into OS memory (cached), so nonMaxSuppress will benefit of cacheable memory
	printf("Copy images into OS memory\n");
	gettimeofday(&start, NULL);
	memcpy((void*)sobelDxOut.data, dx_virtAddr, width*height*sizeof(short));
  memcpy((void*)sobelDyOut.data, dy_virtAddr, width*height*sizeof(short));
  gettimeofday(&stop, NULL);
  printf("...copying wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	
  gettimeofday(&start, NULL);
	if (L2gradient)
	{
		low_thresh = std::min(32767.0, low_thresh);		//max positive number on 16 bit = 2^15 - 1
		high_thresh = std::min(32767.0, high_thresh);

		if (low_thresh > 0) low_thresh *= low_thresh;
		if (high_thresh > 0) high_thresh *= high_thresh;
	}
	int low = cvFloor(low_thresh);
	int high = cvFloor(high_thresh);

	ptrdiff_t mapstep = width + 2;	//long int type
	AutoBuffer<uchar> buffer((width+2)*(height+2) + cn * mapstep * 3 * sizeof(int));
	
	int* mag_buf[3];
	
	mag_buf[0] = (int*)(uchar*)buffer;
	mag_buf[1] = mag_buf[0] + mapstep*cn;
	mag_buf[2] = mag_buf[1] + mapstep*cn;
	for(i=0; i<mapstep*sizeof(int); i++) *(mag_buf[0]+i)=0;					//memset(mag_buf[0], 0, /* cn* */mapstep*sizeof(int));
	

	//printf("map creation and initialization\n");
	unsigned char* map = (unsigned char*)(mag_buf[2] + mapstep*cn);
	for(i=0; i<mapstep; i++) map[i]=1;													//memset(map, 1, mapstep);
	for(i=0; i<mapstep; i++) *(map+mapstep*(height+1)+i)=1;			//memset(map + mapstep*(height + 1), 1, mapstep);

	//printf("Stack initialization\n");
	int maxsize = std::max(1 << 10, (int)(width * height / 10));
	std::vector<uchar*> stack(maxsize);
	uchar **stack_top = &stack[0];
	uchar **stack_bottom = &stack[0];
	
	gettimeofday(&stop, NULL);
  printf("Initialization wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	

	
	//printf("nonMaxSuppress\n");
	gettimeofday(&start, NULL);

	#define CANNY_PUSH(d)   *(d) = uchar(2), *stack_top++ = (d)
	#define CANNY_POP(d)     (d) = *--stack_top
	// calculate magnitude and angle of gradient, perform non-maxima suppression.
	// fill the map with one of the following values:
	//   0 - the pixel might belong to an edge
	//   1 - the pixel can not belong to an edge
	//   2 - the pixel does belong to an edge
	//printf("-d1\n");
	for (int i = 0; i <= height; i++)
	{

		int* _norm = mag_buf[(i > 0) + 1] + 1;
		if (i < height)
		{
			//short* _dx = sobelDxOut.ptr<short>(i); // returns a pointer to the specified matrix row
			short* _dx = (short*) (sobelDxOut.data + i*width*2);
			
			//short* _dy = sobelDyOut.ptr<short>(i);
			short* _dy = (short*) (sobelDyOut.data + i*width*2);
			
			if (!L2gradient)
			{
				int j = 0, width = width * cn;
				for ( ; j < width; ++j){
					_norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
				}
			}
			else
			{
				int j = 0, width = width * cn;
				for ( ; j < width; ++j)
					_norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
			}

			if (cn > 1)
			{
				for(int j = 0, jn = 0; j < width; ++j, jn += cn)
				{
					int maxIdx = jn;
					for(int k = 1; k < cn; ++k){
						if(_norm[jn + k] > _norm[maxIdx]) maxIdx = jn + k;
					}
					_norm[j] = _norm[maxIdx];
					_dx[j] = _dx[maxIdx];
					_dy[j] = _dy[maxIdx];
				}
			}
			_norm[-1] = 0;
		}
		else{
			//memset(_norm-1, 0, /* cn* */mapstep*sizeof(int));
			for(int i = 0; i < mapstep*sizeof(int); i++) *(_norm-1 + i) = 0;
		}

		// at the very beginning we do not have a complete ring
		// buffer of 3 magnitude rows for non-maxima suppression
		if (i == 0)
			continue;
		
		uchar* _map = map + mapstep*i + 1;
		_map[-1] = _map[width] = 1;

		int* _mag = mag_buf[1] + 1; // take the central row
		ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
		ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];

		const short* _x = sobelDxOut.ptr<short>(i-1);
		const short* _y = sobelDyOut.ptr<short>(i-1);
		
		if ((stack_top - stack_bottom) + width > maxsize)
		{
			int sz = (int)(stack_top - stack_bottom);
			maxsize = std::max(maxsize * 3/2, sz + (int)width);
			stack.resize(maxsize);
			stack_bottom = &stack[0];
			stack_top = stack_bottom + sz;
		}

		int prev_flag = 0;
		for (int j = 0; j < width; j++)
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

	gettimeofday(&stop, NULL);
  printf("nonMaxSuppress wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	


//	hysteresisThresh(mapstep, &maxsize, &stack, &stack_top, &stack_bottom);
	gettimeofday(&start, NULL);
	while (stack_top > stack_bottom)
	{
		uchar* m;
		if ((stack_top - stack_bottom) + 8 > maxsize)
		{
			int sz = (int)(stack_top - stack_bottom);
			maxsize = maxsize * 3/2;
			stack.resize(maxsize);
			stack_bottom = &stack[0];
			stack_top = stack_bottom + sz;
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
	gettimeofday(&stop, NULL);
  printf("hysteresisThresh wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	
	// the final step, form the final image
	gettimeofday(&start, NULL);
	const uchar* pmap = map + mapstep + 1;
	uchar* pdst = dst_img.ptr();
	for (int i = 0; i < height; i++, pmap += mapstep, pdst += dst_img.step)
	{
		for (int j = 0; j < width; j++)
			pdst[j] = (uchar)-(pmap[j] >> 1);
	}
	gettimeofday(&stop, NULL);
  printf("Final image creation wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC *1000);
	
	// Copy the final image to a memory buffer (it has to be read from the ARM in order to be displayed)
	gettimeofday(&start, NULL);
  memcpy(destImage_virtAddr, (void*)dst_img.data, width*height);
  gettimeofday(&stop, NULL);
  printf("Final image copy wall time: %lf ms\n\n", ((stop.tv_sec + stop.tv_usec*0.000001)-(start.tv_sec + start.tv_usec*0.000001))*PRESC * 1000);

  // Freeing prisoners
  //sobelDxOut.release();
  //sobelDyOut.release();
  //dst_img.release();
  ////free(stack);
  ////free(mag_buf);
  //buffer.deallocate();
}



void nonMaxSuppress(int rows, int cols, int cn, Mat dx, Mat dy, ptrdiff_t mapstep, int* mag_buf[], uchar* map, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom,	double low, double high, bool L2gradient)
{
	printf("entering nonMaxSuppress\n");
	#define CANNY_PUSH_MOD(d)   *(d) = uchar(2), *(*stack_top)++ = (d)
	#define CANNY_POP_MOD(d)     (d) = *--(*stack_top)
	// calculate magnitude and angle of gradient, perform non-maxima suppression.
	// fill the map with one of the following values:
	//   0 - the pixel might belong to an edge
	//   1 - the pixel can not belong to an edge
	//   2 - the pixel does belong to an edge
	for (int i = 0; i <= rows; i++)
	{

		int* _norm = mag_buf[(i > 0) + 1] + 1;
		if (i < rows)
		{
			short* _dx = dx.ptr<short>(i);
			short* _dy = dy.ptr<short>(i);
			
			
			if (!L2gradient)
			{
				int j = 0, width = cols * cn;
				for ( ; j < width; ++j){
					_norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
				}
			}
			else
			{
				int j = 0, width = cols * cn;
				for ( ; j < width; ++j)
					_norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
			}

			if (cn > 1)
			{
				for(int j = 0, jn = 0; j < cols; ++j, jn += cn)
				{
					int maxIdx = jn;
					for(int k = 1; k < cn; ++k)
						if(_norm[jn + k] > _norm[maxIdx]) maxIdx = jn + k;
					_norm[j] = _norm[maxIdx];
					_dx[j] = _dx[maxIdx];
					_dy[j] = _dy[maxIdx];
				}
			}
			_norm[-1] = _norm[cols] = 0;
		}
		else{
			//memset(_norm-1, 0, /* cn* */mapstep*sizeof(int));
			for(int i = 0; i < mapstep*sizeof(int); i++) *(_norm-1 + i) = 0;
		}

		
		// at the very beginning we do not have a complete ring
		// buffer of 3 magnitude rows for non-maxima suppression
		if (i == 0)
			continue;
		
		uchar* _map = map + mapstep*i + 1;
		_map[-1] = _map[cols] = 1;

		int* _mag = mag_buf[1] + 1; // take the central row
		ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
		ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];

		const short* _x = dx.ptr<short>(i-1);
		const short* _y = dy.ptr<short>(i-1);
		
		if ((*stack_top - *stack_bottom) + cols > *maxsize)
		{
			int sz = (int)(*stack_top - *stack_bottom);
			*maxsize = std::max(*maxsize * 3/2, sz + cols);
			(*stack).resize(*maxsize);
			*stack_bottom = &(*stack)[0];
			*stack_top = *stack_bottom + sz;
		}

		int prev_flag = 0;
		for (int j = 0; j < cols; j++)
		{
			//#define CANNY_SHIFT 15
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
				CANNY_PUSH_MOD(_map + j);
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

		CANNY_POP_MOD(m);

		if (!m[-1])         CANNY_PUSH_MOD(m - 1);
		if (!m[1])          CANNY_PUSH_MOD(m + 1);
		if (!m[-mapstep-1]) CANNY_PUSH_MOD(m - mapstep - 1);
		if (!m[-mapstep])   CANNY_PUSH_MOD(m - mapstep);
		if (!m[-mapstep+1]) CANNY_PUSH_MOD(m - mapstep + 1);
		if (!m[mapstep-1])  CANNY_PUSH_MOD(m + mapstep - 1);
		if (!m[mapstep])    CANNY_PUSH_MOD(m + mapstep);
		if (!m[mapstep+1])  CANNY_PUSH_MOD(m + mapstep + 1);
	}

}
