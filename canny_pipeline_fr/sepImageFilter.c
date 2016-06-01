#ifdef __cplusplus
    extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"





int sepImageFilter_setup(sepimgfilter_handle *handle, int * mem_handler, unsigned int size, unsigned int baseAddr){
	handle->baseAddr=baseAddr;

	handle->imgFilterVirtualAddress = (unsigned int *)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, *mem_handler, handle->baseAddr);
	if(((unsigned int *)handle->imgFilterVirtualAddress) == MAP_FAILED){
		printf("imgFilterVirtualAddress mapping failed.\n");
		return -1;
	}

	return 0;
}


void sepImageFilter_setImageParams(sepimgfilter_handle *handle, unsigned int width, unsigned int height){
	handle->width=width;
	handle->height=height;
}


void sepImageFilter_setHzKernelCoeffs(sepimgfilter_handle *handle, unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4){
	handle->hz_kernel_0 = c0;
	handle->hz_kernel_1 = c1;
	handle->hz_kernel_2 = c2;
	handle->hz_kernel_3 = c3;
	handle->hz_kernel_4 = c4;
}


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


void sepImageFilter_start(sepimgfilter_handle *handle){
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IMG_WIDTH_V_DATA, handle->width);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IMG_HEIGHT_V_DATA, handle->height);
	
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_0_V_DATA, handle->hz_kernel_0);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_1_V_DATA, handle->hz_kernel_1);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_2_V_DATA, handle->hz_kernel_2);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_3_V_DATA, handle->hz_kernel_3);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_4_V_DATA, handle->hz_kernel_4);

	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_0_V_DATA, handle->vt_kernel_0);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_1_V_DATA, handle->vt_kernel_1);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_2_V_DATA, handle->vt_kernel_2);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_3_V_DATA, handle->vt_kernel_3);
	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_4_V_DATA, handle->vt_kernel_4);

	sepImageFilter_set(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_DIVISOR_V_DATA, handle->normalization);

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
	return (sepImageFilter_get(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL) & 4)==0;
}

int sepImageFilter_ready(sepimgfilter_handle *handle){
	// (1<<3)=8 : ap_ready bit
	return (sepImageFilter_get(handle, XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL) & 8)==0;
}


void sepImageFilter_set(sepimgfilter_handle *handle, int num, unsigned int val){
	((volatile unsigned int *)handle->imgFilterVirtualAddress)[num>>2]=val;
}


unsigned int sepImageFilter_get(sepimgfilter_handle *handle, int num){
	if(num>=0)
		return ((volatile unsigned int *)handle->imgFilterVirtualAddress)[num>>2];

	return 0;
}

#ifdef __cplusplus
    }
#endif