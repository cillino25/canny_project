#!/bin/bash

# The following line creates a sub-shell, which will be terminated when EOF is encountered (at the end of the script);
# for this reason, the script executes all the commands in a subshell and then returns to the calling one,
# again as root user (without this trick, when the script exits also the shell will exit, going back to normal user)
#bash  << EOF
#source /root/riscv_exports.sh

#set -e

mkdir $1.objs

DIR=`pwd`
EMUCONFIG=DefaultCPPSmallConfig2Cores
#echo $DIR

cd $1.objs
cp common/crt.o .
cp common/syscalls.o .

riscv64-unknown-elf-gcc -c $TOP/programming_riscv/$1.c -o $1.o -save-temps
riscv64-unknown-elf-gcc -T $TOP/common/test.ld -I../common $1.o crt.o syscalls.o -o $1.riscv -nostdlib -nostartfiles -lc

rm crt.o
rm syscalls.o

elf2hex 16 32768 $1.riscv > $1.riscv.hex

cd $ROCKET/emulator

# Only emulation
 ./emulator-Top-$EMUCONFIG +dramsim +max-cycles=100000000 +verbose +loadmem=$DIR/$1.objs/$1.riscv.hex none 3>&1 1>&2 2>&3 | $RISCV/bin/spike-dasm  > $DIR/$1.objs/$1.riscv.out && [ $PIPESTATUS -eq 0 ]

# Create VCD waveforms (to be open with gtkwave)
#./emulator-Top-DefaultCPPConfig-debug +dramsim +max-cycles=100000000 +verbose -v$DIR/$1.objs/$1.riscv.vcd +loadmem=$DIR/$1.objs/$1.riscv.hex none 3>&1 1>&2 2>&3 | $RISCV/bin/spike-dasm  > $DIR/$1.objs/$1.riscv.out && [ $PIPESTATUS -eq 0 ]


cd $DIR
