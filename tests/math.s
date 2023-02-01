# file: math.s

.global mathAdd, mathSub, mathMul, mathDiv

.section math

#user code
and r1, r2
or r3, r4
xor r4, r5
test r0, r1
shl r8, r8
shr r6, r2

#user code end 
mathAdd:
  push r1
  ldr r0, [r6 + 4]
  ldr r1, [r6 + 6]
  add r0, r1
  pop r1
  ret

mathSub:
  push r1
  ldr r0, [r6 + 4]
  ldr r1, [r6 + 6]
  sub r0, r1
  pop r1
  ret

mathMul:
  push r1
  ldr r0, [r6 + 4]
  ldr r1, [r6 + 6]
  mul r0, r1
  pop r1
  ret

mathDiv:
  push r1
  ldr r0, [r6 + 4]
  ldr r1, [r6 + 6]
  div r0, r1
  pop r1
  ret

.end
