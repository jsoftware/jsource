	.file	"asm64noovf.c"
	.text
	.p2align 4,,15
.globl asmplusvv
	.type	asmplusvv, @function
asmplusvv:
.LFB23:
	.p2align 4,,7
.L2:
	decq	%rdi
	movq	(%rcx,%rdi,8), %r8
	addq	(%rdx,%rdi,8), %r8
	jo overasmplusvv
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L2
	xorl	%eax, %eax
	ret
overasmplusvv:
	movl $50,%eax
	ret
.LFE23:
	.size	asmplusvv, .-asmplusvv
	.p2align 4,,15
.globl asmminusvv
	.type	asmminusvv, @function
asmminusvv:
.LFB24:
	.p2align 4,,7
.L8:
	decq	%rdi
	movq	(%rdx,%rdi,8), %r8
	subq	(%rcx,%rdi,8), %r8
	jo overasmminusvv
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L8
	xorl	%eax, %eax
	ret
overasmminusvv:
	movl $50,%eax
	ret
.LFE24:
	.size	asmminusvv, .-asmminusvv
	.p2align 4,,15
.globl asmtymesvv
	.type	asmtymesvv, @function
asmtymesvv:
.LFB25:
	.p2align 4,,7
.L14:
	decq	%rdi
	movq	(%rcx,%rdi,8), %r8
	imulq	(%rdx,%rdi,8), %r8
	jo overasmtymesvv
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L14
	xorl	%eax, %eax
	ret
overasmtymesvv:
	movl $50,%eax
	ret
.LFE25:
	.size	asmtymesvv, .-asmtymesvv
	.p2align 4,,15
.globl asmplus1v
	.type	asmplus1v, @function
asmplus1v:
.LFB26:
	.p2align 4,,7
.L20:
	decq	%rdi
	movq	%rdx, %r8
	addq	(%rcx,%rdi,8), %r8
	jo overasmplus1v
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L20
	xorl	%eax, %eax
	ret
overasmplus1v:
	movl $50,%eax
	ret
.LFE26:
	.size	asmplus1v, .-asmplus1v
	.p2align 4,,15
.globl asmminus1v
	.type	asmminus1v, @function
asmminus1v:
.LFB27:
	.p2align 4,,7
.L26:
	decq	%rdi
	movq	%rdx, %r8
	subq	(%rcx,%rdi,8), %r8
	jo overasmminus1v
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L26
	xorl	%eax, %eax
	ret
overasmminus1v:
	movl $50,%eax
	ret
.LFE27:
	.size	asmminus1v, .-asmminus1v
	.p2align 4,,15
.globl asmtymes1v
	.type	asmtymes1v, @function
asmtymes1v:
.LFB28:
	.p2align 4,,7
.L32:
	decq	%rdi
	movq	%rdx, %r8
	imulq	(%rcx,%rdi,8), %r8
	jo overasmtymes1v
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L32
	xorl	%eax, %eax
	ret
overasmtymes1v:
	movl $50,%eax
	ret
.LFE28:
	.size	asmtymes1v, .-asmtymes1v
	.p2align 4,,15
.globl asmminusv1
	.type	asmminusv1, @function
asmminusv1:
.LFB29:
	.p2align 4,,7
.L38:
	decq	%rdi
	movq	(%rdx,%rdi,8), %r8
	subq	%rcx, %r8
	jo overasmminusv1
	testq	%rdi, %rdi
	movq	%r8, (%rsi,%rdi,8)
	jg	.L38
	xorl	%eax, %eax
	ret
overasmminusv1:
	movl $50,%eax
	ret
.LFE29:
	.size	asmminusv1, .-asmminusv1
	.p2align 4,,15
.globl asmplusrv
	.type	asmplusrv, @function
asmplusrv:
.LFB30:
	.p2align 4,,7
.L44:
	decq	%rdi
	movq	(%rsi,%rdi,8), %rcx
	addq	(%rdx,%rdi,8), %rcx
	jo overasmplusrv
	testq	%rdi, %rdi
	movq	%rcx, (%rsi,%rdi,8)
	jg	.L44
	xorl	%eax, %eax
	ret
overasmplusrv:
	movl $50,%eax
	ret
.LFE30:
	.size	asmplusrv, .-asmplusrv
	.p2align 4,,15
