.section	.text
.global main

main:	MOVL $10, %ebx
	MOVL $0, %eax
	
LOOP:
	INC %eax
	CMP %eax, %ebx
	JNE LOOP

