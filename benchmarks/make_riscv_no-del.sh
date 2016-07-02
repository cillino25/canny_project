#!/bin/bash

# The following line creates a sub-shell, which will be terminated when EOF is encountered (at the end of the script);
# for this reason, the script executes all the commands in a subshell and then returns to the calling one,
# again as root user (without this trick, when the script exits also the shell will exit, going back to normal user)
#bash  << EOF
#source /root/riscv_exports.sh

#set -e

mkdir $1.objs

DIR=`pwd`
EMUCONFIG=DefaultCPPSmallConfig
GCC_OPTS=-static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf
#echo $DIR

cd $1.objs

riscv32-unknown-elf-gcc $GCC_OPTS -c $TOP/common/crt.S -o crt.o
riscv32-unknown-elf-gcc $GCC_OPTS -c $TOP/common/syscalls.c -o syscalls.o
riscv32-unknown-elf-gcc $GCC_OPTS -c $TOP/programming_riscv/$1.c -o $1.o -save-temps
riscv32-unknown-elf-gcc $GCC_OPTS -T $TOP/common/test.ld $1.o crt.o syscalls.o -o $1.riscv -nostdlib -lc


cd $DIR
