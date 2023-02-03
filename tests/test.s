.global glb
.section data
.skip 4
.word a,b,c
ldr r2, c
ldr sp, %a
a:
ldr r4, $0xFF
b:
ldr r5, $234
c:
ldr r5, $130
ldr r4, $a #za ovo mora relokacioni zapis da se generise, jer ne znamo da li je lokalan ili globalan
ldr r4, 0xFF
ldr r4, a
ldr r4, %a
ldr r3, %b
ldr r3, %random
ldr r3, %glb
ldr r4, r5 #generisemo kod odmah
ldr r5, [r4]
ldr r5, [r4 + 0xFF]
ldr r5, [r4 + a]

.section jumps
jmp 0xFF
jne a
d:
jmp *0xFF
call *a
jne *r4
e:
jgt *[r4]
jmp *[psw]
jmp *[r4 + 0xFF]
jmp *[r4+a]
jmp %a

push r5
pop r3

.word d, e, m


.end