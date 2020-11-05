add_library(j-openssl-sha OBJECT)
target_sources(j-openssl-sha PRIVATE
    md32_common.h
    md4.h
    md4_locl.h
    md5.h
    md5_locl.h
    openssl.h
    sha.h
    sha3.h
    sha_locl.h
    keccak1600.c
    md4_dgst.c
    md4_one.c
    md5_dgst.c
    md5_one.c
    openssl-util.c
    sha1_one.c
    sha256.c
    sha3.c
    sha512.c
)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^(aarch64|arm)")
    target_sources(j-openssl-sha PRIVATE
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/keccak1600-armv8-elf.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha1-armv8-elf.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha256-armv8-elf.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha512-armv8-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/keccak1600-armv4-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha1-armv4-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha256-armv4-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha512-armv4-elf.S>
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(j PRIVATE
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/keccak1600-x86_64-nasm.o>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/sha1-x86_64-nasm.o>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/sha256-x86_64-nasm.o>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/sha512-x86_64-nasm.o>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/keccak1600-mmx-nasm.o>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/sha1-586-nasm.o>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/sha256-586-nasm.o>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:../openssl-asm/sha512-586-nasm.o>
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_sources(j-openssl-sha PRIVATE
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/keccak1600-x86_64-elf.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha1-x86_64-elf.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha256-x86_64-elf.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha512-x86_64-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/keccak1600-mmx-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha1-586-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha256-586-elf.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha512-586-elf.S>
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_sources(j-openssl-sha PRIVATE
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/keccak1600-x86_64-macho.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha1-x86_64-macho.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha256-x86_64-macho.S>
        $<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:asm/sha512-x86_64-macho.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/keccak1600-mmx-macho.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha1-586-macho.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha256-586-macho.S>
        $<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:asm/sha512-586-macho.S>
    )
endif()
