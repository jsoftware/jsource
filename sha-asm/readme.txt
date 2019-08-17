64-bit x86_64 cpu only

header:
/* ssse3 */
extern void sha1_update_intel(int *hash, const char* input, size_t num_blocks );;
/* avx */
extern void sha256_avx(void *input_data, uint32_t digest[8], uint64_t num_blks);
/* avx2 */
extern void sha256_rorx(void *input_data, uint32_t digest[8], uint64_t num_blks);
/* avx2 */
extern void sha256_rorx_x8ms(void *input_data, uint32_t digest[8], uint64_t num_blks);
/* sse4_1 */
extern void sha256_sse4(void *input_data, uint32_t digest[8], uint64_t num_blks);
/* avx */
extern void sha512_avx(const void* M, void* D, uint64_t L);
/* sse4_1 */
extern void sha512_sse4(const void* M, void* D, uint64_t L);

# linux object files
sha1_ssse3-elf64.o
sha256_avx1-elf64.o
sha256_avx2_rorx2-elf64.o
sha256_avx2_rorx8-elf64.o
sha256_sse4-elf64.o
sha512_avx-elf64.o
sha512_sse4-elf64.o

# mac object files
sha1_ssse3-macho64.o
sha256_avx1-macho64.o
sha256_avx2_rorx2-macho64.o
sha256_avx2_rorx8-macho64.o
sha256_sse4-macho64.o
sha512_avx-macho64.o
sha512_sse4-macho64.o

#windows object files
sha1_ssse3-x64.o
sha256_avx1-x64.o
sha256_avx2_rorx2-x64.o
sha256_avx2_rorx8-x64.o
sha256_sse4-x64.o
sha512_avx-x64.o
sha512_sse4-x64.o

sha1-ssse3
;   It implements both UNIX(default) and Windows ABIs, use -DWIN_ABI on Windows
;      extern "C" void sha1_update_intel(int *hash, const char* input, size_t num_blocks );
; Windows:  yasm -f x64 -D WINABI sha1_ssse3.asm
; Linux:    yasm -f elf64 sha1_ssse3.asm

sha256-avx1
; Windows:  yasm -Xvc -f x64 -rnasm -pnasm -o sha256_avx1.obj -g cv8 sha256_avx1.asm
; Linux:    yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_avx1.o sha256_avx1.asm
;; void sha256_avx(void *input_data, UINT32 digest[8], UINT64 num_blks)
;; arg 1 : pointer to input data
;; arg 2 : pointer to digest
;; arg 3 : Num blocks

sha256_avx2_rorx2
; Windows:  yasm -Xvc -f x64 -rnasm -pnasm -o sha256_avx2_rorx2.obj -g cv8 sha256_avx2_rorx2.asm
; Linux:    yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_avx2_rorx2.o sha256_avx2_rorx2.asm
; This code schedules 2 blocks at a time, with 4 lanes per block
;; void sha256_rorx(void *input_data, UINT32 digest[8], UINT64 num_blks)
;; arg 1 : pointer to input data
;; arg 2 : pointer to digest
;; arg 3 : Num blocks

sha256_avx2_rorx8
; Windows:  yasm -Xvc -f x64 -rnasm -pnasm -o sha256_avx2_rorx8.obj -g cv8 sha256_avx2_rorx8.asm
; Linux:    yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_avx2_rorx8.o sha256_avx2_rorx8.asm
; This code schedules 8 blocks at a time, with 1 lane per block
;; void sha256_rorx_x8ms(void *input_data, UINT32 digest[8], UINT64 num_blks)
;; arg 1 : pointer to input data
;; arg 2 : pointer to digest
;; arg 3 : Num blocks

sha256_sse4
; Windows:  yasm -Xvc -f x64 -rnasm -pnasm -o sha256_sse4.obj -g cv8 sha256_sse4.asm
; Linux:    yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_sse4.o sha256_sse4.asm
;; void sha256_sse4(void *input_data, UINT32 digest[8], UINT64 num_blks)
;; arg 1 : pointer to input data
;; arg 2 : pointer to digest
;; arg 3 : Num blocks

sha512_avx
; Windows:  yasm -f x64 -D WINABI sha512_avx.asm
; Linux:    yasm -f elf64 sha512_avx.asm
; void sha512_avx(const void* M, void* D, uint64_t L);
; Purpose: Updates the SHA512 digest stored at D with the message stored in M.
; The size of the message pointed to by M must be an integer multiple of SHA512
;   message blocks.
; L is the message length in SHA512 blocks

sha512_sse4
; Windows:  yasm -f x64 -D WINABI sha512_sse4.asm
; Linux:    yasm -f elf64 sha512_sse4.asm
; void sha512_sse4(const void* M, void* D, uint64_t L);
; Purpose: Updates the SHA512 digest stored at D with the message stored in M.
; The size of the message pointed to by M must be an integer multiple of SHA512
;   message blocks.
; L is the message length in SHA512 blocks.
