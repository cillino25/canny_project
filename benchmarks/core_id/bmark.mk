#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#
# Each benchmark directory should have its own fragment which
# essentially lists what the source files are and how to link them
# into an riscv and/or host executable. All variables should include
# the benchmark name as a prefix so that they are unique.
#

PROG_NAME=core_id

$(PROG_NAME)_c_src = \
	$(PROG_NAME).c \
	syscalls.c \
	

$(PROG_NAME)_riscv_src = \
	crt.S \

$(PROG_NAME)_c_objs     = $(patsubst %.c, %.o, $($(PROG_NAME)_c_src))
$(PROG_NAME)_riscv_objs = $(patsubst %.S, %.o, $($(PROG_NAME)_riscv_src))


$(PROG_NAME)_riscv_bin = $(PROG_NAME).riscv
$($(PROG_NAME)_riscv_bin): $($(PROG_NAME)_c_objs) $($(PROG_NAME)_riscv_objs)
	$(RISCV_LINK) $($(PROG_NAME)_c_objs) $($(PROG_NAME)_riscv_objs) \
    -o $($(PROG_NAME)_riscv_bin) $(RISCV_LINK_OPTS)

junk += $($(PROG_NAME)_c_objs) $($(PROG_NAME)_riscv_objs) \
        $($(PROG_NAME)_riscv_bin)
