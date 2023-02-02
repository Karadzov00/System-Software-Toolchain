

ldr r4, $0xFF
ldr r5, $234
ldr r5, $130
ldr r4, $a #za ovo mora relokacioni zapis da se generise, jer ne znamo da li je lokalan ili globalan
ldr r4, 0xFF
ldr r4, a
ldr r4, %a
ldr r4, r5 #generisemo kod odmah
ldr r5, [r4]
ldr r5, [r4 + 0xFF]
ldr r5, [r4 + a]


jmp 0xFF
jmp a
jmp *0xFF
call *a
jne *r4
jgt *[r4]
jmp *[psw]
jmp *[r4 + 0xFF]
jmp *[r4+a]
jmp %a