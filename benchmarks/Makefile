#=======================================================================
# UCB VLSI FLOW: Makefile for riscv-bmarks
#-----------------------------------------------------------------------
# Yunsup Lee (yunsup@cs.berkeley.edu)
#

default: all

bmarkdir = .

instname = riscv-bmarks
instbasedir = $(UCB_VLSI_HOME)/install

#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------

bmarks = \
	dhrystone \
	median \
	qsort \
	rsort \
	towers \
	vvadd \
	multiply \
	spmv
#	mm


bmarks_host = \
	median \
	qsort \
	towers \
	vvadd \
	multiply \
	spmv 
#bmarks_host = \
	vec-vvadd \
	vec-cmplxmult \
	vec-matmul

#--------------------------------------------------------------------
# Build rules
#--------------------------------------------------------------------

HOST_OPTS = -std=gnu99 -DPREALLOCATE=0 -DHOST_DEBUG=1
HOST_COMP = gcc $(HOST_OPTS)

RISCV_PREFIX=riscv64-unknown-elf-
RISCV_GCC = $(RISCV_PREFIX)gcc
RISCV_GCC_OPTS = -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf
RISCV_LINK = $(RISCV_GCC) -T $(bmarkdir)/common/test.ld $(incs)
RISCV_LINK_MT = $(RISCV_GCC) -T $(bmarkdir)/common/test-mt.ld
RISCV_LINK_OPTS = -nostdlib -nostartfiles -ffast-math -lc -lgcc
RISCV_OBJDUMP = $(RISCV_PREFIX)objdump --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.data
RISCV_SIM = spike

VPATH += $(addprefix $(bmarkdir)/, $(bmarks))
VPATH += $(bmarkdir)/common

incs  += -I$(bmarkdir)/../env -I$(bmarkdir)/common $(addprefix -I$(bmarkdir)/, $(bmarks))
objs  := 

include $(patsubst %, $(bmarkdir)/%/bmark.mk, $(bmarks))

#------------------------------------------------------------
# Build and run benchmarks on riscv simulator

bmarks_riscv_bin  = $(addsuffix .riscv,  $(bmarks))
bmarks_riscv_dump = $(addsuffix .riscv.dump, $(bmarks))
bmarks_riscv_hex = $(addsuffix .riscv.hex, $(bmarks))
bmarks_riscv_out  = $(addsuffix .riscv.out,  $(bmarks))

bmarks_defs   = -DPREALLOCATE=0 -DHOST_DEBUG=0
bmarks_cycles = 80000

%.hex: %
	#elf2hex 16 32768 $< > $@

$(bmarks_riscv_dump): %.riscv.dump: %.riscv
	$(RISCV_OBJDUMP) $< > $@

$(bmarks_riscv_out): %.riscv.out: %.riscv
	$(RISCV_SIM) $< > $@

%.o: %.c
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(bmarks_defs) \
	             -c $(incs) $< -o $@

%.o: %.S
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(bmarks_defs) -D__ASSEMBLY__=1 \
	             -c $(incs) $< -o $@

riscv: $(bmarks_riscv_dump) $(bmarks_riscv_hex)
run-riscv: $(bmarks_riscv_out)
	echo; perl -ne 'print "  [$$1] $$ARGV \t$$2\n" if /\*{3}(.{8})\*{3}(.*)/' \
	       $(bmarks_riscv_out); echo;

junk += $(bmarks_riscv_bin) $(bmarks_riscv_dump) $(bmarks_riscv_hex) $(bmarks_riscv_out)

#------------------------------------------------------------
# Build and run benchmarks on host machine

bmarks_host_bin = $(addsuffix .host, $(bmarks_host))
bmarks_host_out = $(addsuffix .host.out, $(bmarks_host))

$(bmarks_host_out): %.host.out: %.host
	./$< > $@

host: $(bmarks_host_bin)
run-host: $(bmarks_host_out)
	echo; perl -ne 'print "  [$$1] $$ARGV \t$$2\n" if /\*{3}(.{8})\*{3}(.*)/' \
	       $(bmarks_host_out); echo;

junk += $(bmarks_host_bin) $(bmarks_host_out)

#------------------------------------------------------------
# Default

all: add_tests riscv 


#------------------------------------------------------------
# Install

date_suffix = $(shell date +%Y-%m-%d_%H-%M)
install_dir = $(instbasedir)/$(instname)-$(date_suffix)
latest_install = $(shell ls -1 -d $(instbasedir)/$(instname)* | tail -n 1)

install:
	mkdir $(install_dir)
	cp -r $(bmarks_riscv_bin) $(bmarks_riscv_dump) $(install_dir)

install-link:
	rm -rf $(instbasedir)/$(instname)
	ln -s $(latest_install) $(instbasedir)/$(instname)

#------------------------------------------------------------
# Clean up

clean:
	rm -rf $(objs) $(junk) dhrystone_mod/dhrystone_mod.riscv whetstone/whetstone.riscv linpack/linpack.riscv
	#cp common/rv64ima/crt.o .
	#cp common/rv64ima/syscalls.o .
# Original crt and syscalls (rv64g)
	cp common/crt.o .
	cp common/syscalls.o .



dhrystone_mod:
	riscv64-unknown-elf-gcc -c dhrystone_mod/dhry_1.c -o dhrystone_mod/dhry_1.o -DHZ=25000000
	riscv64-unknown-elf-gcc -c dhrystone_mod/dhry_2.c -o dhrystone_mod/dhry_2.o -DHZ=25000000
	riscv64-unknown-elf-gcc dhrystone_mod/dhry_1.o dhrystone_mod/dhry_2.o -o dhrystone_mod/dhrystone_mod.riscv

#printout=-DPRINTOUT
printout=

whetstone:
	riscv64-unknown-elf-gcc whetstone/whetstone.c -o whetstone/whetstone.riscv -lm $(printout)

prec=SP
#prec=DP
roll=ROLL
#roll=UNROLL

linpack:
	riscv64-unknown-elf-gcc -D$(prec) -D$(roll) linpack/linpack.c -o linpack/linpack.riscv


add_tests: dhrystone_mod whetstone linpack

scp_tests:
	scp $(bmarks_riscv_bin) dhrystone_mod/dhrystone_mod.riscv whetstone/whetstone.riscv linpack/linpack.riscv start_tests-g.sh start_tests-ima.sh zedboard:~

.PHONY: add_tests dhrystone_mod whetstone linpack