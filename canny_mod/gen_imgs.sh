#!/bin/bash

T=50
NG=$1
NC=3
SIG=$2

for K in {0..3..1}
do
	CMD=`./canny_mod lena_secret.bmp $T $SIG $NG $NC $K`
	echo "$CMD"
	 
done
