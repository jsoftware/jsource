	.file	"d.c"
.globl a
	.data
	.align 8
	.type	a, @object
	.size	a, 8
a:
	.quad	1
.globl b
	.align 8
	.type	b, @object
	.size	b, 8
b:
	.quad	2
.globl c
	.align 8
	.type	c, @object
	.size	c, 8
c:
	.quad	3
.globl d
	.align 8
	.type	d, @object
	.size	d, 8
d:
	.quad	4
.globl e
	.align 8
	.type	e, @object
	.size	e, 8
e:
	.quad	5
.globl f
	.align 8
	.type	f, @object
	.size	f, 8
f:
	.quad	6
.globl g
	.align 8
	.type	g, @object
	.size	g, 8
g:
	.quad	7
.globl h
	.align 8
	.type	h, @object
	.size	h, 8
h:
	.quad	8
.globl i
	.align 8
	.type	i, @object
	.size	i, 8
i:
	.quad	9
.globl j
	.align 8
	.type	j, @object
	.size	j, 8
j:
	.quad	10
.globl k
	.align 8
	.type	k, @object
	.size	k, 8
k:
	.quad	11
.globl l
	.align 8
	.type	l, @object
	.size	l, 8
l:
	.quad	12
.globl m
	.align 8
	.type	m, @object
	.size	m, 8
m:
	.quad	13
.globl n
	.align 8
	.type	n, @object
	.size	n, 8
n:
	.quad	14
.globl o
	.align 8
	.type	o, @object
	.size	o, 8
o:
	.quad	15
.globl p
	.align 8
	.type	p, @object
	.size	p, 8
p:
	.quad	16
.globl q
	.align 8
	.type	q, @object
	.size	q, 8
q:
	.quad	17
.globl da
	.align 8
	.type	da, @object
	.size	da, 8
da:
	.long	0
	.long	1072693248
.globl db
	.align 8
	.type	db, @object
	.size	db, 8
db:
	.long	0
	.long	1073741824
.globl dc
	.align 8
	.type	dc, @object
	.size	dc, 8
dc:
	.long	0
	.long	1074266112
.globl dd
	.align 8
	.type	dd, @object
	.size	dd, 8
dd:
	.long	0
	.long	1074790400
.globl de
	.align 8
	.type	de, @object
	.size	de, 8
de:
	.long	0
	.long	1075052544
.globl df
	.align 8
	.type	df, @object
	.size	df, 8
df:
	.long	0
	.long	1075314688
.globl dg
	.align 8
	.type	dg, @object
	.size	dg, 8
dg:
	.long	0
	.long	1075576832
.globl dh
	.align 8
	.type	dh, @object
	.size	dh, 8
dh:
	.long	0
	.long	1075838976
.globl di
	.align 8
	.type	di, @object
	.size	di, 8
di:
	.long	0
	.long	1075970048
.globl dj
	.align 8
	.type	dj, @object
	.size	dj, 8
dj:
	.long	0
	.long	1076101120
.globl dk
	.align 8
	.type	dk, @object
	.size	dk, 8
dk:
	.long	0
	.long	1076232192
.globl dl
	.align 8
	.type	dl, @object
	.size	dl, 8
dl:
	.long	0
	.long	1076363264
.globl dm
	.align 8
	.type	dm, @object
	.size	dm, 8
dm:
	.long	0
	.long	1076363264
.globl dn
	.align 8
	.type	dn, @object
	.size	dn, 8
dn:
	.long	0
	.long	1076494336
.globl dox
	.align 8
	.type	dox, @object
	.size	dox, 8
dox:
	.long	0
	.long	1076625408
.globl dp
	.align 8
	.type	dp, @object
	.size	dp, 8
dp:
	.long	0
	.long	1076756480
.globl dq
	.align 8
	.type	dq, @object
	.size	dq, 8
dq:
	.long	0
	.long	1076887552
.globl dr
	.align 8
	.type	dr, @object
	.size	dr, 8
dr:
	.long	0
	.long	1076953088
.globl fa
	.align 4
	.type	fa, @object
	.size	fa, 4
fa:
	.long	1065353216
.globl fb
	.align 4
	.type	fb, @object
	.size	fb, 4
fb:
	.long	1073741824
	.text
.globl test
	.type	test, @function
