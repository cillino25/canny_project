#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"




void sepImageFilter_setupFilter(sepimgfilter_handle *handle){
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IMG_WIDTH_V_DATA, handle->width);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IMG_HEIGHT_V_DATA, handle->height);
	
	// Set kernel 0
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_0_V_DATA, handle->hz_kernel[0][0]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_1_V_DATA, handle->hz_kernel[0][1]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_2_V_DATA, handle->hz_kernel[0][2]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_3_V_DATA, handle->hz_kernel[0][3]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_4_V_DATA, handle->hz_kernel[0][4]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_5_V_DATA, handle->hz_kernel[0][5]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_6_V_DATA, handle->hz_kernel[0][6]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_0_0_V_DATA, handle->vt_kernel[0][0]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_0_1_V_DATA, handle->vt_kernel[0][1]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_0_2_V_DATA, handle->vt_kernel[0][2]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_0_3_V_DATA, handle->vt_kernel[0][3]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_0_4_V_DATA, handle->vt_kernel[0][4]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_5_V_DATA, handle->hz_kernel[0][5]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_0_6_V_DATA, handle->hz_kernel[0][6]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_DIVISOR_0_V_DATA, handle->normalization[0]);

	// Set kernel 1
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_0_V_DATA, handle->hz_kernel[1][0]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_1_V_DATA, handle->hz_kernel[1][1]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_2_V_DATA, handle->hz_kernel[1][2]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_3_V_DATA, handle->hz_kernel[1][3]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_4_V_DATA, handle->hz_kernel[1][4]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_5_V_DATA, handle->hz_kernel[1][5]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_6_V_DATA, handle->hz_kernel[1][6]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_1_0_V_DATA, handle->vt_kernel[1][0]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_1_1_V_DATA, handle->vt_kernel[1][1]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_1_2_V_DATA, handle->vt_kernel[1][2]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_1_3_V_DATA, handle->vt_kernel[1][3]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_1_4_V_DATA, handle->vt_kernel[1][4]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_5_V_DATA, handle->hz_kernel[1][5]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_1_6_V_DATA, handle->hz_kernel[1][6]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_DIVISOR_1_V_DATA, handle->normalization[1]);

	// Set kernel 2
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_0_V_DATA, handle->hz_kernel[2][0]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_1_V_DATA, handle->hz_kernel[2][1]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_2_V_DATA, handle->hz_kernel[2][2]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_3_V_DATA, handle->hz_kernel[2][3]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_4_V_DATA, handle->hz_kernel[2][4]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_5_V_DATA, handle->hz_kernel[2][5]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_6_V_DATA, handle->hz_kernel[2][6]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_2_0_V_DATA, handle->vt_kernel[2][0]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_2_1_V_DATA, handle->vt_kernel[2][1]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_2_2_V_DATA, handle->vt_kernel[2][2]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_2_3_V_DATA, handle->vt_kernel[2][3]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNELS_2_4_V_DATA, handle->vt_kernel[2][4]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_5_V_DATA, handle->hz_kernel[2][5]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNELS_2_6_V_DATA, handle->hz_kernel[2][6]);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_DIVISOR_2_V_DATA, handle->normalization[2]);
}

int sepImageFilter_setupHandle(sepimgfilter_handle *handle, int * mem_handler, unsigned int size, unsigned int baseAddr){
	handle->baseAddr=baseAddr;

	handle->imgFilterVirtualAddress = (unsigned int *)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, *mem_handler, handle->baseAddr);
	if(((unsigned int *)handle->imgFilterVirtualAddress) == MAP_FAILED){
		printf("imgFilterVirtualAddress mapping failed.\n");
		return -1;
	}

	return 0;
}

void sepImageFilter_config(sepimgfilter_handle *handle, int normalize, int truncate, int kernel_num){
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_CONFIG_V_DATA, ((normalize & 1)<<7) | ((truncate & 1)<<6) | (kernel_num & 63));
}

