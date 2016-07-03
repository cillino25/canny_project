canny_video-arm-only:
	g++ canny_video__arm-only.cpp sepImageFilter.c vdma.c gaussian_coefficients.c openCV_HW_filter.cpp -o canny_video__arm-only `pkg-config --cflags --libs opencv`

canny_video-arm:
	g++ canny_video-arm.cpp sepImageFilter.c vdma.c gaussian_coefficients.c openCV_HW_filter.cpp -o canny_video__arm-only `pkg-config --cflags --libs opencv`

.PHONY: canny_video-arm-only canny_video-arm
