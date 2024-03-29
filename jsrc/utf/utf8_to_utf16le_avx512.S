#if !defined(__APPLE__)
	.section	.rodata
#endif
	.balign		64
	// identity permutation
.Lid:	.byte		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
	.byte		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	.byte		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
	.byte		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63

.Lc0:	.byte		0xc0, 0xc0, 0xc0, 0xc0
.Lf0:	.byte		0xf0, 0xf0, 0xf0, 0xf0
.Lc2:	.byte		0xc2, 0xc2, 0xc2, 0xc2

.Ld7c0d7c0:
	.short		0xd800 - 0x0040, 0xd800 - 0x0040
.L0400:	.short		0x0400, 0x0400
.Ld800:	.short		0xd800, 0xd800

	.text
#if !defined(__APPLE__) && !defined(_WIN32)
	.globl		utf8_to_utf16le_avx512
	.type		utf8_to_utf16le_avx512, @function
#elif defined(_WIN32)
	.globl		utf8_to_utf16le_avx512
#else
	.globl		_utf8_to_utf16le_avx512
#endif
	.balign		16
#if !defined(__APPLE__)
utf8_to_utf16le_avx512:
#else
_utf8_to_utf16le_avx512:
#endif
#ifdef _WIN32
	// Windows ABI adapter
	push		%rdi
	push		%rsi
	mov		%rcx, %rdi
	mov		%rdx, %rsi
	mov		%r8, %rdx
	mov		%r9, %rcx
#endif

	// rdi: outbuf
	// rsi: inbuf
	// rdx: inlen
	// rcx: &outlen

	// load constants and masks
	vpbroadcastd	.Lc0(%rip), %zmm30
	vpaddb		%zmm30, %zmm30, %zmm31	// 0x80, 0x80, 0x80, 0x80
	vpbroadcastd	.Lf0(%rip), %zmm28
	vpaddb		%zmm28, %zmm28, %zmm29	// 0xe0, 0xe0, 0xe0, 0xe0
	vpbroadcastd	.Lc2(%rip), %zmm20
	vmovdqa64	.Lid(%rip), %zmm27
	vpternlogd	$0xff, %zmm26, %zmm26, %zmm26 // ffffffff
	vpbroadcastd	.Ld7c0d7c0(%rip), %zmm24
	vpbroadcastd	.L0400(%rip), %zmm21
	vpbroadcastd	.Ld800(%rip), %zmm22
	vpaddw		%zmm21, %zmm21, %zmm23	// 0x0800, 0x0800
	vpaddw		%zmm21, %zmm22, %zmm25	// 0xdc00, 0xdc00

	push		%rbx			// stash callee-saved registers
	push		%r12

	push		%rdi			// stash output buffer for latter
	push		%rsi			// dito with input buffer
	push		%rcx			// and &outbuf

	mov		$0x5555555555555555, %rax
	kmovq		%rax, %k7		// the LSB of each word
	mov		$0x3fffffffffffffff, %rax
	kmovq		%rax, %k0		// all except the last two byte of the vector
	mov		$-1, %rbx		// the valid prefix of bits in the input

	cmp		$64, %rdx		// enough left for another iteration?
	jb		.Ltail

	.balign		16
.Loop:	vmovdqu8	(%rsi), %zmm0		// load 64 byte from input buffer
	vpcmpltub	%zmm31, %zmm0, %k1	// zmm0 < 0x80 (ASCII)
	kortestd	%k1, %k1		// all ASCII?
	jnc		1f			// if no, take slow path

	// Fast path 1: all ASCII characters
	vpmovzxbw	%ymm0, %zmm1		// convert to UTF-16LE
	vmovdqu16	%zmm1, (%rdi)		// and deposit in output buffer

	add		$32, %rsi		// advance source buffer
	sub		$32, %rdx		// apply processed input
	add		$64, %rdi		// advance output buffer
	cmp		$64, %rdx		// enough left for another iteration?
	jae		.Loop			// if yes, go again!
	jmp		.Ltail			// else handle tail

	// classify characters further
