#!/bin/sh
set -e

cd "$(dirname "$0")"
echo "entering `pwd`"

jplatform64=$(./jplatform64.sh)

if [ "" = "$CFLAGS" ]; then
 # OPTLEVEL will be merged back into CFLAGS, further down
	# OPTLEVEL is probably overly elaborate, but it works
 case "$_DEBUG" in
  3) OPTLEVEL=" -O2 -g "
   NASM_FLAGS="-g";;
  2) OPTLEVEL=" -O0 -ggdb "
   NASM_FLAGS="-g";;
  1) OPTLEVEL=" -O2 -g "
   NASM_FLAGS="-g"
   jplatform64=$(./jplatform64.sh)-debug;;
  *) OPTLEVEL=" -O2 ";;
 esac

fi
echo "jplatform64=$jplatform64"

USE_LINENOISE="${USE_LINENOISE:=1}"

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

case "$jplatform64" in
 darwin/j64arm*) macmin="-arch arm64 -mmacosx-version-min=11";;
 darwin/*)      macmin="-arch x86_64 -mmacosx-version-min=10.6";;
	openbsd/*) make=gmake;;
	freebsd/*) make=gmake;;
esac
make="${make:=make}"

CC=${CC-"$(which cc clang gcc 2>/dev/null | head -n1 | xargs basename)"}
compiler="$(readlink -f "$(which $CC)" || which $CC)"
echo "CC=$CC"
echo "compiler=$compiler"

USE_OPENMP="${USE_OPENMP:=0}"
if [ $USE_OPENMP -eq 1 ] ; then
 case $jplatform64 in
  darwin/j64arm*)
   # assume libomp installed at /opt/homebrew/
   OPENMP=" -Xpreprocessor -fopenmp -I/opt/homebrew/include "
   LDOPENMP=" -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lomp "
  ;;
  darwin/*)
   # assume libomp installed at /usr/local/
   OPENMP=" -Xpreprocessor -fopenmp -I/usr/local/include "
   LDOPENMP=" -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lomp "
  ;;
  *)
   OPENMP=" -fopenmp "
   LDOPENMP=" -fopenmp "
   if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
    LDOPENMP32=" -l:libgomp.so.1 " # gcc
   elif [ -f /etc/redhat-release ] ; then
    LDOPENMP32=" -l:libomp.so "    # clang
   else
    LDOPENMP32=" -l:libomp.so.5 "  # clang
   fi
  ;;
 esac
fi

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
 # gcc
 common="$OPENMP -fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden -fno-strict-aliasing -fwrapv -fno-stack-protector -flax-vector-conversions \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wno-cast-function-type \
 -Wno-clobbered \
 -Wno-empty-body \
 -Wno-format-overflow \
 -Wno-implicit-fallthrough \
 -Wno-incompatible-function-pointer-types \
 -Wno-int-to-pointer-cast \
 -Wno-maybe-uninitialized \
 -Wno-missing-field-initializers \
 -Wno-null-pointer-arithmetic \
 -Wno-null-pointer-subtraction \
 -Wno-overflow \
 -Wno-parentheses \
 -Wno-pointer-sign \
 -Wno-pointer-to-int-cast \
 -Wno-return-local-addr \
 -Wno-shift-count-overflow \
 -Wno-shift-negative-value \
 -Wno-sign-compare \
 -Wno-string-plus-int \
 -Wno-type-limits \
 -Wno-uninitialized \
 -Wno-unused-parameter \
 -Wno-unused-value \
 $CFLAGS"

else
 # clang
 common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing -fwrapv \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wsign-compare \
 -Wtautological-constant-out-of-range-compare \
 -Wuninitialized \
 -Wno-char-subscripts \
 -Wno-consumed \
 -Wno-delete-non-abstract-non-virtual-dtor \
 -Wno-deprecated-non-prototype \
 -Wno-empty-body \
 -Wno-implicit-float-conversion \
 -Wno-implicit-int-float-conversion \
 -Wno-incompatible-function-pointer-types \
 -Wno-int-conversion \
 -Wno-int-in-bool-context \
 -Wno-missing-braces \
 -Wno-missing-field-initializers \
 -Wno-null-pointer-arithmetic \
 -Wno-null-pointer-subtraction \
 -Wno-parentheses \
 -Wno-pass-failed \
 -Wno-pointer-sign \
 -Wno-pointer-to-int-cast \
 -Wno-shift-count-overflow \
 -Wno-sometimes-uninitialized \
 -Wno-string-plus-int \
 -Wno-unknown-pragmas \
 -Wno-unsequenced \
 -Wno-unused-but-set-variable \
 -Wno-unused-function \
 -Wno-unused-parameter \
 -Wno-unused-value \
 -Wno-unused-variable \
 $CFLAGS"

fi

common="$common -DJAMALGAM"

USE_BOXEDSPARSE="${USE_BOXEDSPARSE:=0}"
if [ $USE_BOXEDSPARSE -eq 1 ] ; then
 common="$common -DBOXEDSPARSE"
fi

USE_PYXES="${USE_PYXES:=1}"
if [ $USE_PYXES -eq 1 ] ; then
 common="$common -DPYXES=1"
 LDTHREAD=" -pthread "
else
 common="$common -DPYXES=0"
fi

USE_SLEEF_SRC="${USE_SLEEF_SRC:=1}"
case "$jplatform64" in 
 raspberry/j32*) USE_SLEEF=0;;
 *) USE_SLEEF="${USE_SLEEF:=1}";;
esac
if [ $USE_SLEEF -eq 1 ] ; then
 common="$common -DSLEEF=1"
else
 USE_SLEEF_SRC=0
fi

if [ "${USE_GMP_H:=1}" -eq 1 ] ; then
 common="$common -I../../../../mpir/include"
fi

if [ "$USE_LINENOISE" -ne "1" ] ; then
common="$common -DREADLINE"
else
common="$common -DREADLINE -DUSE_LINENOISE"
OBJSLN="linenoise.o"
fi

case "$jplatform64" in
 *32*) USE_EMU_AVX=0;;
  *) USE_EMU_AVX="${USE_EMU_AVX:=1}";;
esac
if [ $USE_EMU_AVX -eq 1 ] ; then
 common="$common -DEMU_AVX=1"
fi

NO_SHA_ASM="${NO_SHA_ASM:=0}"

if [ $NO_SHA_ASM -ne 0 ] ; then

 common="$common -DNO_SHA_ASM"

else

 SRC_ASM_LINUXAVX512=" \
 md5-x86_64-elf.o \
 keccak1600-avx512-elf.o \
 sha1-x86_64-elf.o \
 sha256-x86_64-elf.o \
 sha512-x86_64-elf.o \
 viixamd64.o "

 SRC_ASM_LINUXAVX2=" \
 md5-x86_64-elf.o \
 keccak1600-avx2-elf.o \
 sha1-x86_64-elf.o \
 sha256-x86_64-elf.o \
 sha512-x86_64-elf.o \
 viixamd64.o "

 SRC_ASM_LINUX=" \
 md5-x86_64-elf.o \
 keccak1600-x86_64-elf.o \
 sha1-x86_64-elf.o \
 sha256-x86_64-elf.o \
 sha512-x86_64-elf.o "

 SRC_ASM_LINUX32=" \
 md5-586-elf.o \
 keccak1600-mmx-elf.o \
 sha1-586-elf.o \
 sha256-586-elf.o \
 sha512-586-elf.o "

 SRC_ASM_RASPI=" \
 md5-aarch64-elf.o \
 keccak1600-armv8-elf.o \
 sha1-armv8-elf.o \
 sha256-armv8-elf.o \
 sha512-armv8-elf.o "

 SRC_ASM_RASPI32=" \
 keccak1600-armv4-elf.o \
 sha1-armv4-elf.o \
 sha256-armv4-elf.o \
 sha512-armv4-elf.o "

 SRC_ASM_MAC=" \
 md5-x86_64-macho.o \
 keccak1600-x86_64-macho.o \
 sha1-x86_64-macho.o \
 sha256-x86_64-macho.o \
 sha512-x86_64-macho.o "

 SRC_ASM_MAC32=" \
 md5-586-macho.o \
 keccak1600-mmx-macho.o \
 sha1-586-macho.o \
 sha256-586-macho.o \
 sha512-586-macho.o "

 SRC_ASM_IOS=" \
 md5-aarch64-ios.o \
 keccak1600-armv8-ios.o \
 sha1-armv8-ios.o \
 sha256-armv8-ios.o \
 sha512-armv8-ios.o "

 OBJS_ASM_WIN=" \
 ../../../../openssl-asm/md5-x86_64-nasm.o \
 ../../../../openssl-asm/keccak1600-x86_64-nasm.o \
 ../../../../openssl-asm/sha1-x86_64-nasm.o \
 ../../../../openssl-asm/sha256-x86_64-nasm.o \
 ../../../../openssl-asm/sha512-x86_64-nasm.o "

 OBJS_ASM_WIN32=" \
 ../../../../openssl-asm/md5-586-nasm.o \
 ../../../../openssl-asm/keccak1600-mmx-nasm.o \
 ../../../../openssl-asm/sha1-586-nasm.o \
 ../../../../openssl-asm/sha256-586-nasm.o \
 ../../../../openssl-asm/sha512-586-nasm.o "

fi

OBJS_BASE64=" \
  ../../../../base64/lib/arch/avx2/codec-avx2.o \
  ../../../../base64/lib/arch/generic/codec-generic.o \
  ../../../../base64/lib/arch/neon32/codec-neon32.o \
  ../../../../base64/lib/arch/neon64/codec-neon64.o \
  ../../../../base64/lib/arch/ssse3/codec-ssse3.o \
  ../../../../base64/lib/arch/sse41/codec-sse41.o \
  ../../../../base64/lib/arch/sse42/codec-sse42.o \
  ../../../../base64/lib/arch/avx/codec-avx.o \
  ../../../../base64/lib/lib.o \
  ../../../../base64/lib/codec_choose.o \
  ../../../../base64/lib/tables/tables.o \
"

case $jplatform64 in

 linux/j32*) # linux x86
  TARGET=jamalgam
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -m32 -msse2 -mfpmath=sse "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -m32 -lm -ldl $LDOPENMP32 $LDTHREAD"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX32}"
  GASM_FLAGS="-m32"
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 linux/j64avx512*) # linux intel 64bit avx512
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=skylake-avx512 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX512}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "  #ditto
  FLAGS_BASE64=" -DHAVE_AVX2=1 " #ditto
 ;;

 linux/j64avx2*) # linux intel 64bit avx2
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX2}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 linux/j64avx*) # linux intel 64bit avx
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -mavx "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX "
  FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
 ;;

 linux/j64*) # linux intel 64bit nonavx
  TARGET=jamalgam
  CFLAGS="$common -msse3 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 raspberry/j32*) # linux raspbian arm
  TARGET=jamalgam
  CFLAGS="$common -std=gnu99 -Wno-overflow -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD"
  SRC_ASM="${SRC_ASM_RASPI32}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_VECEXT "    # ENABLE_NEON32 single precision, useless
  FLAGS_BASE64=""
 ;;

 raspberry/j64*) # linux arm64
  TARGET=jamalgam
  CFLAGS="$common -march=armv8-a+crc -DRASPI -DC_CRC32C=1 "    # mno-outline-atomics unavailable on clang-7
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD"
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_RASPI}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_ADVSIMD "
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
 ;;

 openbsd/j32*) # openbsd x86
  TARGET=jamalgam
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -m32 -msse2 -mfpmath=sse "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -m32 -lm -lkvm $LDOPENMP32 $LDTHREAD"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX32}"
  GASM_FLAGS="-m32"
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 openbsd/j64arm) # openbsd arm64
  TARGET=jamalgam
  CFLAGS="$common -march=armv8-a+crc -DC_CRC32C=1 "    # mno-outline-atomics unavailable on clang-7
  LDFLAGS=" -lm -lkvm $LDOPENMP $LDTHREAD"
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_RASPI}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_ADVSIMD "
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
 ;;

 openbsd/j64avx512*) # openbsd intel 64bit avx512
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -lm -lkvm $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=skylake-avx512 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX512}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "  #ditto
  FLAGS_BASE64=" -DHAVE_AVX2=1 " #ditto
 ;;

 openbsd/j64avx2*) # openbsd intel 64bit avx2
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 "
  LDFLAGS=" -lm -lkvm $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX2}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 openbsd/j64avx*) # openbsd intel 64bit avx
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 "
  LDFLAGS=" -lm -lkvm $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -mavx "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX "
  FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
 ;;
 
 openbsd/j64*) # openbsd intel 64bit nonavx
  TARGET=jamalgam
  CFLAGS="$common -msse3 "
  LDFLAGS=" -lm -lkvm $LDOPENMP $LDTHREAD"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;
 
 freebsd/j32*) # freebsd x86
  TARGET=jamalgam
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -m32 -msse2 -mfpmath=sse "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -m32 -lm $LDOPENMP32 $LDTHREAD"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX32}"
  GASM_FLAGS="-m32"
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 freebsd/j64arm) # freebsd arm64
  TARGET=jamalgam
  CFLAGS="$common -march=armv8-a+crc -DC_CRC32C=1 "    # mno-outline-atomics unavailable on clang-7
  LDFLAGS=" -lm $LDOPENMP $LDTHREAD"
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_RASPI}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_ADVSIMD "
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
 ;;

 freebsd/j64avx512*) # freebsd intel 64bit avx512
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -lm $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=skylake-avx512 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX512}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "  #ditto
  FLAGS_BASE64=" -DHAVE_AVX2=1 " #ditto
 ;;

 freebsd/j64avx2*) # freebsd intel 64bit avx2
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 "
  LDFLAGS=" -lm $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX2}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 freebsd/j64avx*) # freebsd intel 64bit avx
  TARGET=jamalgam
  CFLAGS="$common -DC_AVX=1 "
  LDFLAGS=" -lm $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -mavx "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX "
  FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
 ;;
 
 freebsd/j64*) # freebsd intel 64bit nonavx
  TARGET=jamalgam
  CFLAGS="$common -msse3 "
  LDFLAGS=" -lm $LDOPENMP $LDTHREAD"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;
 
 darwin/j32*) # darwin x86
  TARGET=jamalgam
  CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin"
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD -m32 $macmin"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC32}"
  GASM_FLAGS="-m32 $macmin"
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
  ;;

 darwin/j64avx512*) # darwin intel 64bit
  TARGET=jamalgam
  CFLAGS="$common $macmin -DC_AVX=1 -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD $macmin"
  CFLAGS_SIMD=" -march=skylake-avx512 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 darwin/j64avx2*) # darwin intel 64bit
  TARGET=jamalgam
  CFLAGS="$common $macmin -DC_AVX=1 -DC_AVX2=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD $macmin"
  CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 darwin/j64avx*) # darwin intel 64bit
  TARGET=jamalgam
  CFLAGS="$common $macmin -DC_AVX=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD $macmin"
  CFLAGS_SIMD=" -mavx "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_SLEEF=" -DENABLE_AVX "
  FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
 ;;

 darwin/j64arm*) # darwin arm
  TARGET=jamalgam
  CFLAGS="$common $macmin $common -march=armv8-a+crc -mno-outline-atomics -DC_CRC32C=1 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD $macmin"
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_IOS}"
  GASM_FLAGS="$macmin"
  FLAGS_SLEEF=" -DENABLE_ADVSIMD "
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
 ;;

 darwin/j64*) # darwin intel 64bit nonavx
  TARGET=jamalgam
  CFLAGS="$common $macmin -msse3 "
  LDFLAGS=" -lm -ldl $LDOPENMP $LDTHREAD $macmin"
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 windows/j32*) # windows x86
  jolecom="${jolecom:=0}"
  if [ $jolecom -eq 1 ] ; then
   DOLECOM="-DOLECOM"
  fi
  TARGET=jamalgam
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common $DOLECOM -m32 -msse2 -mfpmath=sse -D_FILE_OFFSET_BITS=64 "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP32 $LDTHREAD"
  if [ $jolecom -eq 1 ] ; then
   DLLOBJS=" jdll.o jdllcomx.o "
   LIBJDEF=" ../../../../dllsrc/jdll.def "
  else
   DLLOBJS=" jdll.o "
   LIBJDEF=" ../../../../dllsrc/jdll2.def "
  fi
  LIBJRES=" jdllres.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN32}"
  OBJS_ASM="${OBJS_ASM_WIN32}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 windows/j64avx512*) # windows intel 64bit avx512
  jolecom="${jolecom:=0}"
  if [ $jolecom -eq 1 ] ; then
   DOLECOM="-DOLECOM"
  fi
  TARGET=jamalgam
  CFLAGS="$common $DOLECOM -DC_AVX=1 -DC_AVX2=1 -DC_AVX512=1 -D_FILE_OFFSET_BITS=64 "
  LDFLAGS=" -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=skylake-avx512 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  if [ $jolecom -eq 1 ] ; then
   DLLOBJS=" jdll.o jdllcomx.o "
   LIBJDEF=" ../../../../dllsrc/jdll.def "
  else
   DLLOBJS=" jdll.o "
   LIBJDEF=" ../../../../dllsrc/jdll2.def "
  fi
  LIBJRES=" jdllres.o "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 windows/j64avx2*) # windows intel 64bit avx2
  jolecom="${jolecom:=0}"
  if [ $jolecom -eq 1 ] ; then
   DOLECOM="-DOLECOM"
  fi
  TARGET=jamalgam
  CFLAGS="$common $DOLECOM -DC_AVX=1 -DC_AVX2=1 -D_FILE_OFFSET_BITS=64 "
  LDFLAGS=" -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
  if [ $jolecom -eq 1 ] ; then
   DLLOBJS=" jdll.o jdllcomx.o "
   LIBJDEF=" ../../../../dllsrc/jdll.def "
  else
   DLLOBJS=" jdll.o "
   LIBJDEF=" ../../../../dllsrc/jdll2.def "
  fi
  LIBJRES=" jdllres.o "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX2 "
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
 ;;

 windows/j64avx*) # windows intel 64bit avx
  jolecom="${jolecom:=0}"
  if [ $jolecom -eq 1 ] ; then
   DOLECOM="-DOLECOM"
  fi
  TARGET=jamalgam
  CFLAGS="$common $DOLECOM -DC_AVX=1 -D_FILE_OFFSET_BITS=64 "
  LDFLAGS=" -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
  CFLAGS_SIMD=" -mavx "
  if [ $jolecom -eq 1 ] ; then
   DLLOBJS=" jdll.o jdllcomx.o "
   LIBJDEF=" ../../../../dllsrc/jdll.def "
  else
   DLLOBJS=" jdll.o "
   LIBJDEF=" ../../../../dllsrc/jdll2.def "
  fi
  LIBJRES=" jdllres.o "
  OBJS_FMA=" gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_AVX "
  FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
 ;;

 windows/j64*) # windows intel 64bit nonavx
  jolecom="${jolecom:=0}"
  if [ $jolecom -eq 1 ] ; then
   DOLECOM="-DOLECOM"
  fi
  TARGET=jamalgam
  CFLAGS="$common -msse3 $DOLECOM -D_FILE_OFFSET_BITS=64 "
  LDFLAGS=" -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
  if [ $jolecom -eq 1 ] ; then
   DLLOBJS=" jdll.o jdllcomx.o "
   LIBJDEF=" ../../../../dllsrc/jdll.def "
  else
   DLLOBJS=" jdll.o "
   LIBJDEF=" ../../../../dllsrc/jdll2.def "
  fi
  LIBJRES=" jdllres.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_SLEEF=" -DENABLE_SSE2 "
  FLAGS_BASE64=""
 ;;

 *)
  echo no case for those parameters
  exit
 ;;
esac

if [ $USE_SLEEF -eq 1 ] ; then
 if [ $USE_SLEEF_SRC -eq 1 ] ; then
  OBJS_SLEEF=" \
   ../../../../sleef/src/common/arraymap.o \
   ../../../../sleef/src/common/common.o \
   ../../../../sleef/src/libm/rempitab.o \
   ../../../../sleef/src/libm/sleefsimddp.o \
  "
 fi
fi

echo "CFLAGS=$CFLAGS"

if [ ! -f ../jsrc/jversion.h ] ; then
  cp ../jsrc/jversion-x.h ../jsrc/jversion.h
fi

mkdir -p ../bin/$jplatform64
mkdir -p obj/$jplatform64/
cp makefile-jamalgam obj/$jplatform64/.
export CFLAGS LDFLAGS TARGET CFLAGS_SIMD GASM_FLAGS NASM_FLAGS FLAGS_SLEEF FLAGS_BASE64 DLLOBJS LIBJDEF LIBJRES OBJS_BASE64 OBJS_FMA OBJS_AESNI OBJS_AESARM OBJS_SLEEF OBJS_ASM SRC_ASM OBJSLN jplatform64
cd obj/$jplatform64/
if [ "x$MAKEFLAGS" = x'' ] ; then
 if [ `uname` = Linux ]; then par=`nproc`; else par=`sysctl -n hw.ncpu`; fi
 $make -j$par -f makefile-jamalgam
else
 $make -f makefile-jamalgam
fi
retval=$?
cd -
exit $retval
