#!/bin/sh

echo "################################ Dhrystone_mod #####################################"
echo "./fesvr-zynq pk dhrystone_mod.riscv"
./fesvr-zynq pk dhrystone_mod.riscv

echo "################################   Whetstone   #####################################"
echo "./fesvr-zynq pk whetstone.riscv"
./fesvr-zynq pk whetstone.riscv

echo "################################   Linpack   #####################################"
echo "./fesvr-zynq pk linpack.riscv"
./fesvr-zynq pk linpack.riscv

echo "################################ RISCV: Dhrystone #####################################"
echo "./fesvr-zynq dhrystone.riscv"
./fesvr-zynq dhrystone.riscv

echo "################################ RISCV: Median #####################################"
echo "./fesvr-zynq median.riscv"
./fesvr-zynq median.riscv

echo "################################ RISCV: qsort #####################################"
echo "./fesvr-zynq qsort.riscv"
./fesvr-zynq qsort.riscv

echo "################################ RISCV: rsort #####################################"
echo "./fesvr-zynq rsort.riscv"
./fesvr-zynq rsort.riscv

echo "################################ RISCV: towers #####################################"
echo "./fesvr-zynq towers.riscv"
./fesvr-zynq towers.riscv

echo "################################ RISCV: vvadd #####################################"
echo "./fesvr-zynq vvadd.riscv"
./fesvr-zynq vvadd.riscv

echo "################################ RISCV: multiply #####################################"
echo "./fesvr-zynq multiply.riscv"
./fesvr-zynq multiply.riscv

echo "################################ RISCV: spmv #####################################"
echo "./fesvr-zynq spmv.riscv"
./fesvr-zynq spmv.riscv
