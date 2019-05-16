        START 1000

ZERO:	WORD	0
number: WORD    10
result: RESW    1

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



        END start
