#ifndef GLOBAL_PARAMETERS_H_
#define GLOBAL_PARAMETERS_H_


#define X86_PRESC 1
#define RC_PRESC  1000/25
#ifdef RC
  #define PRESC   RC_PRESC
#else
  #define PRESC   X86_PRESC
#endif 

#define C1 (float) (0.01 * 255 * 0.01  * 255)
#define C2 (float) (0.03 * 255 * 0.03  * 255)


#define BMP_INFO_SIZE           54
#define BMP_WIDTH_OFFSET        18
#define BMP_HEIGHT_OFFSET       22


#define ROCKETCHIP_HTIF         0x43c00000

#define AXI_DMA_BASEADDR        0x40400000
#define AXI_DMA_HIGHADDR        0x4040FFFF


#ifndef RC
 #define AXI_VDMA_BASEADDR           0x45000000
 #define AXI_VDMA_HIGHADDR           0x4500FFFF
 #define AXI_PULSER                  0x46000000
 #define AXI_SEPIMGFILTER            0x47000000
 #define AXI_POLLING_VARIABLE_ADDR   0x47100000
#else
 #define AXI_VDMA_BASEADDR           0x25000000
 #define AXI_VDMA_HIGHADDR           0x2500FFFF
 #define AXI_PULSER                  0x26000000
 #define AXI_SEPIMGFILTER            0x27000000
 #define AXI_POLLING_VARIABLE_ADDR   0x27100000
#endif

#define AXI_GPIO_BASEADDR        0x44000000
#define AXI_GPIO_HIGHADDR        0x4400FFFF



//#define MEM_POLLING_VARIABLE_ADDR            0x1FA00000
#define MEM_POLLING_VARIABLE_ADDR            0x1FF00000

/* Memory buffer starting address (RAM) */
//#define BUFFER_MEM_READ_START_ADDRESS        0x1F400000
//#define BUFFER_MEM_WRITE_START_ADDRESS       0x1F700000
#define BUFFER_MEM_READ_START_ADDRESS        0x1E400000
#define BUFFER_MEM_WRITE_START_ADDRESS       0x1E700000


/* Buffer size = 1M */
#define BUFFER_SIZE				                   0x00100000



#define MEM2VDMA_BUFFER1_BASEADDR		(BUFFER_MEM_READ_START_ADDRESS)
#define MEM2VDMA_BUFFER2_BASEADDR		(MEM2VDMA_BUFFER1_BASEADDR + BUFFER_SIZE)
#define MEM2VDMA_BUFFER3_BASEADDR		(MEM2VDMA_BUFFER2_BASEADDR + BUFFER_SIZE)

//#define MEM2VDMA_BUFFER1_BASEADDR		(BUFFER_MEM_READ_START_ADDRESS)
//#define MEM2VDMA_BUFFER2_BASEADDR		(MEM2VDMA_BUFFER1_BASEADDR + BUFFER_SIZE)
//#define MEM2VDMA_BUFFER3_BASEADDR		(MEM2VDMA_BUFFER2_BASEADDR + BUFFER_SIZE)

#define VDMA2MEM_BUFFER1_BASEADDR		(BUFFER_MEM_WRITE_START_ADDRESS)
#define VDMA2MEM_BUFFER2_BASEADDR		(VDMA2MEM_BUFFER1_BASEADDR + BUFFER_SIZE)
#define VDMA2MEM_BUFFER3_BASEADDR		(VDMA2MEM_BUFFER2_BASEADDR + BUFFER_SIZE)



/* Parameter definitions for image capture */
#define IN_FRAME_WIDTH		640		// for use with the camera
#define IN_FRAME_HEIGHT		480		// for use with the camera
#define PIXEL_CHANNELS      1




#endif
// endif GLOBAL_PARAMETERS_H_
