#!/bin/sh

# keccak1600-x86_64.pl sha1-x86_64.pl sha512-x86_64.pl

perl keccak1600-x86_64.pl elf keccak1600-x86_64-elf.S
perl keccak1600-x86_64.pl macosx keccak1600-x86_64-macho.S
perl keccak1600-x86_64.pl masm keccak1600-x86_64-masm.asm
perl keccak1600-x86_64.pl nasm keccak1600-x86_64-nasm.asm

perl sha1-x86_64.pl elf sha1-x86_64-elf.S
perl sha1-x86_64.pl macosx sha1-x86_64-macho.S
perl sha1-x86_64.pl masm sha1-x86_64-masm.asm
perl sha1-x86_64.pl nasm sha1-x86_64-nasm.asm

perl sha512-x86_64.pl elf sha512-x86_64-elf.S
perl sha512-x86_64.pl macosx sha512-x86_64-macho.S
perl sha512-x86_64.pl masm sha512-x86_64-masm.asm
perl sha512-x86_64.pl nasm sha512-x86_64-nasm.asm
perl sha512-x86_64.pl elf sha256-x86_64-elf.S
perl sha512-x86_64.pl macosx sha256-x86_64-macho.S
perl sha512-x86_64.pl masm sha256-x86_64-masm.asm
perl sha512-x86_64.pl nasm sha256-x86_64-nasm.asm

# keccak1600-mmx.pl sha1-586.pl sha256-586.pl sha512-586.pl

perl keccak1600-mmx.pl elf keccak1600-mmx-elf.S
perl keccak1600-mmx.pl macosx keccak1600-mmx-macho.S
perl keccak1600-mmx.pl win32 keccak1600-mmx-masm.asm
perl keccak1600-mmx.pl win32n keccak1600-mmx-nasm.asm

perl sha1-586.pl elf sha1-586-elf.S
perl sha1-586.pl macosx sha1-586-macho.S
perl sha1-586.pl win32 sha1-586-masm.asm
perl sha1-586.pl win32n sha1-586-nasm.asm

perl sha256-586.pl elf sha256-586-elf.S
perl sha256-586.pl macosx sha256-586-macho.S
perl sha256-586.pl win32 sha256-586-masm.asm
perl sha256-586.pl win32n sha256-586-nasm.asm

perl sha512-586.pl elf sha512-586-elf.S
perl sha512-586.pl macosx sha512-586-macho.S
perl sha512-586.pl win32 sha512-586-masm.asm
perl sha512-586.pl win32n sha512-586-nasm.asm

# keccak1600-armv8.pl sha1-armv8.pl sha512-armv8.pl

perl keccak1600-armv8.pl linux keccak1600-armv8-elf.S
perl keccak1600-armv8.pl ios64 keccak1600-armv8-ios.S

perl sha1-armv8.pl linux sha1-armv8-elf.S
perl sha1-armv8.pl ios64 sha1-armv8-ios.S

perl sha512-armv8.pl linux sha512-armv8-elf.S
perl sha512-armv8.pl ios64 sha512-armv8-ios.S

perl sha512-armv8.pl linux sha256-armv8-elf.S
perl sha512-armv8.pl ios64 sha256-armv8-ios.S

# keccak1600-armv4.pl sha1-armv4-large.pl sha256-armv4.pl sha512-armv4.pl

perl keccak1600-armv4.pl linux keccak1600-armv4-elf.S

perl sha1-armv4-large.pl linux sha1-armv4-elf.S

perl sha256-armv4.pl linux sha256-armv4-elf.S

perl sha512-armv4.pl linux sha512-armv4-elf.S

