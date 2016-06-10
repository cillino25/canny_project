

#include "global_parameters.h"

#include "vdma.h"
#include "vdma_parameters.h"
#include "sepImageFilter.h"


void GaussianBlur_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out);

void SobelDx_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out);

void SobelDy_HW(vdma_handle *vdma_handle, sepimgfilter_handle *filter_handle, unsigned int width, unsigned int height, unsigned int fb_in, unsigned int fb_out);