.globl asmminusrv
	.type	asmminusrv, @function
asmminusrv:
.LFB31:
	.p2align 4,,7
.L50:
	decq	%rdi
	movq	(%rdx,%rdi,8), %rcx
	subq	(%rsi,%rdi,8), %rcx
	jo overasmminusrv
	testq	%rdi, %rdi
	movq	%rcx, (%rsi,%rdi,8)
	jg	.L50
	xorl	%eax, %eax
	ret
overasmminusrv:
	movl $50,%eax
	ret
.LFE31:
	.size	asmminusrv, .-asmminusrv
	.p2align 4,,15
.globl asmtymesrv
	.type	asmtymesrv, @function
asmtymesrv:
.LFB32:
	.p2align 4,,7
.L56:
	decq	%rdi
	movq	(%rsi,%rdi,8), %rcx
	imulq	(%rdx,%rdi,8), %rcx
	jo overasmtymesrv
	testq	%rdi, %rdi
	movq	%rcx, (%rsi,%rdi,8)
	jg	.L56
	xorl	%eax, %eax
	ret
overasmtymesrv:
	movl $50,%eax
	ret
.LFE32:
	.size	asmtymesrv, .-asmtymesrv
	.p2align 4,,15
.globl asmplusr
	.type	asmplusr, @function
asmplusr:
.LFB33:
	xorl	%eax, %eax
	.p2align 4,,7
.L62:
	decq	%rdi
	addq	(%rdx,%rdi,8), %rax
	jo overasmplusr
	testq	%rdi, %rdi
	jg	.L62
	movq	%rax, (%rsi)
	xorl	%eax, %eax
	ret
overasmplusr:
	movl $50,%eax
	ret
.LFE33:
	.size	asmplusr, .-asmplusr
	.p2align 4,,15
.globl asmminusr
	.type	asmminusr, @function
asmminusr:
.LFB34:
	xorl	%ecx, %ecx
	.p2align 4,,7
.L68:
	decq	%rdi
	movq	(%rdx,%rdi,8), %r8
	subq	%rcx, %r8
	jo overasmminusr
	testq	%rdi, %rdi
	movq	%r8, %rcx
	jg	.L68
	xorl	%eax, %eax
	movq	%rcx, (%rsi)
	ret
overasmminusr:
	movl $50,%eax
	ret
.LFE34:
	.size	asmminusr, .-asmminusr
	.p2align 4,,15
.globl asmtymesr
	.type	asmtymesr, @function
asmtymesr:
.LFB35:
	movl	$1, %eax
	.p2align 4,,7
.L74:
	decq	%rdi
	imulq	(%rdx,%rdi,8), %rax
	jo overasmtymesr
	testq	%rdi, %rdi
	jg	.L74
	movq	%rax, (%rsi)
	xorl	%eax, %eax
	ret
overasmtymesr:
	movl $50,%eax
	ret
.LFE35:
	.size	asmtymesr, .-asmtymesr
	.p2align 4,,15
.globl asmplusp
	.type	asmplusp, @function
asmplusp:
.LFB36:
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,7
.L80:
	addq	(%rdx,%rax,8), %rcx
	jo overasmplusp
	movq	%rcx, (%rsi,%rax,8)
	incq	%rax
	cmpq	%rdi, %rax
	jl	.L80
	xorl	%eax, %eax
	ret
overasmplusp:
	movl $50,%eax
	ret
.LFE36:
	.size	asmplusp, .-asmplusp
	.p2align 4,,15
.globl asmminusp
	.type	asmminusp, @function
asmminusp:
.LFB37:
	xorl	%ecx, %ecx
	xorl	%r8d, %r8d
	.p2align 4,,7
.L86:
	addq	(%rdx,%r8,8), %rcx
	jo overasmminusp
	leaq	1(%r8), %rax
	cmpq	%rdi, %rax
	movq	%rcx, (%rsi,%r8,8)
	je	.L85
	subq	(%rdx,%rax,8), %rcx
	jo overasmminusp
	addq	$2, %r8
	cmpq	%rdi, %r8
	movq	%rcx, (%rsi,%rax,8)
	jl	.L86
.L85:
	xorl	%eax, %eax
	ret
overasmminusp:
	movl $50,%eax
	ret
.LFE37:
	.size	asmminusp, .-asmminusp
	.p2align 4,,15
