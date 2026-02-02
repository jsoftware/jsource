#!/bin/sh

yasm -f win64 -p nasm -o keccak1600-x86_64-nasm.o keccak1600-x86_64-nasm.asm
yasm -f win64 -p nasm -o md5-x86_64-nasm.o md5-x86_64-nasm.asm
yasm -f win64 -p nasm -o sha1-x86_64-nasm.o sha1-x86_64-nasm.asm
yasm -f win64 -p nasm -o sha256-x86_64-nasm.o sha256-x86_64-nasm.asm
yasm -f win64 -p nasm -o sha512-x86_64-nasm.o sha512-x86_64-nasm.asm

yasm -f win32 -p nasm -o keccak1600-mmx-nasm.o keccak1600-mmx-nasm.asm
yasm -f win32 -p nasm -o md5-586-nasm.o md5-586-nasm.asm
yasm -f win32 -p nasm -o sha1-586-nasm.o sha1-586-nasm.asm
yasm -f win32 -p nasm -o sha256-586-nasm.o sha256-586-nasm.asm
yasm -f win32 -p nasm -o sha512-586-nasm.o sha512-586-nasm.asm
