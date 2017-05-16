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
	.align 	2
	
base64_decode_s:
	.frame		$fp, 52, ra 	# $fp: registro usado como frame pointer
	
	.set	noreorder
	.cpload	t9
	.set 	reorder
	
	#Creo el stack frame
	subu		sp, sp, 52		# 4 (SRA) + 9 (ABA)
	
	.crestore 	36
	# salvado de callee-saved regs en SRA
	sw			$fp, 40(sp)
	sw			ra, 44(sp)
	
	# de aqui al fin de la funcion uso $fp en lugar de sp.
	move		$fp, sp
	
	# Salvo los argumentos
	sw 			a0, 0($fp)
	sw 			a1, 4($fp)
	
	#Leo desde el file descriptor que me llega por parametro
	li			v0, SYS_read		# ver dentro de <sys/syscall.h>.
	lw			a0, 0($fp)			# a0: file descriptor (redundante)
	la			a1, 8($fp)			# a1: data pointer.
	lw			a2, 4				# a2: longitud, leo de a 4 bytes.
	syscall
	
	bne			a3, zero, _error_decode		# Chequeo si la syscall se ejecuto bien
	
	be			v0, zero, _end_decode 		# Si no lei nada, termino
	li			t0, 4						# t0 = 4
	bne 		v0, t0, _error_bad_encoding	# if (bytes_read != 4) no es Base64
	
	
	li			t0, 3							# int output_length = 3
	lbu			t1, 3(a1)						#t1 = el cuarto byte de lo que lei
	la			t9, padding
	bne			t1, 0(t9), _desencodear
	subu		t0, 1								
#output_length -= 1
	lbu			t1, 2(a1)						#t1 = el tercer byte de lo que lei
	bne			t1, 0(t9), _desencodear
	subu		t0, 1							#output_length -= 1
_desencodear:
	move		t2, zero 		#first_char = 0
	move		t3, zero		#second_char = 0
	move		t4, zero		#third_char = 0
	move		t5, zero		#fourth_ char = 0
	li			t6, 64			#int len_encoding_table = 64
	
	
	# for (int i = 0; i < len_encoding_table; i++)
	move		t7, zero						# t7 -> i
_for_loop:
	beq			t7, t6, _end_for				# Uso la condicion be porque t6 = 64 y t7 empieza en 0.
												# Va a cortar en el mismo momento que bgte
	la			t8, encoding_table
	addu		t8, t7, zero					# t8 = direccion de encoding_table[i]
	lbu			t9, 0(t8)						# t9 = encoding_table[i]
_compare_first_char:
	lbu			t1, 0(a1)						# t1 -> read[0]
	bne			t9, t1, _compare_second_char	# if (encoding_table[i] == read[0]) 
	addu		t2, t7, zero					# fisrt_char = i
_compare_second_char:
	lbu			t1, 1(a1)						# t1 -> read[1]
	bne			t9, t1, _compare_third_char		# if (encoding_table[i] == read[1]) second_char = i
	addu		t3, t7, zero
_compare_third_char:
	lbu			t1, 2(a1)						# t1 -> read[2]
	bne			t9, t1, _compare_third_char 	# if (encoding_table[i] == read[2]) third_char = i
	addu		t4, t7, zero
_compare_fourth_char:
	lbu			t1, 3(a1)						# t1 -> read[3]
	bne			t9, t1, _end_comparison 		# if (encoding_table[i] == read[3]) fourth_char = i
	addu		t5, t7, zero
_end_comparison:
	addu		t7, t7, 1
	b			_foor_loop	
	
_end_for:
	sll			t2, t2, 18		# first_char << 18
	sll			t3, t3, 12		# second_char << 12
	sll			t4, t4, 6		# third_char << 6
	
	move		t6, zero		# Limpio t6 de lo anterior. t6 -> sum
	or			t6, t6, t2		# sum = sum + first_char
	or			t6, t6, t3		# sum = sum + second_char
	or			t6, t6, t4		# sum = sum + third_char
	or			t6, t6, t5		# sum = sum + fourth_char
	
	la			a1, 24($fp)
	sb			t6, 2(a1)
	srl			t6, t6, 8		# t6 >> 8
	sb			t6, 1(a1)
	srl			t6, t6, 8		# t6 >> 8
	sb			t6, 0(a1)
	move		a2, t0			# Seteo la longitud a escribir
	lb			a0, 4($fp)		# Recupero el parametro del file descriptor de salida
	li			v0, SYS_write
	syscall
	
	bne	a3, zero, _error_decode # Chequeo si la syscall se ejecuto bien
	
_end_decode:
	# Volvemos al sistema operativo, devolviendo un código 
	# de retorno nulo.
	move		v0, zero
	lw			ra,  44(sp)
	lw			$fp, 40(sp)
	addu		sp, sp, 52
	j			ra
		
	.end	base64_decode_s
	
_error_decode:
	li 			a0, 2 				# salida por stderr
	la 			a1, mensaje_error
	li 			a2, 12
	li 			v0, SYS_write 		# imprime string
	syscall
	li 			a0, 1 				# se carga codigo de error
	li 			v0, SYS_exit 		# exit con valor de salida en a0
	syscall
	b			_end_decode
	
_error_bad_encoding:
	li 			a0, 2 				# salida por stderr
	la 			a1, mensaje_error_bad_encoding
	li 			a2, 12
	li 			v0, SYS_write 		# imprime string
	syscall
	li 			a0, 1 				# se carga codigo de error
	li 			v0, SYS_exit 		# exit con valor de salida en a0
	syscall
	b			_end_decode

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
mensaje_error_bad_encoding:		.asciiz		"La codificacion no es correcta para Base64.\n"