.globl asmtymesp
	.type	asmtymesp, @function
asmtymesp:
.LFB38:
	movl	$1, %ecx
	xorl	%eax, %eax
	.p2align 4,,7
.L92:
	imulq	(%rdx,%rax,8), %rcx
	jo overasmtymesp
	movq	%rcx, (%rsi,%rax,8)
	incq	%rax
	cmpq	%rdi, %rax
	jl	.L92
	xorl	%eax, %eax
	ret
overasmtymesp:
	movl $50,%eax
	ret
.LFE38:
	.size	asmtymesp, .-asmtymesp
	.p2align 4,,15
.globl asmpluss
	.type	asmpluss, @function
asmpluss:
.LFB39:
	xorl	%eax, %eax
	.p2align 4,,7
.L98:
	decq	%rdi
	addq	(%rdx,%rdi,8), %rax
	jo overasmpluss
	testq	%rdi, %rdi
	movq	%rax, (%rsi,%rdi,8)
	jg	.L98
	xorl	%eax, %eax
	ret
overasmpluss:
	movl $50,%eax
	ret
.LFE39:
	.size	asmpluss, .-asmpluss
	.p2align 4,,15
.globl asmminuss
	.type	asmminuss, @function
asmminuss:
.LFB40:
	xorl	%ecx, %ecx
	.p2align 4,,7
.L104:
	decq	%rdi
	movq	(%rdx,%rdi,8), %r8
	subq	%rcx, %r8
	jo overasmminuss
	testq	%rdi, %rdi
	movq	%r8, %rcx
	movq	%rcx, (%rsi,%rdi,8)
	jg	.L104
	xorl	%eax, %eax
	ret
overasmminuss:
	movl $50,%eax
	ret
.LFE40:
	.size	asmminuss, .-asmminuss
	.p2align 4,,15
.globl asmtymess
	.type	asmtymess, @function
asmtymess:
.LFB41:
	movl	$1, %eax
	.p2align 4,,7
.L110:
	decq	%rdi
	imulq	(%rdx,%rdi,8), %rax
	jo overasmtymess
	testq	%rdi, %rdi
	movq	%rax, (%rsi,%rdi,8)
	jg	.L110
	xorl	%eax, %eax
	ret
overasmtymess:
	movl $50,%eax
	ret
.LFE41:
	.size	asmtymess, .-asmtymess
	.p2align 4,,15
.globl asminnerprod
	.type	asminnerprod, @function
asminnerprod:
.LFB42:
	testq	%rdi, %rdi
	je	.L115
	movq	$0, (%rsi)
	.p2align 4,,7
.L117:
	decq	%rdi
	movq	%rdi, %r10
	imulq	%r8, %r10
	movq	(%rcx,%r10,8), %r9
	imulq	(%rdx,%rdi,8), %r9
	jo overasminnerprod
	addq	(%rsi), %r9
	jo overasminnerprod
	testq	%rdi, %rdi
	movq	%r9, (%rsi)
	jg	.L117
.L115:
	xorl	%eax, %eax
	ret
overasminnerprod:
	movl $50,%eax
	ret
.LFE42:
	.size	asminnerprod, .-asminnerprod
	.p2align 4,,15
.globl asminnerprodx
	.type	asminnerprodx, @function
asminnerprodx:
.LFB43:
	movq	$-1, %r8
	jmp	.L122
	.p2align 4,,7
.L126:
	movq	%rdx, %r9
	imulq	(%rcx,%r8,8), %r9
	jo overasminnerprodx
	addq	(%rsi,%r8,8), %r9
	jo overasminnerprodx
	movq	%r9, (%rsi,%r8,8)
.L122:
	incq	%r8
	cmpq	%rdi, %r8
	jne	.L126
	xorl	%eax, %eax
	ret
overasminnerprodx:
	movl $50,%eax
	ret
.LFE43:
	.size	asminnerprodx, .-asminnerprodx
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
	.quad	.LFB23
	.quad	.LFE23-.LFB23
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.quad	.LFB24
	.quad	.LFE24-.LFB24
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.quad	.LFB25
	.quad	.LFE25-.LFB25
	.align 8
.LEFDE5:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.quad	.LFB26
	.quad	.LFE26-.LFB26
	.align 8
