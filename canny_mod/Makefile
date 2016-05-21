ifeq ($(RISCV_TYPE),riscv)
	exe_type=_rv64g
else
	exe_type=_rv64ima
endif

canny_mod:
	g++ -g -I/usr/local/include -L/usr/local/lib -I/home/stefano/TESI/openCV/include convolution.cpp gaussian_coefficients.c Canny_test.cpp opencv_Canny.cpp opencv_Gblur.cpp -o canny_mod `pkg-config --cflags --libs opencv`

canny_mod_metrics:
	g++ -g -DMETRICS=1 -I/usr/local/include -L/usr/local/lib -I/home/stefano/TESI/openCV/include convolution.cpp gaussian_coefficients.c Canny_test.cpp opencv_Canny.cpp opencv_Gblur.cpp -o canny_mod `pkg-config --cflags --libs opencv` -lm

canny_mod_riscv:
	riscv64-unknown-linux-gnu-g++ -g -static -DRC=1 -I$(RISCV)/include -L$(RISCV)/lib -I/home/stefano/TESI/openCV/include convolution.cpp gaussian_coefficients.c Canny_test.cpp opencv_Canny.cpp opencv_Gblur.cpp -o canny_mod_$(exe_type) `pkg-config --cflags --libs --static opencv`

canny_mod_riscv_elf:
	riscv64-unknown-elf-g++ -g -static -DRC=1 -I$(RISCV)/include -L$(RISCV)/lib -I/home/stefano/TESI/openCV/include convolution.cpp gaussian_coefficients.c Canny_test.cpp opencv_Canny.cpp opencv_Gblur.cpp -o canny_mod_$(exe_type) `pkg-config --cflags --libs --static opencv`

quality:
	g++ -g -I/usr/local/include -L/usr/local/lib -I/home/stefano/TESI/openCV/include quality_metrics_OpenCV.cpp `pkg-config --cflags --libs opencv`

gauss:
	gcc gaussian_coefficients.c -o gaussian_coefficients -lm

gblur:
	g++ -g -I/usr/local/include -L/usr/local/lib -I/home/stefano/TESI/openCV/include opencv_Gblur.cpp -o opencv_Gblur `pkg-config --cflags --libs opencv`

valgrind: canny_mod
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./canny_mod lena_secret.bmp 50

cp_callgrind: valgrind
	cp `ls callgrind.out*` /Dropbox/poliTO/tesi/results/canny_callgrind/
	chown stefano:stefano /Dropbox/poliTO/tesi/results/canny_callgrind/*

clean:
	rm -rf callgrind.out.* canny_mod canny_mod__rv64g canny_mod__rv64ima

.PHONY: canny_mod canny_mod_metrics canny_mod_riscv canny_mod_riscv_elf valgrind gauss