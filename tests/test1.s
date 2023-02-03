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

.end