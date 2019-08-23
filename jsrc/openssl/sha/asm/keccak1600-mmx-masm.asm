IF @Version LT 800
ECHO MASM version 8.00 or later is strongly recommended.
ENDIF
.686
.XMM
IF @Version LT 800
XMMWORD STRUCT 16
DQ	2 dup (?)
XMMWORD	ENDS
ENDIF

.MODEL	FLAT
OPTION	DOTNAME
IF @Version LT 800
.text$	SEGMENT PAGE 'CODE'
ELSE
.text$	SEGMENT ALIGN(64) 'CODE'
ENDIF
ALIGN	16
__KeccakF1600	PROC PRIVATE
	movq	mm0,QWORD PTR 60[esi]
	movq	mm1,QWORD PTR 68[esi]
	movq	mm2,QWORD PTR 76[esi]
	movq	mm3,QWORD PTR 84[esi]
	movq	mm4,QWORD PTR 92[esi]
	mov	ecx,24
	jmp	$L000loop
ALIGN	16
$L000loop:
	pxor	mm0,QWORD PTR [esi-100]
	pxor	mm1,QWORD PTR [esi-92]
	pxor	mm2,QWORD PTR [esi-84]
	pxor	mm3,QWORD PTR [esi-76]
	pxor	mm4,QWORD PTR [esi-68]
	pxor	mm0,QWORD PTR [esi-60]
	pxor	mm1,QWORD PTR [esi-52]
	pxor	mm2,QWORD PTR [esi-44]
	pxor	mm3,QWORD PTR [esi-36]
	pxor	mm4,QWORD PTR [esi-28]
	pxor	mm0,QWORD PTR [esi-20]
	pxor	mm1,QWORD PTR [esi-12]
	pxor	mm2,QWORD PTR [esi-4]
	pxor	mm3,QWORD PTR 4[esi]
	pxor	mm4,QWORD PTR 12[esi]
	pxor	mm2,QWORD PTR 36[esi]
	pxor	mm0,QWORD PTR 20[esi]
	pxor	mm1,QWORD PTR 28[esi]
	pxor	mm3,QWORD PTR 44[esi]
	movq	mm5,mm2
	pxor	mm4,QWORD PTR 52[esi]
	movq	mm7,mm2
	psrlq	mm5,63
	movq	mm6,mm0
	psllq	mm7,1
	pxor	mm5,mm0
	psrlq	mm0,63
	pxor	mm5,mm7
	psllq	mm6,1
	movq	mm7,mm1
	movq	QWORD PTR 12[esp],mm5
	pxor	mm6,mm0
	psrlq	mm7,63
	pxor	mm6,mm3
	movq	mm0,mm1
	movq	QWORD PTR 36[esp],mm6
	psllq	mm0,1
	pxor	mm7,mm4
	pxor	mm0,mm7
	movq	mm7,mm3
	psrlq	mm3,63
	movq	QWORD PTR 4[esp],mm0
	psllq	mm7,1
	movq	mm5,mm4
	psrlq	mm4,63
	pxor	mm1,mm3
	psllq	mm5,1
	pxor	mm1,mm7
	pxor	mm2,mm4
	movq	QWORD PTR 20[esp],mm1
	pxor	mm2,mm5
	movq	mm3,QWORD PTR 44[esi]
	movq	QWORD PTR 28[esp],mm2
	pxor	mm3,mm2
	movq	mm4,QWORD PTR 92[esi]
	movq	mm7,mm3
	psrlq	mm3,43
	pxor	mm4,mm6
	psllq	mm7,21
	movq	mm6,mm4
	psrlq	mm4,50
	por	mm3,mm7
	psllq	mm6,14
	movq	mm2,QWORD PTR [esi-4]
	por	mm4,mm6
	pxor	mm2,mm1
	movq	mm1,QWORD PTR [esi-52]
	movq	mm6,mm2
	psrlq	mm2,21
	pxor	mm1,QWORD PTR 12[esp]
	psllq	mm6,43
	movq	mm7,mm1
	psrlq	mm1,20
	por	mm2,mm6
	psllq	mm7,44
	pxor	mm0,QWORD PTR [esi-100]
	por	mm1,mm7
	movq	mm5,mm1
	movq	mm6,mm2
	pandn	mm5,mm2
	pandn	mm2,mm3
	pxor	mm5,mm0
	pxor	mm2,mm1
	pxor	mm5,QWORD PTR [ebx]
	lea	ebx,DWORD PTR 8[ebx]
	movq	mm7,mm3
	movq	QWORD PTR [edi-100],mm5
	movq	mm5,mm4
	pandn	mm3,mm4
	pandn	mm4,mm0
	pxor	mm3,mm6
	movq	QWORD PTR [edi-92],mm2
	pxor	mm4,mm7
	movq	mm7,QWORD PTR [esi-76]
	movq	QWORD PTR [edi-84],mm3
	pandn	mm0,mm1
	movq	QWORD PTR [edi-76],mm4
	pxor	mm0,mm5
	pxor	mm7,QWORD PTR 28[esp]
	movq	QWORD PTR [edi-68],mm0
	movq	mm0,mm7
	psrlq	mm7,36
	movq	mm1,QWORD PTR [esi-28]
	psllq	mm0,28
	pxor	mm1,QWORD PTR 36[esp]
	por	mm0,mm7
	movq	mm6,mm1
	psrlq	mm1,44
	movq	mm2,QWORD PTR [esi-20]
	psllq	mm6,20
	pxor	mm2,QWORD PTR 4[esp]
	por	mm1,mm6
	movq	mm7,mm2
	psrlq	mm2,61
	movq	mm3,QWORD PTR 28[esi]
	psllq	mm7,3
	pxor	mm3,QWORD PTR 12[esp]
	por	mm2,mm7
	movq	mm5,mm3
	psrlq	mm3,19
	movq	mm4,QWORD PTR 76[esi]
	psllq	mm5,45
	pxor	mm4,QWORD PTR 20[esp]
	por	mm3,mm5
	movq	mm6,mm4
	psrlq	mm4,3
	psllq	mm6,61
	por	mm4,mm6
	movq	mm5,mm1
	movq	mm6,mm2
	pandn	mm5,mm2
	pandn	mm2,mm3
	pxor	mm5,mm0
	pxor	mm2,mm1
	movq	mm7,mm3
	movq	QWORD PTR [edi-60],mm5
	movq	mm5,mm4
	pandn	mm3,mm4
	pandn	mm4,mm0
	pxor	mm3,mm6
	movq	QWORD PTR [edi-52],mm2
	pxor	mm4,mm7
	movq	mm7,QWORD PTR [esi-92]
	movq	QWORD PTR [edi-44],mm3
	pandn	mm0,mm1
	movq	QWORD PTR [edi-36],mm4
	pxor	mm0,mm5
	pxor	mm7,QWORD PTR 12[esp]
	movq	QWORD PTR [edi-28],mm0
	movq	mm0,mm7
	psrlq	mm7,63
	movq	mm1,QWORD PTR [esi-44]
	psllq	mm0,1
	pxor	mm1,QWORD PTR 20[esp]
	por	mm0,mm7
	movq	mm6,mm1
	psrlq	mm1,58
	movq	mm2,QWORD PTR 4[esi]
	psllq	mm6,6
	pxor	mm2,QWORD PTR 28[esp]
	por	mm1,mm6
	movq	mm7,mm2
	psrlq	mm2,39
	movq	mm3,QWORD PTR 52[esi]
	psllq	mm7,25
	pxor	mm3,QWORD PTR 36[esp]
	por	mm2,mm7
	movq	mm5,mm3
	psrlq	mm3,56
	movq	mm4,QWORD PTR 60[esi]
	psllq	mm5,8
	pxor	mm4,QWORD PTR 4[esp]
	por	mm3,mm5
	movq	mm6,mm4
	psrlq	mm4,46
	psllq	mm6,18
	por	mm4,mm6
	movq	mm5,mm1
	movq	mm6,mm2
	pandn	mm5,mm2
	pandn	mm2,mm3
	pxor	mm5,mm0
	pxor	mm2,mm1
	movq	mm7,mm3
	movq	QWORD PTR [edi-20],mm5
	movq	mm5,mm4
	pandn	mm3,mm4
	pandn	mm4,mm0
	pxor	mm3,mm6
	movq	QWORD PTR [edi-12],mm2
	pxor	mm4,mm7
	movq	mm7,QWORD PTR [esi-68]
	movq	QWORD PTR [edi-4],mm3
	pandn	mm0,mm1
	movq	QWORD PTR 4[edi],mm4
	pxor	mm0,mm5
	pxor	mm7,QWORD PTR 36[esp]
	movq	QWORD PTR 12[edi],mm0
	movq	mm0,mm7
	psrlq	mm7,37
	movq	mm1,QWORD PTR [esi-60]
	psllq	mm0,27
	pxor	mm1,QWORD PTR 4[esp]
	por	mm0,mm7
	movq	mm6,mm1
	psrlq	mm1,28
	movq	mm2,QWORD PTR [esi-12]
	psllq	mm6,36
	pxor	mm2,QWORD PTR 12[esp]
	por	mm1,mm6
	movq	mm7,mm2
	psrlq	mm2,54
	movq	mm3,QWORD PTR 36[esi]
	psllq	mm7,10
	pxor	mm3,QWORD PTR 20[esp]
	por	mm2,mm7
	movq	mm5,mm3
	psrlq	mm3,49
	movq	mm4,QWORD PTR 84[esi]
	psllq	mm5,15
	pxor	mm4,QWORD PTR 28[esp]
	por	mm3,mm5
	movq	mm6,mm4
	psrlq	mm4,8
	psllq	mm6,56
	por	mm4,mm6
	movq	mm5,mm1
	movq	mm6,mm2
	pandn	mm5,mm2
	pandn	mm2,mm3
	pxor	mm5,mm0
	pxor	mm2,mm1
	movq	mm7,mm3
	movq	QWORD PTR 20[edi],mm5
	movq	mm5,mm4
	pandn	mm3,mm4
	pandn	mm4,mm0
	pxor	mm3,mm6
	movq	QWORD PTR 28[edi],mm2
	pxor	mm4,mm7
	movq	mm7,QWORD PTR [esi-84]
	movq	QWORD PTR 36[edi],mm3
	pandn	mm0,mm1
	movq	QWORD PTR 44[edi],mm4
	pxor	mm0,mm5
	pxor	mm7,QWORD PTR 20[esp]
	movq	QWORD PTR 52[edi],mm0
	movq	mm0,mm7
	psrlq	mm7,2
	movq	mm1,QWORD PTR [esi-36]
	psllq	mm0,62
	pxor	mm1,QWORD PTR 28[esp]
	por	mm0,mm7
	movq	mm6,mm1
	psrlq	mm1,9
	movq	mm2,QWORD PTR 12[esi]
	psllq	mm6,55
	pxor	mm2,QWORD PTR 36[esp]
	por	mm1,mm6
	movq	mm7,mm2
	psrlq	mm2,25
	movq	mm3,QWORD PTR 20[esi]
	psllq	mm7,39
	pxor	mm3,QWORD PTR 4[esp]
	por	mm2,mm7
	movq	mm5,mm3
	psrlq	mm3,23
	movq	mm4,QWORD PTR 68[esi]
	psllq	mm5,41
	pxor	mm4,QWORD PTR 12[esp]
	por	mm3,mm5
	movq	mm6,mm4
	psrlq	mm4,62
	psllq	mm6,2
	por	mm4,mm6
	movq	mm5,mm0
	xor	edi,esi
	movq	QWORD PTR 12[esp],mm1
	xor	esi,edi
	xor	edi,esi
	movq	mm6,mm1
	movq	mm7,mm2
	pandn	mm6,mm2
	pandn	mm7,mm3
	pxor	mm0,mm6
	pxor	mm1,mm7
	movq	mm6,mm3
	movq	QWORD PTR 60[esi],mm0
	pandn	mm6,mm4
	movq	QWORD PTR 68[esi],mm1
	pxor	mm2,mm6
	movq	mm7,mm4
	movq	QWORD PTR 76[esi],mm2
	pandn	mm7,mm5
	pandn	mm5,QWORD PTR 12[esp]
	pxor	mm3,mm7
	pxor	mm4,mm5
	movq	QWORD PTR 84[esi],mm3
	sub	ecx,1
	movq	QWORD PTR 92[esi],mm4
	jnz	$L000loop
	lea	ebx,DWORD PTR [ebx-192]
	ret
