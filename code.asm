        START 1000h

ZERO:	WORD	0

start:
m:      mov ax, number
        dec number
        mov bx, number
        mul bx

        dec number
        mul result
        mov result, ax

        cmp number, ZERO
        jne m

        mov result, ax

number: WORD    10
result: RESW    1
a:	resb	2
b:	byte	hello
c:	byte	256
d:	byte	ED1Ah

        END start