1:	vpcmpnltub	%zmm30, %zmm0, %k2	// 0xc0 <= zmm0 (2, 3, or 4 start bytes)
	vpcmpnltub	%zmm29, %zmm0, %k3	// 0xe0 <= zmm0 (3 or 4 byte start bytes)
	vpcmpltub	%zmm20, %zmm0, %k5{%k2}	// 0xc0 <= zmm0 < 0xc2 (illegal two byte sequence?)
	ktestq		%k5, %k5		// if present, encoding error!
	jnz		.Lfail1

	kmovq		%k2, %r12		// LEAD234
	ktestq		%k3, %k3		// any 3 or 4 byte sequences present?
	jnz		2f			// if yes, take slow path

	// Fast path 2: all ASCII or 2 byte
	korq		%k1, %k2, %k5		// LEAD: first bytes of each sequence
	kmovq		%k5, %r8		// LEAD bytes
	pext		%r8d, %r8d, %ecx	// one bit for each char in the first 32 byte of input
	kmovd		%ecx, %k1

	// Translate into UTF-8.  It's a bit simpler than in the general case.
	// --- CASE ---  -------------- INPUT --------------  ---- OUTPUT ----
	// ascii                                    0GFEDCBA  000000000GFEDCBA
	// 2 byte                          110LKJHG 10FEDCBA  00000LKJHGFEDCBA
	vpsubb		%zmm20, %zmm0, %zmm2{%k2}{z} // 2 byte sequence lead bytes with tag bits subtracted
	knotq		%k2, %k6		// FOLLOW: last bytes of each sequence
	vpcompressb	%zmm2, %zmm2{%k5}{z}	// the first byte of each sequence
	vpmovzxbw	%ymm2, %zmm2		// ... zero extended into words
	vpcompressb	%zmm0, %zmm1{%k6}{z}	// the last bytes of each sequence
	vpmovzxbw	%ymm1, %zmm1		// ... zero extended into words
	vpsllw		$6, %zmm2, %zmm2	// shifted into position
	vpaddw		%zmm2, %zmm1, %zmm1	// and added to the last byte

	bt		$31, %r12d		// does a 2 byte sequence straddle bytes 31/32?
	sbb		$32, %rdx		// if yes, we have processed 33 bytes, if no 32 bytes.

	// check if there is a sequencing error (i.e. lead byte not followed by follow byte)
	add		%r12, %r12		// where follow bytes should be
	not		%r12			// where the should not be
	cmp		%r12, %r8		// does that match reality?
	jne		.Lfail2			// if not, I've got bad news for you

	vmovdqu16	%zmm1, (%rdi){%k1}	// deposit decoded text into output

	// advance pointers, counters, and iterate
	cmpb		$0xc0, 31(%rsi)		// does a 2 byte sequence straddle bytes 31/32?
	sbb		$-33, %rsi		// if yes, we have processed 33 bytes, if no 32 bytes.

	popcnt		%r8d, %r8d		// number of characters converted in 32/33 byte of input
	lea		(%rdi, %r8, 2), %rdi	// advance output by that many words

	cmp		$64, %rdx		// enough for another round?
	jae		.Loop			// if yes, go again!
	jmp		.Ltail			// else handle tail

	.balign		16
