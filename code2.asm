	START 	0

start:  mov	ax, 1
	mov	cx, 8
m:	mul	cx
	loop 	m

	END	start
