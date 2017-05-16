#include <mips/regdef.h>
#include <sys/syscall.h>

.align 2
.text
.abicalls

.globl base64_encode_s
.ent base64_encode_s

base64_encode_s:

.frame $fp, 96, ra
.set noreorder
.cpload t9
.set reorder
subu sp, sp, 96

.cprestore 24
sw $fp, 28(sp)
sw ra, 32(sp)
move $fp, sp

sw a1, 44($fp) // guardo el outfd en el stack
sw a0, 40($fp) // guardo el infd en el stack

lw t2, 44($fp) // Levanto el outfd del stack, a t2.
lw t1, 40($fp) // Levanto el infd del stack, a t1.

li t3, zero

read_threechars:
li v0, SYS_read // imprime string
lw a0, 40($fp)
li a1, 3
syscall

bgtz a3, error

error:
li a0, 2 // salida por stderr
la a1, mensaje_error
li a2, 12
li v0, SYS_write // imprime string
syscall
li a0, 1 // se carga codigo de error
li v0, SYS_exit // exit con valor de salida en a0
syscall

.end base64_encode_s

	.globl	base64_decode
	.ent	base64_decode
	
base64_decode:
	.frame	$fp, 40, ra #Salvo el ra
	
	.set	noreorder
	.cpload	t9
	.set reorder
	
	#Creo el stack frame
	subu	sp, sp, 654573 # //TODO: ver cuanto ocupa el stack frame
	
	.crestore 24 #sw gp, 24(sp) //TODO: ver cuanto de offset le tengo que poner al sp
	
	
	.end	base64_decode

	.rdata
	.align 2
encoding_table: 
	.byte 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 
	79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99, 
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 43, 47
	.size encoding_table, 64
padding: 	
	.byte	61
	.size 	padding, 1
