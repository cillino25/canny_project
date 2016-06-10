#ifndef XSEPIMAGEFILTER_H_
#define XSEPIMAGEFILTER_H_


#define KERNEL_COEFFS 7
#define KERNEL_NUM    3


typedef struct {
	unsigned int baseAddr;
	int imgfilterHandler;
	unsigned int width;
	unsigned int height;
	unsigned char hz_kernel[KERNEL_NUM][KERNEL_COEFFS];
	unsigned char vt_kernel[KERNEL_NUM][KERNEL_COEFFS];
	unsigned int  normalization[KERNEL_NUM];

	unsigned int * imgFilterVirtualAddress;

} sepimgfilter_handle;

void sepImageFilter_setupFilter(sepimgfilter_handle *handle);
// An already opened /dev/mem handler (mem_handler) must be passed, since RC-Linux doesn't allow multiple memory handlers.
int sepImageFilter_setupHandle(sepimgfilter_handle *handle, int * mem_handler, unsigned int size, unsigned int baseAddr);


/* Configure sepImageFilter:
 * -bit 7    : Normalize computation (see normalization parameter)
 * -bit 6    : Truncate to 0 if negative
 * -bit[5:0] : Kernel selection
 */
void sepImageFilter_config(sepimgfilter_handle *handle, int normalize, int truncate, int kernel_num);

void sepImageFilter_setupHandleParams(sepimgfilter_handle *handle, unsigned int width, unsigned int height, unsigned char * k0_hz_coeffs, unsigned char * k0_vt_coeffs, unsigned int norm0, unsigned char * k1_hz_coeffs, unsigned char * k1_vt_coeffs, unsigned int norm1, unsigned char * k2_hz_coeffs, unsigned char * k2_vt_coeffs, unsigned int norm2);

void sepImageFilter_setImageSize(sepimgfilter_handle *handle, unsigned int width, unsigned int height);
// Kernel 0 parameters
void sepImageFilter_setKernel0_HzCoeffs(sepimgfilter_handle *handle, unsigned char * coeffs);
void sepImageFilter_setKernel0_VtCoeffs(sepimgfilter_handle *handle, unsigned char * coeffs);
void sepImageFilter_setKernel0_NormalizationFactor(sepimgfilter_handle *handle, unsigned int norm);
// Kernel 1 parameters
void sepImageFilter_setKernel1_HzCoeffs(sepimgfilter_handle *handle, unsigned char * coeffs);
void sepImageFilter_setKernel1_VtCoeffs(sepimgfilter_handle *handle, unsigned char * coeffs);
void sepImageFilter_setKernel1_NormalizationFactor(sepimgfilter_handle *handle, unsigned int norm);
// Kernel 2 parameters
void sepImageFilter_setKernel2_HzCoeffs(sepimgfilter_handle *handle, unsigned char * coeffs);
void sepImageFilter_setKernel2_VtCoeffs(sepimgfilter_handle *handle, unsigned char * coeffs);
void sepImageFilter_setKernel2_NormalizationFactor(sepimgfilter_handle *handle, unsigned int norm);



void sepImageFilter_start(sepimgfilter_handle *handle);

// Returns 1 if filter is running (actually, not idle)
int sepImageFilter_running(sepimgfilter_handle *handle);

void sepImageFilter_set(sepimgfilter_handle *handle, int num, unsigned int val);
unsigned int sepImageFilter_get(sepimgfilter_handle *handle, int num);



#endif
