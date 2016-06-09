#ifndef XSEPIMAGEFILTER_PARAMETERS_
#define XSEPIMAGEFILTER_PARAMETERS_

// CONTROL_BUS
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x10 : Data signal of IMG_WIDTH_V
//        bit 10~0 - IMG_WIDTH_V[10:0] (Read/Write)
//        others   - reserved
// 0x14 : reserved
// 0x18 : Data signal of IMG_HEIGHT_V
//        bit 10~0 - IMG_HEIGHT_V[10:0] (Read/Write)
//        others   - reserved
// 0x1c : reserved
// 0x20 : Data signal of hz_kernel_0_V
//        bit 7~0 - hz_kernel_0_V[7:0] (Read/Write)
//        others  - reserved
// 0x24 : reserved
// 0x28 : Data signal of hz_kernel_1_V
//        bit 7~0 - hz_kernel_1_V[7:0] (Read/Write)
//        others  - reserved
// 0x2c : reserved
// 0x30 : Data signal of hz_kernel_2_V
//        bit 7~0 - hz_kernel_2_V[7:0] (Read/Write)
//        others  - reserved
// 0x34 : reserved
// 0x38 : Data signal of hz_kernel_3_V
//        bit 7~0 - hz_kernel_3_V[7:0] (Read/Write)
//        others  - reserved
// 0x3c : reserved
// 0x40 : Data signal of hz_kernel_4_V
//        bit 7~0 - hz_kernel_4_V[7:0] (Read/Write)
//        others  - reserved
// 0x44 : reserved
// 0x48 : Data signal of vt_kernel_0_V
//        bit 7~0 - vt_kernel_0_V[7:0] (Read/Write)
//        others  - reserved
// 0x4c : reserved
// 0x50 : Data signal of vt_kernel_1_V
//        bit 7~0 - vt_kernel_1_V[7:0] (Read/Write)
//        others  - reserved
// 0x54 : reserved
// 0x58 : Data signal of vt_kernel_2_V
//        bit 7~0 - vt_kernel_2_V[7:0] (Read/Write)
//        others  - reserved
// 0x5c : reserved
// 0x60 : Data signal of vt_kernel_3_V
//        bit 7~0 - vt_kernel_3_V[7:0] (Read/Write)
//        others  - reserved
// 0x64 : reserved
// 0x68 : Data signal of vt_kernel_4_V
//        bit 7~0 - vt_kernel_4_V[7:0] (Read/Write)
//        others  - reserved
// 0x6c : reserved
// 0x70 : Data signal of divisor_V
//        bit 9~0 - divisor_V[9:0] (Read/Write)
//        others  - reserved
// 0x74 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_AP_CTRL            0x00
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_GIE                0x04
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IER                0x08
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_ISR                0x0c
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IMG_WIDTH_V_DATA   0x10
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_IMG_WIDTH_V_DATA   11
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_IMG_HEIGHT_V_DATA  0x18
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_IMG_HEIGHT_V_DATA  11
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_0_V_DATA 0x20
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_HZ_KERNEL_0_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_1_V_DATA 0x28
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_HZ_KERNEL_1_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_2_V_DATA 0x30
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_HZ_KERNEL_2_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_3_V_DATA 0x38
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_HZ_KERNEL_3_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_HZ_KERNEL_4_V_DATA 0x40
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_HZ_KERNEL_4_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_0_V_DATA 0x48
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_VT_KERNEL_0_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_1_V_DATA 0x50
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_VT_KERNEL_1_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_2_V_DATA 0x58
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_VT_KERNEL_2_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_3_V_DATA 0x60
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_VT_KERNEL_3_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_VT_KERNEL_4_V_DATA 0x68
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_VT_KERNEL_4_V_DATA 8
#define XSEPIMAGEFILTER_CONTROL_BUS_ADDR_DIVISOR_V_DATA     0x70
#define XSEPIMAGEFILTER_CONTROL_BUS_BITS_DIVISOR_V_DATA     10


#endif
