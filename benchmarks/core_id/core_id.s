	.file	"core_id.c"
	.section	.rodata
	.align	3
.LC0:
	.string	"Core id = "
	.align	3
.LC1:
	.string	"%x - "
	.align	3
.LC2:
	.string	"%x\n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	add	sp,sp,-16
	sd	ra,8(sp)
	sd	s0,0(sp)
	add	s0,sp,16
	lui	a5,%hi(.LC0)
	add	a0,a5,%lo(.LC0)
	call	puts
	mv	a5,s2
	mv	a1,a5
	lui	a5,%hi(.LC1)
	add	a0,a5,%lo(.LC1)
	call	printf
	li	s2,0
	mv	a5,s2
	mv	a1,a5
	lui	a5,%hi(.LC2)
	add	a0,a5,%lo(.LC2)
	call	printf
	li	a5,0
	mv	a0,a5
	ld	ra,8(sp)
	ld	s0,0(sp)
	add	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 5.3.0"