void sepImageFilter_setupHandleParams(sepimgfilter_handle *handle, unsigned int width, unsigned int height, int * k0_hz_coeffs, int * k0_vt_coeffs, unsigned int norm0, int * k1_hz_coeffs, int * k1_vt_coeffs, unsigned int norm1, int * k2_hz_coeffs, int * k2_vt_coeffs, unsigned int norm2)
{
	sepImageFilter_setImageSize(handle, width, height);

	sepImageFilter_setKernel0_HzCoeffs(handle, k0_hz_coeffs);
  sepImageFilter_setKernel0_VtCoeffs(handle, k0_vt_coeffs);
  sepImageFilter_setKernel0_NormalizationFactor(handle, norm0);

  sepImageFilter_setKernel1_HzCoeffs(handle, k1_hz_coeffs);
  sepImageFilter_setKernel1_VtCoeffs(handle, k1_vt_coeffs);
  sepImageFilter_setKernel1_NormalizationFactor(handle, norm1);

  sepImageFilter_setKernel2_HzCoeffs(handle, k2_hz_coeffs);
  sepImageFilter_setKernel2_VtCoeffs(handle, k2_vt_coeffs);
  sepImageFilter_setKernel2_NormalizationFactor(handle, norm2);

}
void sepImageFilter_setImageSize(sepimgfilter_handle *handle, unsigned int width, unsigned int height){
	handle->width=width;
	handle->height=height;
}

// Kernel 0 parameters
void sepImageFilter_setKernel0_HzCoeffs(sepimgfilter_handle *handle, int * coeffs){
	for (int i = 0; i < KERNEL_COEFFS; ++i)	handle->hz_kernel[0][i] = coeffs[i];
}
void sepImageFilter_setKernel0_VtCoeffs(sepimgfilter_handle *handle, int * coeffs){
	for (int i = 0; i < KERNEL_COEFFS; ++i)	handle->vt_kernel[0][i] = coeffs[i];
}
void sepImageFilter_setKernel0_NormalizationFactor(sepimgfilter_handle *handle, unsigned int norm){
	handle->normalization[0] = norm;
}

// Kernel 1 parameters
void sepImageFilter_setKernel1_HzCoeffs(sepimgfilter_handle *handle, int * coeffs){
	for (int i = 0; i < KERNEL_COEFFS; ++i)	handle->hz_kernel[1][i] = coeffs[i];
}
void sepImageFilter_setKernel1_VtCoeffs(sepimgfilter_handle *handle, int * coeffs){
	for (int i = 0; i < KERNEL_COEFFS; ++i)	handle->vt_kernel[1][i] = coeffs[i];
}
void sepImageFilter_setKernel1_NormalizationFactor(sepimgfilter_handle *handle, unsigned int norm){
	handle->normalization[1] = norm;
}

// Kernel 2 parameters
void sepImageFilter_setKernel2_HzCoeffs(sepimgfilter_handle *handle, int * coeffs){
	for (int i = 0; i < KERNEL_COEFFS; ++i)	handle->hz_kernel[2][i] = coeffs[i];
}
void sepImageFilter_setKernel2_VtCoeffs(sepimgfilter_handle *handle, int * coeffs){
	for (int i = 0; i < KERNEL_COEFFS; ++i)	handle->vt_kernel[2][i] = coeffs[i];
}
void sepImageFilter_setKernel2_NormalizationFactor(sepimgfilter_handle *handle, unsigned int norm){
	handle->normalization[2] = norm;
}

/*
void sepImageFilter_setVtKernelCoeffs(sepimgfilter_handle *handle, unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4){
	handle->vt_kernel_0 = c0;
	handle->vt_kernel_1 = c1;
	handle->vt_kernel_2 = c2;
	handle->vt_kernel_3 = c3;
	handle->vt_kernel_4 = c4;
}
void sepImageFilter_setNormalizationFactor(sepimgfilter_handle *handle, unsigned int norm){
	handle->normalization = norm;
}
*/


void sepImageFilter_start(sepimgfilter_handle *handle){
	// Set bits 0 (ap_start) and 7 (auto_restart)
	//sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL, 129);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL, 1);
}


int sepImageFilter_done(sepimgfilter_handle *handle){
	// (1<<1)=2 : ap_done bit
	return (sepImageFilter_get(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL) & 2)==0;
}

int sepImageFilter_running(sepimgfilter_handle *handle){
	// (1<<2)=4 : ap_idle bit
	if((sepImageFilter_get(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL) & 4)==0)
		return 1;
	return 0;
		
}

int sepImageFilter_ready(sepimgfilter_handle *handle){
	// (1<<3)=8 : ap_ready bit
	return (sepImageFilter_get(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL) & 8)==0;
}


void sepImageFilter_set(sepimgfilter_handle *handle, int num, unsigned int val){
	//printf("Setting address 0x%x + 0x%x: %d\n", handle->baseAddr, num, val);
	((volatile unsigned int *)handle->imgFilterVirtualAddress)[num>>2]=val;
}


unsigned int sepImageFilter_get(sepimgfilter_handle *handle, int num){
	if(num>=0)
		return ((volatile unsigned int *)handle->imgFilterVirtualAddress)[num>>2];

	return 0;
}
