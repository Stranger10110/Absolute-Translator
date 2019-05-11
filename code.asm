	START 1000

start:
m:	mov	ax, number
	dec	number
	mov	bx, number
	mul	bx
	
	cmp	number, 1
	jne	m
	
	mov	result, ax

number:	WORD	10
result:	RESW	1

	END start
