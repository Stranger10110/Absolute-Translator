        START 1000h

start:
m:      mov ax, number
        dec number
        mov bx, number
        mul bx

        dec number
        mul result
        mov result, ax

        cmp number, 0
        jne m

        mov result, ax

number: WORD    10
result: RESW    1
a:	resb	2
b:	byte	"hello"
d:	byte	ED1Ah
c:	byte	256

        END start