.Ltailiteration:
2:	vpcmpnltub	%zmm28, %zmm0, %k4	// 0xf0 <= zmm0 (4 byte start bytes)

	// move masks into scalar registers to play with
	kmovq		%k1, %r8		// ASCII
	kmovq		%k3, %r9		// LEAD34
	or		%r12, %r8		// LEAD: UTF-8 first bytes

	ktestq		%k4, %k4		// any surrogates present?
	jnz		3f			// if yes, go to default path

	// Fast path 3: no surrogates

	// compute the various bit masks we need to assembly the UTF-16 characters
	// of note: we will later only process Unicode characters that start no later
	// than 5 bytes before the end of the input.  LAST may contain wrong data for
	// the last 4 bytes of input, but we don't care about that a whole lot.
	mov		%r8, %rax
	mov		%r9, %r10		// LEAD3: lead bytes for 3 byte sequences
	shr		$1, %rax		// END: the last byte of each sequence

	// compute permutation mask and load the last bytes
	knotq		%k1, %k5		// not ASCII bytes
	kmovq		%rax, %k6
	vpcompressb	%zmm27, %zmm7{%k6}{z}	// the indices of the input bytes marked in END
	vpmovzxbw	%ymm7, %zmm7		// ... zero expanded into words
	vmovdqu8	%zmm30, %zmm6{%k5}{z}	// ASCII: 00000000  other: 11000000
	vpandnd		%zmm0, %zmm6, %zmm0	// high two bits cleared where not ASCII
	vpermb		%zmm0, %zmm7, %zmm1{%k7}{z} // the last byte of each character

	// check if there is an encoding error
	// check for correct character sequencing and validity
	// we don't need to check for 0xf8 <= zmm0 here: if this
	// occurs, we decode it as a 4 byte sequence which will
	// encode a character > U+10FFFF and is rejected later on
	shl		$2, %r9			// where follow bytes two bytes after a lead byte should be
	add		%r12, %r12		// where follow bytes directly after a lead byte should be
	or		%r9, %r12		// where we expect follow bytes to be
	not		%r8			// FOLLOW: UTF-8 follow bytes
	cmp		%r12, %r8		// does that match reality?
	jne		.Lfail2			// if not, I've got bad news for you

	// add in second last bytes
	kshiftrq	$1, %k5, %k5		// bytes that precede non-ASCII bytes
	vpaddw		%zmm26, %zmm7, %zmm7	// indices of the second last bytes
	vmovdqu8	%zmm0, %zmm6{%k5}{z}	// only bytes that precede non-ASCII bytes
	vpermb		%zmm6, %zmm7, %zmm2{%k7}{z} // the second last bytes (of two, three byte seq, surrogates)
	vpsllw		$6, %zmm2, %zmm2	// shifted into position
	vpaddw		%zmm2, %zmm1, %zmm2	// and add to the last bytes

	// add in third last bytes
	kandq		%k0, %k3, %k3		// bytes that could be third-last bytes (LEAD34 sans wrap around)
	vpaddw		%zmm26, %zmm7, %zmm7	// indices of the third last bytes
	vmovdqu8	%zmm0, %zmm6{%k3}{z}	// only those that are the third last byte of a sequece
	vpermb		%zmm6, %zmm7, %zmm3{%k7}{z} // the third last bytes (of three byte sequences, hi surrogate)
	vpsllw		$12, %zmm3, %zmm3	// shifted into position
	vpaddw		%zmm3, %zmm2, %zmm1	// and added to the other bytes

	// now we have the following situation; the tag bits have already been cleared.
	// --- CASE ---  -------------- INPUT --------------  ---- OUTPUT ----
	// ascii                                    0GFEDCBA  000000000GFEDCBA
	// 2 byte                          110LKJHG 10FEDCBA  00000LKJHGFEDCBA
	// 3 byte                 1110RQPN 10MLKJHG 10FEDCBA  RQPNMLKJHGFEDCBA

	// compute the number characters in zmm1 and advance input buffer, count
	// if the last word is a high surrogate, discard it as we don't have code
	// to straddle surrogate pairs over a pair of input vectors
	mov		%rax, %r9		// LAST+3RD bytes
	and		%rbx, %r9		// those that are part of the input
	mov		$-1, %ecx		// 32 set bits
	pdep		%r9, %rcx, %r9		// the first up to 32 LAST bytes

	lzcnt		%r9, %rcx		// number of vector items that were not processed
	add		$64, %rsi		// advance source by full vector
	sub		%rcx, %rsi		// but not the parts that weren't processed
	lea		-64(%rdx, %rcx, 1), %rdx // process item count the same way
	popcnt		%r9, %r9		// number of words written out

	// post process surrogates and deposit.
	// Here we assume that each high surrogate is followed by a low surrogate
	bzhi		%r9d, %ebx, %r8d	// output words that hold meaningful characters
	kmovd		%r8d, %k4
	vmovdqu16	%zmm1, (%rdi){%k4}	// deposit decoded text into output
	lea		(%rdi, %r9, 2), %rdi	// advance output buffer by that many words

	// check 3 byte sequences for correctness
	shl		$2, %r10		// the end bytes of 3 byte sequences
	pext		%rax, %r10, %r10	// words in zmm1 that represent three byte sequences
	kmovd		%r10d, %k6

	vpcmpltuw	%zmm23, %zmm1, %k4{%k6}	// any 3 byte characters < 0x0800?
	vpsubw		%zmm22, %zmm1, %zmm2	// zmm1 - 0xd800
	vpcmpltuw	%zmm23, %zmm2, %k2{%k6}	// any unexpected surrogates?
	kortestd	%k4, %k2
	jnz		.Lfail3

	cmp		$64, %rdx		// enough for another round?
	jae		.Loop			// if yes, go again!
	jmp		.Ltail

	// Default path: surrogates are present

	// compute the various bit masks we need to assembly the UTF-16 characters
	// of note: we will later only process Unicode characters that start no later
	// than 5 bytes before the end of the input.  LAST may contain wrong data for
	// the last 4 bytes of input, but we don't care about that a whole lot.
	.balign		16
