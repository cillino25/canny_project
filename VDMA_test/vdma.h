#ifndef VDMA_H_
#define VDMA_H_




typedef struct {
    unsigned int baseAddr;
    int vdmaHandler;
    int width;
    int height;
    int pixelChannels;
    int fbLength;
    unsigned int* vdmaVirtualAddress;
    unsigned int* fb1VirtualAddress;
    long fb1PhysicalAddress;
    unsigned int* fb2VirtualAddress;
    long fb2PhysicalAddress;
    unsigned int* fb3VirtualAddress;
    long fb3PhysicalAddress;
    
} vdma_handle;




int vdma_setup(vdma_handle *handle, unsigned int page_size, unsigned int baseAddr, int width, int height, int pixelChannels, int max_buffer_size, long fb1Addr, long fb2Addr, long fb3Addr);
void vdma_halt(vdma_handle *handle);
unsigned int vdma_get(vdma_handle *handle, int num);
void vdma_set(vdma_handle *handle, int num, unsigned int val);
void vdma_status_dump(int status);
void vdma_s2mm_status_dump(vdma_handle *handle);
void vdma_mm2s_status_dump(vdma_handle *handle);
void vdma_start_triple_buffering(vdma_handle *handle);
int vdma_s2mm_running(vdma_handle *handle);
int vdma_s2mm_idle(vdma_handle *handle);
int vdma_mm2s_running(vdma_handle *handle);
int vdma_mm2s_idle(vdma_handle *handle);

void vdma_fill_buffer(vdma_handle *handle, unsigned int *fbAddr, unsigned int val);
void vdma_cmp_buffer(vdma_handle *handle, unsigned int *fbAddr, unsigned int val);


#endif
