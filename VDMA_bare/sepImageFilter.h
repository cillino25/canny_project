#ifndef XSEPIMAGEFILTER_H_
#define XSEPIMAGEFILTER_H_


typedef struct {
	unsigned int baseAddr;
	int imgfilterHandler;
	unsigned int width;
	unsigned int height;
	unsigned char hz_kernel_0;
	unsigned char hz_kernel_1;
	unsigned char hz_kernel_2;
	unsigned char hz_kernel_3;
	unsigned char hz_kernel_4;
	unsigned char vt_kernel_0;
	unsigned char vt_kernel_1;
	unsigned char vt_kernel_2;
	unsigned char vt_kernel_3;
	unsigned char vt_kernel_4;
	unsigned int normalization;

	unsigned int * imgFilterVirtualAddress;

} sepimgfilter_handle;

void sepImageFilter_setup(sepimgfilter_handle *handle);
// An already opened /dev/mem handler (mem_handler) must be passed, since RC-Linux doesn't allow multiple memory handlers.
int sepImageFilter_setup_handle(sepimgfilter_handle *handle, int * mem_handler, unsigned int size, unsigned int baseAddr);
void sepImageFilter_setImageParams(sepimgfilter_handle *handle, unsigned int width, unsigned int height);
void sepImageFilter_setHzKernelCoeffs(sepimgfilter_handle *handle, unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);
void sepImageFilter_setVtKernelCoeffs(sepimgfilter_handle *handle, unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);
void sepImageFilter_setNormalizationFactor(sepimgfilter_handle *handle, unsigned int norm);

void sepImageFilter_start(sepimgfilter_handle *handle);

// Returns 1 if filter is running (actually, not idle)
int sepImageFilter_running(sepimgfilter_handle *handle);

void sepImageFilter_set(sepimgfilter_handle *handle, int num, unsigned int val);
unsigned int sepImageFilter_get(sepimgfilter_handle *handle, int num);



#endif
