
#include "openCV_HW_filter.h"




void GaussianBlur_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out){
	// Set VDMA fb in and out
	vdma_setParams(vdma_handle, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR);
	vdma_setFbInOut(vdma_handle, fb_in, fb_out);

	vdma_start_triple_buffering_mod(vdma_handle);
	vdma_reset_fsync(vdma_handle);
	// Set sepImageFilter config register
	sepImageFilter_config(filter_handle, 1, 0, 0);

	// Send sepImageFilter start
	sepImageFilter_start(filter_handle);

	// Send fsync 
	vdma_send_fsync(vdma_handle);

	// Wait for VDMA complete memory write
	// while(vdma_mm2s_running(vdma_handle));
	while(sepImageFilter_running(filter_handle) != 0);

}

void SobelDx_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out){
	// Set VDMA fb in and out
	vdma_setParams(vdma_handle, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR);
	vdma_setFbInOut(vdma_handle, fb_in, fb_out);
	
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

void SobelDy_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out){
	// Set VDMA fb in and out
	vdma_setParams(vdma_handle, AXI_VDMA_BASEADDR, width, height, 2*PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR);
	vdma_setFbInOut(vdma_handle, fb_in, fb_out);

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