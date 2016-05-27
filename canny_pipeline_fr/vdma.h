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
    unsigned int* fb1VirtualAddress_mm2s;
    long fb1PhysicalAddress_mm2s;
    unsigned int* fb2VirtualAddress_mm2s;
    long fb2PhysicalAddress_mm2s;
    unsigned int* fb3VirtualAddress_mm2s;
    long fb3PhysicalAddress_mm2s;
    unsigned int* fb1VirtualAddress_s2mm;
    long fb1PhysicalAddress_s2mm;
    unsigned int* fb2VirtualAddress_s2mm;
    long fb2PhysicalAddress_s2mm;
    unsigned int* fb3VirtualAddress_s2mm;
    long fb3PhysicalAddress_s2mm;
} vdma_handle;




int vdma_setup(vdma_handle *handle, unsigned int page_size, unsigned int baseAddr, int width, int height, int pixelChannels, int max_buffer_size, long fb1Addr_mm2s, long fb2Addr_mm2s, long fb3Addr_mm2s, long fb1Addr_s2mm, long fb2Addr_s2mm, long fb3Addr_s2mm);
void vdma_halt(vdma_handle *handle);
unsigned int vdma_get(vdma_handle *handle, int num);
void vdma_set(vdma_handle *handle, int num, unsigned int val);
void vdma_status_dump(int status);
void vdma_s2mm_status_dump(vdma_handle *handle);
void vdma_mm2s_status_dump(vdma_handle *handle);
void vdma_control_dump(int control);
void vdma_s2mm_control_dump(vdma_handle *handle);
void vdma_mm2s_control_dump(vdma_handle *handle);
void vdma_start_triple_buffering_mod(vdma_handle *handle);
void vdma_start_triple_buffering(vdma_handle *handle);
int vdma_s2mm_running(vdma_handle *handle);
int vdma_s2mm_idle(vdma_handle *handle);
int vdma_mm2s_running(vdma_handle *handle);
int vdma_mm2s_idle(vdma_handle *handle);

void print_vdma_stats(vdma_handle *handle);

void fill_buffer(unsigned int * fbAddr, int length, unsigned int val);
int cmp_buffer(unsigned int * fbAddr, int length, unsigned int val);


#endif