__KeccakF1600 ENDP
ALIGN	16
_KeccakF1600	PROC PUBLIC
$L_KeccakF1600_begin::
	push	ebp
	push	ebx
	push	esi
	push	edi
	mov	esi,DWORD PTR 20[esp]
	mov	ebp,esp
	sub	esp,240
	call	$L001pic_point
$L001pic_point:
	pop	ebx
	lea	ebx,DWORD PTR ($Liotas-$L001pic_point)[ebx]
	and	esp,-8
	lea	esi,DWORD PTR 100[esi]
	lea	edi,DWORD PTR 140[esp]
	call	__KeccakF1600
	mov	esp,ebp
	emms
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
_KeccakF1600 ENDP
ALIGN	16
_SHA3_absorb	PROC PUBLIC
$L_SHA3_absorb_begin::
	push	ebp
	push	ebx
	push	esi
	push	edi
	mov	esi,DWORD PTR 20[esp]
	mov	eax,DWORD PTR 24[esp]
	mov	ecx,DWORD PTR 28[esp]
	mov	edx,DWORD PTR 32[esp]
	mov	ebp,esp
	sub	esp,248
	call	$L002pic_point
$L002pic_point:
	pop	ebx
	lea	ebx,DWORD PTR ($Liotas-$L002pic_point)[ebx]
	and	esp,-8
	mov	edi,esi
	lea	esi,DWORD PTR 100[esi]
	mov	DWORD PTR [ebp-4],edx
	jmp	$L003loop
