#!/bin/sh

# linux
yasm -f elf64 -X gnu -o sha1_ssse3-elf64.o sha1_ssse3.asm
yasm -f elf64 -X gnu -D LINUX -o sha256_avx1-elf64.o sha256_avx1.asm
yasm -f elf64 -X gnu -D LINUX -o sha256_avx2_rorx2-elf64.o sha256_avx2_rorx2.asm
yasm -f elf64 -X gnu -D LINUX -o sha256_avx2_rorx8-elf64.o sha256_avx2_rorx8.asm
yasm -f elf64 -X gnu -D LINUX -o sha256_sse4-elf64.o sha256_sse4.asm
yasm -f elf64 -X gnu -o sha512_avx-elf64.o sha512_avx.asm
yasm -f elf64 -X gnu -o sha512_sse4-elf64.o sha512_sse4.asm

# mac
yasm -f macho64 -X gnu -o sha1_ssse3-macho64.o sha1_ssse3.asm
yasm -f macho64 -X gnu -D LINUX -o sha256_avx1-macho64.o sha256_avx1.asm
yasm -f macho64 -X gnu -D LINUX -o sha256_avx2_rorx2-macho64.o sha256_avx2_rorx2.asm
yasm -f macho64 -X gnu -D LINUX -o sha256_avx2_rorx8-macho64.o sha256_avx2_rorx8.asm
yasm -f macho64 -X gnu -D LINUX -o sha256_sse4-macho64.o sha256_sse4.asm
yasm -f macho64 -X gnu -o sha512_avx-macho64.o sha512_avx.asm
yasm -f macho64 -X gnu -o sha512_sse4-macho64.o sha512_sse4.asm

# windows
yasm -f x64 -X gnu -D WIN_ABI -o sha1_ssse3-x64.o sha1_ssse3.asm
yasm -f x64 -X gnu -o sha256_avx1-x64.o sha256_avx1.asm
yasm -f x64 -X gnu -o sha256_avx2_rorx2-x64.o sha256_avx2_rorx2.asm
yasm -f x64 -X gnu -o sha256_avx2_rorx8-x64.o sha256_avx2_rorx8.asm
yasm -f x64 -X gnu -o sha256_sse4-x64.o sha256_sse4.asm
yasm -f x64 -X gnu -D WINABI -o sha512_avx-x64.o sha512_avx.asm
yasm -f x64 -X gnu -D WINABI -o sha512_sse4-x64.o sha512_sse4.asm
