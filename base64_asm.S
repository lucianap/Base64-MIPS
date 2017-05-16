#include <mips/regdef.h>
#include <sys/syscall.h>

.align 2
.text

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

.globl base64_decode_s
.ent base64_decode_s

base64_decode_s:


.end base64_decode_s

.rdata
