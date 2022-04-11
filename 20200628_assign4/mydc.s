### 20200628 JinWooGyeol
### Assignment 4
### mydc.s
### --------------------------------------------------------------------
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------
	.equ	MOVERFLOW, -2147483648
	.equ	ARRAYSIZE, 20
	.equ	EOF, -1
	.equ	PRINTFOFFSET, 8
	.equ	FPRINTFOFFSET, 8
	.equ	SCANFOFFSET, 8
	.equ	EXPONENTIALOFFSET, 8
	.equ	ISDIGITOFFSET, 4
	.equ	ATOIOFFSET, 4
	.equ	STACKSIZE, 4
	.equ	OVER_TRUE, 1
	.equ	OVER_FALSE, 0
	.equ	ZERO, 0

.section ".rodata"

scanfFormat:
	.asciz "%s"
printfFormat:
	.asciz "%d\n"
errormessage_empty:
	.asciz "dc: stack empty\n"
errormessage_divide:
	.asciz "dc: divide by zero\n"
errormessage_overflow:
	.asciz "dc: overflow happens\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------
        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

## dc number stack initialized. %esp = %ebp
## go to input
main:

	pushl	%ebp
	movl	%esp, %ebp

## get string by scanf and save in buffer
## doing calculation with my input
input:

	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $SCANFOFFSET, %esp

	## check if input is EOF
	cmp		$EOF, %eax
	je		quit

	## check buffer[0] is digit
	movl	$buffer, %ebx
	movzx	0(%ebx), %esi
	pushl	%esi
	call	isdigit
	addl	$ISDIGITOFFSET, %esp
	cmpl	$ZERO, %eax
	je		not_isdigit

	## if isdigit function return nonzero value
	## change string to integer using atoi
	add:
		pushl	$buffer
		call	atoi
		addl	$ATOIOFFSET, %esp
		pushl	%eax
		jmp		input

	## if isdigit function return 0, 
	## same as input is not isdigit
	not_isdigit:
		cmpb	$'_', buffer
		je		negative
		cmpb	$'p', buffer
		je		print
		cmpb	$'q', buffer
		je		quit
		cmpb	$'+', buffer
		je		plus
		cmpb	$'-', buffer
		je		minus
		cmpb	$'*', buffer
		je		multiple
		cmpb	$'/', buffer
		je		divide
		cmpb	$'%', buffer
		je		remain
		cmpb	$'^', buffer
		je		power
		cmpb	$'f', buffer
		je		printall
		cmpb	$'c', buffer
		je		clear
		cmpb	$'d', buffer
		je		duplicate
		cmpb	$'r', buffer
		je		reverse
		jmp input

	## check stack contains at least one operand
	## print value at the top of stack
	print:
		cmp		%ebp, %esp
		je 		errorprint1
		pushl 	(%esp)
		pushl 	$printfFormat
		call 	printf
		addl 	$PRINTFOFFSET, %esp
		jmp 	input
	
	## print error dc: stack empty
	errorprint1:
		pushl 	$errormessage_empty
		pushl 	stderr
		call 	fprintf
		addl 	$FPRINTFOFFSET, %esp
		jmp 	input
	
	## while we use two number at the top of stack,
	## if there are no second number,
	## push first popped value again and print error
	recover:
		pushl	%ebx
		jmp		errorprint1

	## print error dc: divide by zero
	errorprint2:
		pushl	$errormessage_divide
		pushl	stderr
		call	fprintf
		addl	$FPRINTFOFFSET, %esp
		jmp		quit

	## print error dc: overflow happens
	errorprint3:
		pushl	$errormessage_overflow
		pushl	stderr
		call	fprintf
		addl	$FPRINTFOFFSET, %esp
		jmp		quit
	
	## if input buffer start with '_',
	## change it to '-' and use atoi for handling negative number
	negative:
		movb	$'-', buffer
		pushl	$buffer
		call	atoi
		addl	$ATOIOFFSET, %esp
		pushl	%eax
		jmp 	input

	## if input buffer is '+'
	## check stack contains enough operands
	## add two value at register eax and ebx
	## save value to eax and push it
	## check overflow by jo and print error
	plus:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		addl	%ebx, %eax
		jo		errorprint3
		pushl	%eax
		jmp		input

	## if input buffer is '-'
	## check stack contains enough operands
	## subtract two value at register eax and ebx
	## save value to eax and push it
	## check overflow by jo and print error
	minus:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		subl	%ebx, %eax
		jo		errorprint3
		pushl	%eax
		jmp		input

	## if input buffer is '*'
	## check stack contains enough operands
	## multiply two value at register eax and ebx
	## save value to eax and push it
	## check overflow by jo and print error
	multiple:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		imul	%ebx
		jo		errorprint3
		pushl	%eax
		jmp		input

	## if input buffer is '/'
	## check stack contains enough operands
	## divide two value at register eax and ebx
	## save value to eax and push it
	## overflow can happen when I divide -2^31(MOVERFLOW) with -1
	## check overflow case and divide with 0 case -> make error
	divide:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		cmpl	$ZERO, %ebx
		je		errorprint2
		cmpl	$MOVERFLOW, %eax
		je		divide_overflow
		cdq
		idivl	%ebx
		pushl	%eax
		jmp		input

	## if eax is MOVERFLOW and ebx is -1 -> overflow
	## else -> just print quotient
	divide_overflow:
		cmpl	$(-1), %ebx
		je		errorprint3
		cdq
		idivl	%ebx
		pushl	%eax
		jmp		input

	## if input buffer is '%'
	## check stack contains enough operands
	## divide two value at register eax and ebx
	## save value to edx(remainder) and push it
	## overflow can happen when I divide -2^31(MOVERFLOW) with -1
	## check overflow case and divide with 0 case -> make error
	remain:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		cmpl	$ZERO, %ebx
		je		errorprint2
		cmpl	$MOVERFLOW, %eax
		je		remain_overflow
		cdq
		idivl	%ebx
		pushl	%edx
		jmp		input

	## if eax is MOVERFLOW and ebx is -1 -> overflow
	## else -> just print remainder
	remain_overflow:
		cmpl	$(-1), %ebx
		je		errorprint3
		cdq
		idivl	%ebx
		pushl	%edx
		jmp		input


	## if input buffer is '^'
	## check stack contains enough operands
	## push poped value as parameter, then call exponential function
	## if ecx is 1 -> overflow happens -> print error
	## else, push value stored at eax(return value)
	power:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		movl	%eax, %edi
		movl	%ebx, %esi
		pushl	%edi
		pushl	%esi
		call	exponential
		add		$EXPONENTIALOFFSET, %esp
		cmp		$OVER_TRUE, %ecx
		je		quit
		pushl	%eax
		jmp 	input

	## if input buffer is 'f'
	## copy value of %esp to %ebx
	## after print, add STACKSIZE to %ebx
	## print value of (%ebx) until it is same to %ebp
	printall:
		movl	%esp, %ebx
		printall_loop:
			cmp		%ebp, %ebx
			je		input
			pushl 	(%ebx)
			pushl 	$printfFormat
			call 	printf
			addl 	$PRINTFOFFSET, %esp
			addl	$STACKSIZE, %ebx
			jmp		printall_loop
	
	## if input buffer is 'c'
	## add STACKSIZE to %esp until it is same to %ebp
	clear:
		cmp		%ebp, %esp
		je		input
		addl	$STACKSIZE, %esp
		jmp		clear

	## if input buffer is 'd'
	## check stack contains enough operands
	## duplicate top value of stack by push (%esp)
	duplicate:
		cmp		%ebp, %esp
		je		errorprint1
		movl	(%esp), %ebx
		pushl	%ebx
		jmp		input
	
	## if input buffer is 'r'
	## check stack contains enough operands
	## pop each value to %ebx and %eax
	## then, push them with different order
	reverse:
		cmp		%ebp, %esp
		je		errorprint1
		popl	%ebx
		cmp		%ebp, %esp
		je		recover
		popl	%eax
		pushl	%ebx
		pushl	%eax
		jmp		input


## first, copy value of eax to ecx
## multiply %eax and %ecx for %ebx-1 times
## check overflow
## if ebx is zero, move 1 to eax
## set ecx value OVER_FALSE = 0 / OVER_TRUE = 1
## -> for handling overflow
exponential:
	pushl	%ebp
	movl	%esp, %ebp
	movl	12(%ebp), %eax
	movl	8(%ebp), %ebx
	movl	%eax, %ecx
	cmpl	$ZERO, %ebx
	je		push1
	exponential_loop:
		decl	%ebx
		cmpl	$ZERO, %ebx
		je		end_exponential_loop
		imul	%ecx
		jo		exponential_error
		jmp		exponential_loop
	end_exponential_loop:
		movl	$OVER_FALSE, %ecx
		jmp		quit_exponential
	
	push1:
		movl	$1, %eax
		movl	$OVER_FALSE, %ecx
		jmp		quit_exponential

	exponential_error:
		pushl	$errormessage_overflow
		pushl	stderr
		call	fprintf
		addl	$FPRINTFOFFSET, %esp
		movl	$OVER_TRUE, %ecx
		jmp		quit_exponential
	
	quit_exponential:
		movl	%ebp, %esp
		popl	%ebp
		ret

## quit dc
quit:
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret
	