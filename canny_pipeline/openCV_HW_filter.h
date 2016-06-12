

#include "global_parameters.h"

#include "vdma.h"
#include "vdma_parameters.h"
#include "sepImageFilter.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core.hpp"

using namespace cv;
using namespace std;


void GaussianBlur_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out);

void SobelDx_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out);

void SobelDy_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out);

void Canny_HW( vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, Mat dst_img, Mat sobelDxOut, void * dx_virtAddr, Mat sobelDyOut, void * dy_virtAddr, unsigned int width, unsigned int height, unsigned int sobelIn_fbAddr, unsigned int sobelDxOut_fbAddr, unsigned int sobelDyOut_fbAddr, double low_thresh, double high_thresh, int aperture_size, bool L2gradient );

void nonMaxSuppress(int rows, int cols, int cn, Mat dx, Mat dy, ptrdiff_t mapstep, int* mag_buf[], uchar* map, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom, double threshold1, double threshold2, bool L2gradient = false );

void hysteresisThresh(ptrdiff_t mapstep, int* maxsize, std::vector<uchar*> *stack, uchar*** stack_top, uchar*** stack_bottom);
