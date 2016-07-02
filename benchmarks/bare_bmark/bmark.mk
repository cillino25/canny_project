#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#
# Each benchmark directory should have its own fragment which
# essentially lists what the source files are and how to link them
# into an riscv and/or host executable. All variables should include
# the benchmark name as a prefix so that they are unique.
#

bmark_name=

$(bmark_name)_c_src = \
	$(bmark_name).c \
	syscalls.c \

$(bmark_name)_riscv_src = \
	crt.S \

$(bmark_name)_c_objs     = $(patsubst %.c, %.o, $($(bmark_name)_c_src))
$(bmark_name)_riscv_objs = $(patsubst %.S, %.o, $($(bmark_name)_riscv_src))

$(bmark_name)_host_bin = $(bmark_name).host
$($(bmark_name)_host_bin) : $($(bmark_name)_c_src)
	$(HOST_COMP) $^ -o $($(bmark_name)_host_bin)

$(bmark_name)_riscv_bin = $(bmark_name).riscv
$($(bmark_name)_riscv_bin) : $($(bmark_name)_c_objs) $($(bmark_name)_riscv_objs)
	$(RISCV_LINK) $($(bmark_name)_c_objs) $($(bmark_name)_riscv_objs) -o $($(bmark_name)_riscv_bin) $(RISCV_LINK_OPTS)

junk += $($(bmark_name)_c_objs) $($(bmark_name)_riscv_objs) \
        $($(bmark_name)_host_bin) $($(bmark_name)_riscv_bin)