ALIGN	16
$L003loop:
	cmp	ecx,edx
	jc	$L004absorbed
	shr	edx,3
$L005block:
	movq	mm0,QWORD PTR [eax]
	lea	eax,DWORD PTR 8[eax]
	pxor	mm0,QWORD PTR [edi]
	lea	edi,DWORD PTR 8[edi]
	sub	ecx,8
	movq	QWORD PTR [edi-8],mm0
	dec	edx
	jnz	$L005block
	lea	edi,DWORD PTR 140[esp]
	mov	DWORD PTR [ebp-8],ecx
	call	__KeccakF1600
	mov	ecx,DWORD PTR [ebp-8]
	mov	edx,DWORD PTR [ebp-4]
	lea	edi,DWORD PTR [esi-100]
	jmp	$L003loop
ALIGN	16
$L004absorbed:
	mov	eax,ecx
	mov	esp,ebp
	emms
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
_SHA3_absorb ENDP
ALIGN	16
_SHA3_squeeze	PROC PUBLIC
$L_SHA3_squeeze_begin::
	push	ebp
	push	ebx
	push	esi
	push	edi
	mov	esi,DWORD PTR 20[esp]
	mov	eax,DWORD PTR 24[esp]
	mov	ecx,DWORD PTR 28[esp]
	mov	edx,DWORD PTR 32[esp]
	mov	ebp,esp
	sub	esp,248
	call	$L006pic_point
