#!/bin/sh

perl md5-x86_64.pl elf md5-x86_64-elf.S
perl md5-x86_64.pl macosx md5-x86_64-macho.S
perl md5-x86_64.pl masm md5-x86_64-masm.asm
perl md5-x86_64.pl nasm md5-x86_64-nasm.asm

perl md5-aarch64.pl elf md5-aarch64-elf.S
perl md5-aarch64.pl ios64 md5-aarch64-ios.S

perl md5-586.pl elf md5-586-elf.S
perl md5-586.pl android md5-586-android.S
perl md5-586.pl macosx md5-586-macho.S
perl md5-586.pl win32 md5-586-masm.asm
perl md5-586.pl win32n md5-586-nasm.asm
