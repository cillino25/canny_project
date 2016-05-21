#!/bin/bash



## Threshold value is related with nC!

# span threshold range
# nG=3, nC=3
# for I in {10..100..10}

# nG=3, nC=5
# for I in {200..2000..100}

# nG=5, nC=3
# for I in {10..100..10}


# nG=5, nC=5
# for I in {200..2000..100}

# nG=7, nC=5
# for I in {100..1000..100}
# for J in {50..200..10}

# nG=5, nC=7
# for I in {1000..14000..1000}
# for J in {50..200..10}

# nG=7, nC=7
# for I in {1000..14000..1000}
# for J in {50..200..10}

# nG=3, nC=7
# for I in {1000..14000..1000}
# for J in {50..200..10}

# nG=7, nC=3
# for I in {10..100..10}
# for J in {50..200..10}


NG=7
NC=3

for I in {10..100..10}
do
	# span sigma values
	for J in {50..200..10}
	do
		SIG=$(bc <<< "scale=2; $J / 100" )
		for K in {0..3..1}
		do
			FILE="lena_nG${NG}_nC${NC}/lena_t${I}_s${SIG}_nG${NG}_nC${NC}_t${K}.bmp"
			CMD=`./canny_mod lena_secret.bmp $I $SIG $NG $NC $K $FILE`
			echo "$CMD"
			 
		done
	done
done