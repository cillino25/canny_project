#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "global_parameters.h"
#include "vdma_parameters.h"
#include "vdma.h"
#include "sepImageFilter.h"
#include "sepImageFilter_parameters.h"

unsigned char* readBMP(char* filename, unsigned char ** info, unsigned char ** data, int *size, int *width, int *height);
int writeBMP(char* filename, unsigned char * info, unsigned char * data, int size);

int main() {
  int h, j, i;
  vdma_handle handle;
  sepimgfilter_handle filter_handle;

  int frame_len = PIXEL_CHANNELS * IN_FRAME_WIDTH * IN_FRAME_HEIGHT;
  int mem;
  void *read_fb, *write_fb;
  unsigned int page_size = sysconf(_SC_PAGESIZE);

  unsigned char hz_coeffs[] = {1, 4, 7, 4, 1};
  unsigned char vt_coeffs[] = {1, 4, 7, 4, 1};
  unsigned int norm = 289;

  char in_file[]="lena_gray.bmp";
  char out_file[]="lena_blurred.bmp";
  int size=0, width=0, height=0;
  unsigned char *img_data;
  unsigned char *img_info;
  readBMP(in_file, &img_info, &img_data, &size, &width, &height);
  writeBMP(out_file, img_info, img_data, size);

  if(img_data==NULL){
    printf("Error opening file %s.\nExiting..\n", in_file);
    return 1;
  }
  if(img_info==NULL){
    printf("Error opening file %s.\nExiting..\n", in_file);
    return 1;
  }

  printf("%s opened: w=%d, h=%d, tot pixels=%d\n", in_file, width, height, size);
  /*printf("Image INFO (as unsigned char values):\n");
  for(i=0; i<BMP_INFO_SIZE; i++){
    printf("Info[%d]=%u\n", i, ((unsigned char *)img_info)[i]);
  }
  printf("\n\n");
  */

  vdma_setup(&handle, page_size, AXI_VDMA_BASEADDR, IN_FRAME_WIDTH, IN_FRAME_HEIGHT, PIXEL_CHANNELS, BUFFER_SIZE, MEM2VDMA_BUFFER1_BASEADDR, MEM2VDMA_BUFFER2_BASEADDR, MEM2VDMA_BUFFER3_BASEADDR, VDMA2MEM_BUFFER1_BASEADDR, VDMA2MEM_BUFFER2_BASEADDR, VDMA2MEM_BUFFER3_BASEADDR, AXI_PULSER);
  
  sepImageFilter_setup(&filter_handle, &handle.vdmaHandler, page_size, AXI_SEPIMGFILTER);
  sepImageFilter_setImageParams(&filter_handle, width, height);
  sepImageFilter_setHzKernelCoeffs(&filter_handle, hz_coeffs[0], hz_coeffs[1], hz_coeffs[2], hz_coeffs[3], hz_coeffs[4]);
  sepImageFilter_setVtKernelCoeffs(&filter_handle, vt_coeffs[0], vt_coeffs[1], vt_coeffs[2], vt_coeffs[3], vt_coeffs[4]);
  sepImageFilter_setNormalizationFactor(&filter_handle, norm);

  printf("Setting up done.\n");
  

  write_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, handle.vdmaHandler, MEM2VDMA_BUFFER1_BASEADDR);
  if(((unsigned int *)write_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  read_fb = (unsigned int*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, handle.vdmaHandler, VDMA2MEM_BUFFER1_BASEADDR);
  if(((unsigned int *)read_fb) == MAP_FAILED) {
    printf("vdmaVirtualAddress mapping for absolute memory access failed.\n");
    return -1;
  }

  vdma_start_triple_buffering_mod(&handle);
  sepImageFilter_start(&filter_handle);
  printf("VDMA and filter started.\n");

  // Read the image
  

  printf("Image read.\n");
  
  // Copy image to mm2s buffer
  printf("memcpy..\n");
  memcpy(write_fb, (void*)img_data, size);            // ?????
  //for(i=0; i<size/4; i++) ((unsigned int*)write_fb)[i]=((unsigned int *)img_data)[i];

  
  printf("Debug: write buffer content:\n");
  //for(i=0; i<size; i++)
  	//printf("-d: img[%d][%d] = %d\n", i/width, i%width, ((volatile unsigned char *)write_fb)[i]);
  printf("write buffer printed.\n");


  // send fsync to VDMA  
  printf("Sending fsync...\n");
  vdma_send_fsync(&handle);

  // wait for filter to finish
  printf("Waiting the filter to finish..\n");
  //while(sepImageFilter_running(&filter_handle) == 1);

  // read s2mm buffer
 	printf("Debug: read buffer content:\n");
  //for(i=0; i<size; i++)
  	//printf("-d: img[%d][%d] = %d\n", i/width, i%width, ((volatile unsigned char *)read_fb)[i]);
  
  printf("read buffer printed.\n");

  //writeBMP(out_file, img_info, read_fb, size);
  

  // Halt VDMA and unmap memory ranges
  vdma_halt(&handle);

  munmap(read_fb, BUFFER_SIZE);
  munmap(write_fb, BUFFER_SIZE);
  printf("Bye!\n");

  return 0;
}

unsigned char* readBMP(char* filename, unsigned char ** info, unsigned char ** data, int *size, int *width, int *height)
{
  int i;
  FILE* f = fopen(filename, "rb");
  if(f==NULL){
    return NULL;
  }
  unsigned char *img_info = new unsigned char[BMP_INFO_SIZE];
  fread(img_info, sizeof(unsigned char), BMP_INFO_SIZE, f); // read the 54-byte header

  // extract image height and width from header
  int w = img_info[BMP_WIDTH_OFFSET] + ((int)img_info[BMP_WIDTH_OFFSET+1]<<8);
  int h = img_info[BMP_HEIGHT_OFFSET] + ((int)img_info[BMP_HEIGHT_OFFSET+1]<<8);
  *width=w;
  *height=h;
  // RGB
  //int size = 3 * width * height;
  
  // Grayscale
  int s = w * h;
  *size=s;

  unsigned char* img_data = new unsigned char[s]; // allocate 3 bytes per pixel
  fread(img_data, sizeof(unsigned char), s, f); // read the rest of the data at once
  fclose(f);

  /*
  // Swap BGR to RGB. Not needed since we have grayscale image.
  for(i = 0; i < size; i += 3)
  {
    unsigned char tmp = data[i];
    data[i] = data[i+2];
    data[i+2] = tmp;
  }
  */

  *data=img_data;
  *info=img_info;


  return img_data;
}

int writeBMP(char* filename, unsigned char * info, unsigned char * data, int size){
  //int i;
  FILE* f = fopen(filename, "w");
  if(f==NULL){
    return 1;
  }
  fwrite(info, sizeof(char), (size_t)BMP_INFO_SIZE, f);
  fwrite(data, sizeof(char), (size_t)size, f);
  fclose(f);

  return 0;
}