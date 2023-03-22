; vim:ft=nasm
; x I.y, implemented in assembly because gcc/clang were both generating shitty code
; at first, I just did multi-word compares in inline assembly, but then I saw the compilers were also generating branchy code to moving the endpoints--obviously wrong--at which point I decided there was no point wrestling with them anymore.  Writing in assembly also permitted more effective parallelisation/pipelining.  Overall, this is up to 4x faster than the c versions were

; make sure to declare all prototypes on the c side with __attribute__((sysv_abi)), as it will break on windows otherwise; that's easier than maintaining two sets of prologues

; note: danger with these functions: these are not signal-safe, and because they clobber gs, tls access in signal handlers won't work

; NB. dyalog seems to overtake F_1/F_2 somewhere between 1e5 and 1e6 elements; probably it is partitioning w.  Until then, we trounce it

; TODO: gather for intel
; TODO: for integer x range representable exactly as float (with one integer of buffer on either side to guard against rounded y), we should be able to use F_1 instead of I_1, converting both x and y to float.

global jtiixi_1a, jtiixi_1d
global jtiixf_1a, jtiixf_1d
global jtiixif_1a, jtiixif_1d, jtiixfi_1a, jtiixfi_1d

; I. on singleton floats or mixed floats and ints, a is ascending or descending.  m is guaranteed to be divisible by 6
; Macro parameters:
; %1: function name
; %2: condition code telling when to move q
; %3: load x (movq or cvtsi2sd)
; %4: load y (movq or cvtsi2sd)
; %5: align offset from 64
; Function parameters:
; rdi: z
; rsi: a
; rdx: w
; rcx: #a
; r8: #z  #w
; r9: 2 >.@^. #a.  In other words, the number of inner loop iterations
%macro F_1 5
align 64
; The alignment here is to ensure that .iloop is aligned.  Change this if you change the below code!  I was unable to convince nasm to do the alignment automatically.
times %5 nop
%1:
; stash callee-saved registers
push	rbp
push	rbx
push	r12
push	r13
push	r14
push	r15
rdfsbase rax ;sysv doesn't require saving fs, but we do it anyway to be nice
push	rax
rdgsbase rax
push	rax
vmovq	xmm15,rsp  ;stash rsp in xmm15
wrfsbase rdi  ;fs is z
wrgsbase rdx  ;gs is w
mov	rdx,rsi
mov	rax,r8
vmovq	xmm14,r9	; stash iteration ct
mov	r9d,1
vmovq	xmm13,r9	; set constant 1
; registers:
; xmm0/xmm1: x0/y0
; xmm2/xmm3: x1/y1
; xmm4/xmm5: x2/y2
; xmm6/xmm7: x3/y3
; xmm8/xmm9: x4/y4
; xmm10/xmm11: x5/y5
; xmm12: #inner loop iterations left
; xmm13: constant 1
; xmm14: 2 >.@^. #a
; rdx: a
; rcx: n (#a)
; rax: m (#w and #z)
; r8/r9:   p0,q0
; r10/r11: p1,q1
; r12/r13: p2,q2
; r14/r15: p3,q3
; rdi/rsi: p4,q4
; rsp/rbp: p5,q5
; rbx: scratch
; note use of fs/gs for w/z; we reserve a register for a since most memory accesses go to a, and use of segment registers entails a cycle's delay
; note: use of sar (rather than shr) in inner loop is crucial (see note about idempotence), as we need _1=(_1+_1)>>1.  IMAX/2-sized arrays seem a lot less tenable on 64-bit, and are only possible anyway for 1-byte data 

.oloop:                          ;outer loop: iterate over w
%4	xmm11,qword [gs:rax*8-8] ;load y5
mov	rbp,rcx                  ;q5 (upper)
mov	rsp,-1                   ;p5 (lower)
%4	xmm9,qword [gs:rax*8-16] ;load y4
mov	rsi,rcx                  ;q4 (upper)
mov	rdi,rsp                  ;p4 (lower)
%4	xmm7,qword [gs:rax*8-24] ;load y3
mov	r15,rcx                  ;q3 (upper)
mov	r14,rsp                  ;p3 (lower)
%4	xmm5,qword [gs:rax*8-32] ;load y2
mov	r13,rcx                  ;q2 (upper)
mov	r12,rsp                  ;p2 (lower)
%4	xmm3,qword [gs:rax*8-40] ;load y1
mov     r11,rcx                  ;q1 (upper)
mov	r10,rsp                  ;p1 (lower)
%4	xmm1,qword [gs:rax*8-48] ;load y0
mov     r9,rcx                   ;q1 (upper)
mov	r8,rsp                   ;p1 (lower)
vmovq	xmm12,xmm14              ;set inner loop count
.iloop:              ;inner loop: iterate over a
lea	rbx,[rsp+rbp+1]
sar	rbx,1
%3	xmm10,qword [rdx+rbx*8] ;I tried giving comisd a memory operand, and it was 5-10% slower
vcomisd	xmm10,xmm11       ;performance loss seems to come just from switching comisd operand order; probably uarch/decoder-specific stuff?  (TODO this was on zen2; should re-bench on intel)
cmov%-2	rsp,rbx
cmov%+2	rbp,rbx

lea	rbx,[rdi+rsi+1]
sar	rbx,1
%3	xmm8,qword [rdx+rbx*8]
vcomisd	xmm8,xmm9
cmov%-2	rdi,rbx
cmov%+2	rsi,rbx

lea	rbx,[r14+r15+1]
sar	rbx,1
%3	xmm6,qword [rdx+rbx*8]
vcomisd	xmm6,xmm7
cmov%-2	r14,rbx
cmov%+2	r15,rbx

lea	rbx,[r12+r13+1]
sar	rbx,1
%3	xmm4,qword [rdx+rbx*8]
vcomisd	xmm4,xmm5
cmov%-2	r12,rbx
cmov%+2	r13,rbx

lea	rbx,[r10+r11+1]
sar	rbx,1
%3	xmm2,qword [rdx+rbx*8]
vcomisd	xmm2,xmm3
cmov%-2	r10,rbx
cmov%+2	r11,rbx

lea	rbx,[r8+r9+1]
sar	rbx,1
%3	xmm0,qword [rdx+rbx*8]
vcomisd	xmm0,xmm1
cmov%-2	r8,rbx
cmov%+2	r9,rbx

vpsubq	xmm12,xmm13
vptest	xmm12,xmm12
jnz	.iloop

; todo consider not doing redundant work, but instead reloading each parameter once it finishes?  I think this requires index registers, and I'm using all the registers already

mov	[fs:rax*8-8],rbp  ;store result for y5
mov	[fs:rax*8-16],rsi ;store result for y4
mov	[fs:rax*8-24],r15 ;store result for y3
mov	[fs:rax*8-32],r13 ;store result for y2
mov	[fs:rax*8-40],r11 ;store result for y1
mov	[fs:rax*8-48],r9  ;store result for y0

sub	rax,6
jnz	.oloop       ;continue if more items of w to process

vmovq	rsp,xmm15
;mov	rsp,[fs:-8]
pop	rax
wrgsbase rax
pop	rax
wrfsbase rax
pop	r15
pop	r14
pop	r13
pop	r12
pop	rbx
pop	rbp
ret
%endmacro

;float comparisons set above/below flags
F_1 jtiixf_1a, ae, vmovq, vmovq, 28
F_1 jtiixf_1d, be, vmovq, vmovq, 28
F_1 jtiixif_1a, ae, vcvtsi2sd, vmovq, 28
F_1 jtiixif_1d, be, vcvtsi2sd, vmovq, 28
F_1 jtiixfi_1a, ae, vmovq, vcvtsi2sd, 22
F_1 jtiixfi_1d, be, vmovq, vcvtsi2sd, 22

; I. on singleton integers, a is ascending or descending.  m is guaranteed to be divisible by 4
; %1: function name
; %2: condition code telling when to move q
; Function parameters:
; rdi: z
; rsi: a
; rdx: w
; rcx: #a
; r8: #z  #w
; r9: 2 >.@^. #a.  In other words, the number of inner loop iterations
%macro I_1 2
; The alignment here is to ensure that .iloop is aligned.  Change this if you change the below code!  I was unable to convince nasm to do the alignment automatically.  Still waiting for an SMT-solver-equipped assembler...
align 64
%1:
; stash callee-saved registers
push	rbp
push	rbx
push	r12
push	r13
push	r14
push	r15
rdfsbase rax ;sysv doesn't require saving fs, but we do it anyway to be nice
push	rax
rdgsbase rax
push	rax
vmovq	xmm0,rsp  ;stash rsp in xmm0
wrfsbase rdi  ;fs is z
wrgsbase rdx  ;gs is w
mov	rdx,rsi
mov	rax,r8
vmovq	xmm14,r9
mov	r9d,1
vmovq	xmm13,r9
; registers:
; rdi: y0
; r8/r9: p0/q0
; rsi: y1
; r10/r11: p1/q1
; rsp: y2
; r12/r13: p2/q2
; rbp: y3
; r14/r15: p3/q3
; rdx: a
; rcx: n (#a)
; rax: m (#w and #z)
; rbx: scratch
; xmm12: #inner loop iterations left
; xmm13: constant 1
; xmm14: 2 >.@^. #a
; note use of fs/gs for w/z; we reserve a register for a since most memory accesses go to a, and use of segment registers entails a cycle's delay
; note: use of sar (rather than shr) in inner loop is crucial (see note about idempotence), as we need _1=(_1+_1)>>1.  IMAX/2-sized arrays seem a lot less tenable on 64-bit, and are only possible anyway for 1-byte data 

.oloop:               ;outer loop: iterate over w
mov	rbp,[gs:rax*8-8]  ;load y3
mov	rsp,[gs:rax*8-16] ;load y3
mov	rsi,[gs:rax*8-24] ;load y1
mov	rdi,[gs:rax*8-32] ;load y0
mov     r15,rcx      ;q3 (upper)
mov	r14,-1       ;p3 (lower)
mov	r13,rcx      ;q2 (upper)
mov	r12,r14      ;p2 (lower)
mov	r11,rcx      ;q1 (upper)
mov	r10,r14      ;p1 (lower)
mov	r9,rcx       ;q0 (upper)
mov	r8,r14       ;p0 (lower)
vmovq	xmm12,xmm14
.iloop:              ;inner loop: iterate over a
; can we start the cmp before the sar by using rbx*4?  Obviously not directly, but what if we keep p/q premultiplied by 2 (would need to mask off the bottom bit--ok), and say rbx=r15+r14+2 and load rdx+rbx*2? 
lea	rbx,[r15+r14+1]
sar	rbx,1
cmp	rbp,[rdx+rbx*8];compare y3 to x3 (note reversed from normal)
cmov%-2	r14,rbx
cmov%+2	r15,rbx

lea	rbx,[r13+r12+1]
sar	rbx,1
cmp	rsp,[rdx+rbx*8]
cmov%-2	r12,rbx
cmov%+2	r13,rbx

lea	rbx,[r11+r10+1]
sar	rbx,1
cmp	rsi,[rdx+rbx*8]
cmov%-2	r10,rbx
cmov%+2	r11,rbx

lea	rbx,[r9+r8+1]
sar	rbx,1
cmp	rdi,[rdx+rbx*8]
cmov%-2	r8,rbx
cmov%+2	r9,rbx

vpsubq	xmm12,xmm13
vptest	xmm12,xmm12
jnz	.iloop

mov	[fs:rax*8-8],r15  ;store result for y3
mov	[fs:rax*8-16],r13 ;store result for y2
mov	[fs:rax*8-24],r11 ;store result for y1
mov	[fs:rax*8-32],r9  ;store result for y0

sub	rax,4
jnz	.oloop       ;continue if more items of w to process

vmovq	rsp,xmm0
;mov	rsp,[fs:-8]
pop	rax
wrgsbase rax
pop	rax
wrfsbase rax
pop	r15
pop	r14
pop	r13
pop	r12
pop	rbx
pop	rbp
ret
%endmacro

I_1 jtiixi_1a, le ;note backwards conditions; compare is reversed
I_1 jtiixi_1d, ge