3:	kmovq		%k4, %r11		// LEAD4
	lea		(%r8, %r11, 8), %rax	// FIRST+4TH: the first and the fourth byte of each sequence
	andn		%r9, %r11, %r10		// LEAD3: lead bytes for 3 byte sequences
	shr		$1, %rax		// LAST+3RD: the last byte of each sequence and the third byte of each
						// ... 4 byte sequence, i.e. where we want to start to decode the input
	shl		$3, %r11		// where follow bytes three bytes after a lead byte should be
	or		%r11, %rax		// including for 4 byte sequences ending at position 63 (but not others)

	// compute permutation mask and load the last bytes
	knotq		%k1, %k5		// not ASCII bytes
	kmovq		%rax, %k6
	vpcompressb	%zmm27, %zmm7{%k6}{z}	// the indices of the input bytes marked in L+3RD
	vpmovzxbw	%ymm7, %zmm7		// ... zero expanded into words
	vmovdqu8	%zmm30, %zmm6{%k5}{z}	// ASCII: 00000000  other: 11000000
	vpandnd		%zmm0, %zmm6, %zmm0	// high two bits cleared where not ASCII
	vpermb		%zmm0, %zmm7, %zmm1{%k7}{z} // the last byte of each character

	// check if there is an encoding error
	// check for correct character sequencing and validity
	// we don't need to check for 0xf8 <= zmm0 here: if this
	// occurs, we decode it as a 4 byte sequence which will
	// encode a character > U+10FFFF and is rejected later on
	shl		$2, %r9			// where follow bytes two bytes after a lead byte should be
	add		%r12, %r12		// where follow bytes directly after a lead byte should be
	or		%r11, %r9
	or		%r9, %r12		// where we expect follow bytes to be
	not		%r8			// FOLLOW: UTF-8 follow bytes
	cmp		%r12, %r8		// does that match reality?
	jne		.Lfail2			// if not, I've got bad news for you

	// find location of surrogates
	pext		%rax, %r11, %rcx	// words in zmm1 that represent low surrogates
	kmovd		%ecx, %k2
	shr		$1, %rcx		// words in zmm1 that represent high surrogates
	kmovd		%ecx, %k1

	// add in second last bytes
	kshiftrq	$1, %k5, %k5		// bytes that precede non-ASCII bytes
	vpaddw		%zmm26, %zmm7, %zmm7	// indices of the second last bytes
	vmovdqu8	%zmm0, %zmm6{%k5}{z}	// only bytes that precede non-ASCII bytes
	vpermb		%zmm6, %zmm7, %zmm2{%k7}{z} // the second last bytes (of two, three byte seq, surrogates)
	vpsllw		$6, %zmm2, %zmm2	// shifted into position
	vpaddw		%zmm2, %zmm1, %zmm2	// and add to the last bytes

	// add in third last bytes
	kandq		%k0, %k3, %k3		// bytes that could be third-last bytes (LEAD34 sans wrap around)
	vpaddw		%zmm26, %zmm7, %zmm7	// indices of the third last bytes
	vmovdqu8	%zmm0, %zmm6{%k3}{z}	// only those that are the third last byte of a sequece
	vpermb		%zmm6, %zmm7, %zmm3{%k7}{z} // the third last bytes (of three byte sequences, hi surrogate)
	vpsllw		$12, %zmm3, %zmm3	// shifted into position
	vpaddw		%zmm3, %zmm2, %zmm1	// and added to the other bytes

	// now we have the following situation; the tag bits have already been cleared.
	// --- CASE ---  -------------- INPUT --------------  ---- OUTPUT ----
	// ascii                                    0GFEDCBA  000000000GFEDCBA
	// 2 byte                          110LKJHG 10FEDCBA  00000LKJHGFEDCBA
	// 3 byte                 1110RQPN 10MLKJHG 10FEDCBA  RQPNMLKJHGFEDCBA
	// hi surrogate  11110WVU 10TSRQPN 10MLKJHG           0WVUTSRQPNMLKJHG
	// lo surrogate                    10MLKJHG 10FEDCBA  0000MLKJHGFEDCBA

	// compute the number characters in zmm1 and advance input buffer, count
	// if the last word is a high surrogate, discard it as we don't have code
	// to straddle surrogate pairs over a pair of input vectors
	mov		%rax, %r9		// LAST+3RD bytes
	and		$0x80000000, %ecx	// 80000000 if last item is a high surrogate, 00000000 otherwise
	and		%rbx, %r9		// those that are part of the input
	not		%ecx			// 7fffffff if last item is a high surrogate, ffffffff otherwise
	pdep		%r9, %rcx, %r9		// the first up to 32 LAST+3RD byte such that
						// ... the last one is not a high surrogate

	lzcnt		%r9, %rcx		// number of vector items that were not processed
	add		$64, %rsi		// advance source by full vector
	sub		%rcx, %rsi		// but not the parts that weren't processed
	lea		-64(%rdx, %rcx, 1), %rdx // process item count the same way
	popcnt		%r9, %r9		// number of words written out

	// post process surrogates and deposit.
	// Here we assume that each high surrogate is followed by a low surrogate
	bzhi		%r9d, %ebx, %r8d	// output words that hold meaningful characters
	kmovd		%r8d, %k4
	vmovdqu16	%zmm25, %zmm2{%k2}{z}	// lo surr: 1101110000000000, other:  0000000000000000
	vpsrlw		$4, %zmm1, %zmm3	// hi surr: 00000WVUTSRQPNML  vuts = WVUTS - 1
	vpord		%zmm1, %zmm2, %zmm1	// lo surr: 110111KJHGFEDCBA, other:  unchanged
	vpaddw		%zmm24, %zmm3, %zmm1{%k1}// hi sur: 110110vutsRQPNML, other:  unchanged
	vmovdqu16	%zmm1, (%rdi){%k4}	// deposit decoded text into output
	lea		(%rdi, %r9, 2), %rdi	// advance output buffer by that many words

	// check 3 and 4 byte sequences for correctness
	shl		$2, %r10		// the end bytes of 3 byte sequences
	pext		%rax, %r10, %r10	// words in zmm1 that represent three byte sequences
	kmovd		%r10d, %k6

	vpcmpltuw	%zmm23, %zmm1, %k4{%k6}	// any 3 byte characters < 0x0800?
	vpsubw		%zmm22, %zmm1, %zmm2	// zmm1 - 0xd800
	vpcmpltuw	%zmm23, %zmm2, %k2{%k6}	// any unexpected surrogates?
	kord		%k4, %k2, %k2

	vpcmpnltuw	%zmm21, %zmm2, %k4{%k1}	// any botched high surrogates?  These occur when
						// char < U+10000 or > U+10FFFF or lead byte >F7
	kortestd	%k4, %k2
	jnz		.Lfail3

	cmp		$64, %rdx		// enough for another round?
	jae		.Loop			// if yes, go again!