$L006pic_point:
	pop	ebx
	lea	ebx,DWORD PTR ($Liotas-$L006pic_point)[ebx]
	and	esp,-8
	shr	edx,3
	mov	edi,esi
	lea	esi,DWORD PTR 100[esi]
	mov	DWORD PTR [ebp-4],edx
	jmp	$L007loop
ALIGN	16
$L007loop:
	cmp	ecx,8
	jc	$L008tail
	movq	mm0,QWORD PTR [edi]
	lea	edi,DWORD PTR 8[edi]
	movq	QWORD PTR [eax],mm0
	lea	eax,DWORD PTR 8[eax]
	sub	ecx,8
	jz	$L009done
	dec	edx
	jnz	$L007loop
	lea	edi,DWORD PTR 140[esp]
	mov	DWORD PTR [ebp-8],ecx
	call	__KeccakF1600
	mov	ecx,DWORD PTR [ebp-8]
	mov	edx,DWORD PTR [ebp-4]
	lea	edi,DWORD PTR [esi-100]
	jmp	$L007loop
ALIGN	16
$L008tail:
	mov	esi,edi
	mov	edi,eax
DD	0xA4F39066
$L009done:
	mov	esp,ebp
	emms
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
_SHA3_squeeze ENDP
ALIGN	32
$Liotas::
DD	1,0
DD	32898,0
DD	32906,2147483648
DD	2147516416,2147483648
DD	32907,0
DD	2147483649,0
DD	2147516545,2147483648
DD	32777,2147483648
DD	138,0
DD	136,0
DD	2147516425,0
DD	2147483658,0
DD	2147516555,0
DD	139,2147483648
DD	32905,2147483648
DD	32771,2147483648
DD	32770,2147483648
DD	128,2147483648
DD	32778,0
DD	2147483658,2147483648
DD	2147516545,2147483648
DD	32896,2147483648
DD	2147483649,0
DD	2147516424,2147483648
DB	75,101,99,99,97,107,45,49,54,48,48,32,97,98,115,111
DB	114,98,32,97,110,100,32,115,113,117,101,101,122,101,32,102
DB	111,114,32,77,77,88,44,32,67,82,89,80,84,79,71,65
DB	77,83,32,98,121,32,60,97,112,112,114,111,64,111,112,101
DB	110,115,115,108,46,111,114,103,62,0
.text$	ENDS
END
