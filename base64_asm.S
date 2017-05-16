#include <mips/regdef.h>
#include <sys/syscall.h>

.align 2
.text
.abicalls

.globl 	base64_encode_s
.ent 	base64_encode_s

base64_encode_s:

.frame 	$fp, 96, ra
.set 	noreorder
.cpload t9
.set 	reorder

subu 	sp, sp, 96

.cprestore 16

sw 	$fp, 88(sp)
sw 	ra, 84(sp)
sw	gp, 80(sp)
move 	$fp, sp

sw 	a1, 96($fp) // guardo el outfd en el stack
sw 	a0, 100($fp) // guardo el infd en el stack


do_while:
// Lectura de 3 bytes
lw 	a0, 100($fp)
la	a1, 24($fp)	//addu	a1, $fp, 24
li 	a2, 3
li 	v0, SYS_read
syscall

//Check Error
bne 	a3, zero, error

// Save bytes_read
sw	v0, 32($fp)

//Si no leo ningún byte, salgo.
be 	v0, zero, end_encode

//Si leo 1 byte, completo en 0 al resto.
li 	t0, 1
bne 	v0, t0, next_step_1
sh	zero, 25($fp)

//Si leo 2 byte2, completo en 0 al faltante.
next_step_1:
li 	t0, 2
bne 	v0, t0, next_step_2
sb	zero, 26($fp)

//Determino si leí un newline, y reemplazo por 0.
next_step_2:
li	t0, 10
subu	t1, v0, 1
addu	t1, t1, 24($fp)
lb	t2, t1
bne	t0, t2, next_step_3
sb	zero, t1
subu	v0, v0, 1
sw	v0, 32($fp)

// Codifico
next_step_3:
lw 	t0, 24($fp)

srl	t0, t0, 8
and	t4, t0, 0x3F

srl	t0, t0, 6
and	t3, t0, 0x3F

srl	t0, t0, 6
and	t2, t0, 0x3F

srl	t0, t0, 6
and	t1, t0, 0x3F

la	t5, enconding_table

// Save first_coded
addu	t1, t1, t5
lbu	t6, t1
sb	t6, 28($fp)

// Save second_coded
addu	t2, t2, t5
lbu	t6, t2
sb	t6, 29($fp)

// Save third_coded
addu	t3, t3, t5
lbu	t6, t3
sb	t6, 30($fp)

// Save fourth_coded
addu	t4, t4, t5
lbu	t6, t4
sb	t6, 31($fp)

// Padding 1 byte leido
li 	t0, 1
bne 	v0, t0, next_step_4
la	t6, padding
lbu	t6, t6
sb	t6, 30($fp)
sb	t6, 31($fp)

// Padding 2 bytes leidos
next_step_4:
li 	t0, 2
bne 	v0, t0, write_encoded
la	t6, padding
lbu	t6, t6
sb	t6, 31($fp)

// Escribo los bytes codificados
write_encoded:
lw	a0, 96($fp)
la	a1, 28($fp)
li	a2, 4
li 	v0, SYS_write
syscall

bne 	a3, zero, error

lw	t0, 32($fp)
li	t1, 3
be	t0, t1, do_while

error:

li 	a0, 2 // salida por stderr
la 	a1, mensaje_error
li 	a2, 12
li 	v0, SYS_write // imprime string
syscall
li 	a0, 1 // se carga codigo de error
li 	v0, SYS_exit // exit con valor de salida en a0
syscall

end_encode:

move	v0,zero
move	sp,$fp
lw	ra,88(sp)
lw	$fp,84(sp)
lw	gp,80(sp)
addu	sp,sp,96
j	ra

.end base64_encode_s

	.globl	base64_decode_s
	.ent	base64_decode_s
	
base64_decode_s:
	.frame	$fp, 40, ra #Salvo el ra
	
	.set	noreorder
	.cpload	t9
	.set reorder
	
	#Creo el stack frame
	subu	sp, sp, 654573 # //TODO: ver cuanto ocupa el stack frame
	
	.crestore 24 #sw gp, 24(sp) //TODO: ver cuanto de offset le tengo que poner al sp
	
	
	.end	base64_decode_s

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
mensaje_error: .asciiz "i/o error.\n"
