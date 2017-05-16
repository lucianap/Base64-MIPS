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

	.globl	base64_decode_s
	.ent	base64_decode_s
	.align 2
	
base64_decode_s:
	.frame	$fp, 40, ra #Salvo el ra
	
	.set	noreorder
	.cpload	t9
	.set reorder
	
	#Creo el stack frame
	subu	sp, sp, 654573 # //TODO: ver cuanto ocupa el stack frame
	
	.crestore 24 # //TODO: ver cuanto de offset le tengo que poner al sp
	# salvado de callee-saved regs en SRA	# //TODO: ver, de nuevo, los offsets de aca
	sw	$fp, 28(sp)
	sw	ra, 32(sp)
	
	# de aqui al fin de la funcion uso $fp en lugar de sp.
	move	$fp, sp
	
	# Salvo los argumentos
	sw 	a0, 40($fp)
	sw 	a1, 44($fp)
	
	
	li	v0, SYS_read	# ver dentro de <sys/syscall.h>.
	lw	a0, 40($fp)		# a0: file descriptor (redundante)
	la	a1, 8(fp)		# a1: data pointer.
	lw	a2, 4			# a2: longitud, leo de a 4 bytes.
	syscall
	
	bne	a3, zero, error # Chequeo si la syscall se ejecuto bien
	
	be	v0, zero, end_decode 		# Si no lei nada, termino
	li	t0, 4						#t0 = 4
	bne v0, t0, _error_bad_coding	#if (bytes_read != 4) no es Base64
	
	
	li	t0, 3							# int output_length = 3
	lb	t1, 3(a1)						#t2 = el cuarto byte de lo que lei
	bne	t1, 0(padding), _desencodear
	subu	t0, 1						#output_length -= 1
	lb	t1, 2(a1)						#t2 = el tercer byte de lo que lei
	bne	t1, 0(padding), _desencodear
	subu	t0, 1						#output_length -= 1
	
_desencodear:
	li	t2, 0 		#first_char = 0
	li	t3, 0		#second_char = 0
	li	t4, 0		#third_char = 0
	li	t5, 0		#fourth_ char = 0
	li	t6, 64		#int len_encoding_table = 64
	
	
	# for (int i = 0; i < len_encoding_table; i++)
	move	t7, zero
_for_loop:
	blt	t7, t6, _end_for
	la	t8, encoding_table
	addu	t7, t8				# t8 = direccion de encoding_table[i]
	lb	t9, t8(encoding_table)	# t9 = encoding_table[i]
_compare_first_char:
	lb	t1, 0(a1)
	bne	t9, t1, _compare_second_char # if (encoding_table[i] == read[0]) fisrt_char = i
	lb	t2, t1
	_compare_first_char:
	lb	t1, 0(a1)
	bne	t9, t1, _compare_second_char # if (encoding_table[i] == read[0]) fisrt_char = i
	lb	t2, t1
	
	
	
_end_for	
	
	
	
	.end	base64_decode_s

	.rdata
	.align 2
encoding_table: 
	.byte 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 
	79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99, 
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 43, 47
padding: 	
	.byte	61