.Ltail:	test		%rdx, %rdx		// any bytes left to process?
	jnz		3f			// if yes, deal with them

	// compute input/output length and finish
	pop		%rcx			// &outbuf
	pop		%r8			// original rsi (inbuf)
	pop		%r9			// original rdi (outbuf)

	sub		%r8, %rsi		// number of bytes processed
	sub		%r9, %rdi		// number of bytes written out
	shr		$1, %rdi		// number of words written out
	mov		%rsi, %rax
	mov		%rdi, (%rcx)

	pop		%r12
	pop		%rbx

#ifdef _WIN32
	// Windows ABI adapter
	pop		%rsi
	pop		%rdi
#endif

	vzeroupper
	ret

	// handle the remaining tail bytes
	.balign		16
3:	bzhi		%rdx, %rbx, %rbx	// set ebx to mask of bytes left in tail
	kmovq		%rbx, %k1
	vmovdqu8	(%rsi), %zmm0{%k1}{z}	// load input under mask
	vpcmpltub	%zmm31, %zmm0, %k1	// zmm0 < 0x80 (ASCII)
	vpcmpnltub	%zmm30, %zmm0, %k2	// 0xc0 <= zmm0 (2, 3, or 4 start bytes)
	vpcmpnltub	%zmm29, %zmm0, %k3	// 0xe0 <= zmm0 (3 or 4 byte start bytes)
	vpcmpltub	%zmm20, %zmm0, %k5{%k2}	// 0xc0 <= zmm0 < 0xc2 (illegal two byte sequence?)
	ktestq		%k5, %k5		// if present, encoding error!
	jnz		.Lfail1
	kmovq		%k2, %r12		// LEAD234
	jmp		.Ltailiteration

	// failure case: bytes C0 or C1 present (in K5)