.LEFDE7:
.LSFDE9:
	.long	.LEFDE9-.LASFDE9
.LASFDE9:
	.long	.LASFDE9-.Lframe1
	.quad	.LFB27
	.quad	.LFE27-.LFB27
	.align 8
.LEFDE9:
.LSFDE11:
	.long	.LEFDE11-.LASFDE11
.LASFDE11:
	.long	.LASFDE11-.Lframe1
	.quad	.LFB28
	.quad	.LFE28-.LFB28
	.align 8
.LEFDE11:
.LSFDE13:
	.long	.LEFDE13-.LASFDE13
.LASFDE13:
	.long	.LASFDE13-.Lframe1
	.quad	.LFB29
	.quad	.LFE29-.LFB29
	.align 8
.LEFDE13:
.LSFDE15:
	.long	.LEFDE15-.LASFDE15
.LASFDE15:
	.long	.LASFDE15-.Lframe1
	.quad	.LFB30
	.quad	.LFE30-.LFB30
	.align 8
.LEFDE15:
.LSFDE17:
	.long	.LEFDE17-.LASFDE17
.LASFDE17:
	.long	.LASFDE17-.Lframe1
	.quad	.LFB31
	.quad	.LFE31-.LFB31
	.align 8
.LEFDE17:
.LSFDE19:
	.long	.LEFDE19-.LASFDE19
.LASFDE19:
	.long	.LASFDE19-.Lframe1
	.quad	.LFB32
	.quad	.LFE32-.LFB32
	.align 8
.LEFDE19:
.LSFDE21:
	.long	.LEFDE21-.LASFDE21
.LASFDE21:
	.long	.LASFDE21-.Lframe1
	.quad	.LFB33
	.quad	.LFE33-.LFB33
	.align 8
.LEFDE21:
.LSFDE23:
	.long	.LEFDE23-.LASFDE23
.LASFDE23:
	.long	.LASFDE23-.Lframe1
	.quad	.LFB34
	.quad	.LFE34-.LFB34
	.align 8
.LEFDE23:
.LSFDE25:
	.long	.LEFDE25-.LASFDE25
.LASFDE25:
	.long	.LASFDE25-.Lframe1
	.quad	.LFB35
	.quad	.LFE35-.LFB35
	.align 8
.LEFDE25:
.LSFDE27:
	.long	.LEFDE27-.LASFDE27
.LASFDE27:
	.long	.LASFDE27-.Lframe1
	.quad	.LFB36
	.quad	.LFE36-.LFB36
	.align 8
.LEFDE27:
.LSFDE29:
	.long	.LEFDE29-.LASFDE29
.LASFDE29:
	.long	.LASFDE29-.Lframe1
	.quad	.LFB37
	.quad	.LFE37-.LFB37
	.align 8
.LEFDE29:
.LSFDE31:
	.long	.LEFDE31-.LASFDE31
.LASFDE31:
	.long	.LASFDE31-.Lframe1
	.quad	.LFB38
	.quad	.LFE38-.LFB38
	.align 8
.LEFDE31:
.LSFDE33:
	.long	.LEFDE33-.LASFDE33
.LASFDE33:
	.long	.LASFDE33-.Lframe1
	.quad	.LFB39
	.quad	.LFE39-.LFB39
	.align 8
.LEFDE33:
.LSFDE35:
	.long	.LEFDE35-.LASFDE35
.LASFDE35:
	.long	.LASFDE35-.Lframe1
	.quad	.LFB40
	.quad	.LFE40-.LFB40
	.align 8
.LEFDE35:
.LSFDE37:
	.long	.LEFDE37-.LASFDE37
.LASFDE37:
	.long	.LASFDE37-.Lframe1
	.quad	.LFB41
	.quad	.LFE41-.LFB41
	.align 8
.LEFDE37:
.LSFDE39:
	.long	.LEFDE39-.LASFDE39
.LASFDE39:
	.long	.LASFDE39-.Lframe1
	.quad	.LFB42
	.quad	.LFE42-.LFB42
	.align 8
.LEFDE39:
.LSFDE41:
	.long	.LEFDE41-.LASFDE41
.LASFDE41:
	.long	.LASFDE41-.Lframe1
	.quad	.LFB43
	.quad	.LFE43-.LFB43
	.align 8
.LEFDE41:
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.3.3 (SuSE Linux)"