test:
.LFB3:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	subq	$64, %rsp
.LCFI2:
	movl	$2, %esi
	movl	$1, %edi
	call	fii
	movl	$4, %ecx
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	fiiii
	movl	$6, %r9d
	movl	$5, %r8d
	movl	$4, %ecx
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	fiiiiii
	movq	h(%rip), %rax
	movq	%rax, 8(%rsp)
	movq	g(%rip), %rax
	movq	%rax, (%rsp)
	movq	f(%rip), %r9
	movq	e(%rip), %r8
	movq	d(%rip), %rcx
	movq	c(%rip), %rdx
	movq	b(%rip), %rsi
	movq	a(%rip), %rdi
	call	fiiiiiiii
	movabsq	$4612136378390124954, %rax
	movq	%rax, -16(%rbp)
	movlpd	-16(%rbp), %xmm0
	movl	$1, %edi
	call	fid
	movabsq	$4616639978017495450, %rax
	movabsq	$4612136378390124954, %rdx
	movq	%rax, -16(%rbp)
	movlpd	-16(%rbp), %xmm1
	movl	$3, %esi
	movq	%rdx, -16(%rbp)
	movlpd	-16(%rbp), %xmm0
	movl	$1, %edi
	call	fidid
	movlpd	df(%rip), %xmm2
	movq	e(%rip), %rdx
	movlpd	dd(%rip), %xmm1
	movq	c(%rip), %rsi
	movlpd	db(%rip), %xmm0
	movq	a(%rip), %rdi
	call	fididid
	movq	dr(%rip), %rax
	movq	%rax, 24(%rsp)
	movq	q(%rip), %rax
	movq	%rax, 16(%rsp)
	movq	o(%rip), %rax
	movq	%rax, 8(%rsp)
	movq	m(%rip), %rax
	movq	%rax, (%rsp)
	movlpd	dp(%rip), %xmm7
	movlpd	dn(%rip), %xmm6
	movlpd	dl(%rip), %xmm5
	movq	k(%rip), %r9
	movlpd	dj(%rip), %xmm4
	movq	i(%rip), %r8
	movlpd	dh(%rip), %xmm3
	movq	g(%rip), %rcx
	movlpd	df(%rip), %xmm2
	movq	e(%rip), %rdx
	movlpd	dd(%rip), %xmm1
	movq	c(%rip), %rsi
	movlpd	db(%rip), %xmm0
	movq	a(%rip), %rdi
	call	fid9
	movabsq	$4616639978017495450, %rax
	movabsq	$4614613358185178726, %rdx
	movabsq	$4612136378390124954, %rcx
	movabsq	$4607632778762754458, %rsi
	movq	%rax, -16(%rbp)
	movlpd	-16(%rbp), %xmm3
	movq	%rdx, -16(%rbp)
	movlpd	-16(%rbp), %xmm2
	movq	%rcx, -16(%rbp)
	movlpd	-16(%rbp), %xmm1
	movq	%rsi, -16(%rbp)
	movlpd	-16(%rbp), %xmm0
	call	fdddd
	movabsq	$4621706527598287258, %rax
	movabsq	$4621650232602945126, %rdx
	movabsq	$4621593937607602995, %rcx
	movabsq	$4621537642612260864, %rsi
	movabsq	$4621481347616918733, %rdi
	movabsq	$4621425052621576602, %r8
	movabsq	$4621368757626234470, %r9
	movabsq	$4621312462630892339, %r10
	movq	%rax, -16(%rbp)
	movlpd	-16(%rbp), %xmm7
	movq	%rdx, -16(%rbp)
	movlpd	-16(%rbp), %xmm6
	movq	%rcx, -16(%rbp)
	movlpd	-16(%rbp), %xmm5
	movq	%rsi, -16(%rbp)
	movlpd	-16(%rbp), %xmm4
	movq	%rdi, -16(%rbp)
	movlpd	-16(%rbp), %xmm3
	movq	%r8, -16(%rbp)
	movlpd	-16(%rbp), %xmm2
	movq	%r9, -16(%rbp)
	movlpd	-16(%rbp), %xmm1
	movq	%r10, -16(%rbp)
	movlpd	-16(%rbp), %xmm0
	call	fd8
	movq	dl(%rip), %rax
	movq	%rax, 24(%rsp)
	movq	dk(%rip), %rax
	movq	%rax, 16(%rsp)
	movq	dj(%rip), %rax
	movq	%rax, 8(%rsp)
	movq	di(%rip), %rax
	movq	%rax, (%rsp)
	movl	$ddata, %edi
	movlpd	dh(%rip), %xmm7
	movlpd	dg(%rip), %xmm6
	movlpd	df(%rip), %xmm5
	movlpd	de(%rip), %xmm4
	movlpd	dd(%rip), %xmm3
	movlpd	dc(%rip), %xmm2
	movlpd	db(%rip), %xmm1
	movlpd	da(%rip), %xmm0
	call	fd12
	movlpd	db(%rip), %xmm0
	cvtsd2ss	%xmm0, %xmm1
	movss	%xmm1, -20(%rbp)
	movl	-20(%rbp), %edx
	movlpd	da(%rip), %xmm0
	cvtsd2ss	%xmm0, %xmm1
	movss	%xmm1, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%edx, -20(%rbp)
	movss	-20(%rbp), %xmm1
	movl	%eax, -20(%rbp)
	movss	-20(%rbp), %xmm0
	call	ff2
	movlpd	db(%rip), %xmm1
	movlpd	da(%rip), %xmm0
	call	fd2
	leave
	ret
.LFE3:
	.size	test, .-test
.globl foo
	.type	foo, @function
foo:
.LFB5:
	pushq	%rbp
.LCFI3:
	movq	%rsp, %rbp
.LCFI4:
	movsd	%xmm0, -8(%rbp)
	movsd	%xmm1, -16(%rbp)
	movsd	%xmm2, -24(%rbp)
	movsd	%xmm3, -32(%rbp)
	movsd	%xmm4, -40(%rbp)
	movsd	%xmm5, -48(%rbp)
	movsd	%xmm6, -56(%rbp)
	movsd	%xmm7, -64(%rbp)
	movlpd	-8(%rbp), %xmm0
	addsd	-16(%rbp), %xmm0
	addsd	-24(%rbp), %xmm0
	addsd	-32(%rbp), %xmm0
	addsd	-40(%rbp), %xmm0
	addsd	-48(%rbp), %xmm0
	addsd	-56(%rbp), %xmm0
	addsd	-64(%rbp), %xmm0
	addsd	16(%rbp), %xmm0
	addsd	24(%rbp), %xmm0
	addsd	32(%rbp), %xmm0
	addsd	40(%rbp), %xmm0
	movsd	%xmm0, -72(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, -72(%rbp)
	movlpd	-72(%rbp), %xmm0
	leave
	ret
.LFE5:
	.size	foo, .-foo
	.comm	ddata,96,32
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.quad	.LFB3
	.quad	.LFE3-.LFB3
	.byte	0x4
	.long	.LCFI0-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.quad	.LFB5
	.quad	.LFE5-.LFB5
	.byte	0x4
	.long	.LCFI3-.LFB5
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI4-.LCFI3
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE3:
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.3.3 (SuSE Linux)"