.Lfail1:
	kmovq		%k5, %rdx
	tzcnt		%rdx, %rdx		// how many bytes in rdx are valid?
	jmp		.Ltail			// process these

	// failure case: mismatch in follow bytes between r8 and r12
.Lfail2:
	xor		%r8, %r12		// where did the mismatch occur?
	tzcnt		%r12, %rdx		// find location of first mismatch
	bt		%rdx, %r8		// is this because of a follow byte that is present but should not?
	jc		.Ltail			// if this is the case, rdx is how many bytes are still valid
	not		%r8			// LEAD bytes
	bzhi		%rdx, %r8, %rax		// lead bytes preceding the mismatch
	mov		$63, %edx
	lzcnt		%rax, %rax
	sub		%eax, %edx		// length of prefix before the lead byte of first missing follow byte
	jmp		.Ltail

	// overlong encoding, 5+ byte sequence, or surrogate encoded in K4|K2
	// invariant: the error can never be at a low surrogate
.Lfail3:
	sub		%r9, %rdi		// roll back rdi adjustment
	sub		%r9, %rdi
	lea		-64(%rsi, %rcx, 1), %rsi // roll back rsi adjustment
	kord		%k4, %k2, %k2		// where did errors occur?
	kmovd		%k2, %edx
	stc
	adc		%rax, %rax		// LEAD+4TH once again
	pdep		%rax, %rdx, %rdx	// which start byte does this error correspond to?
	tzcnt		%rdx, %rdx		// process input until just before that start byte
	jmp		.Ltail

#if !defined(__APPLE__) && !defined(_WIN32)
	.size utf8_to_utf16le_avx512, .-utf8_to_utf16le_avx512
#endif

// Compute the maximal output buffer size in words needed to transcode an UTF-8
// string into UTF-16LE.
	.globl utf8_to_utf16le_buflen_avx512
#if !defined(__APPLE__) && !defined(_WIN32)
	.type utf8_to_utf16le_buflen_avx512, @function
#endif
utf8_to_utf16le_buflen_avx512:
#ifndef _WIN32
	mov		%rdi, %rax		// does not write past the end of the buffer
#else
	mov		%rcx, %rax		// dito, but for Windows
#endif
	ret

#if !defined(__APPLE__) && !defined(_WIN32)
	.size utf8_to_utf16le_buflen_avx512, .-utf8_to_utf16le_buflen_avx512
#endif
