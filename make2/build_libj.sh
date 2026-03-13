#!/bin/sh
set -e

cd "$(dirname "$0")"
echo "entering $(pwd)"

unameop=$(uname -o || uname -s)
eval "$(./jplatform64.sh)"

unset TARGET
unset TARGET_a

if [ "" = "$CFLAGS" ]; then
 # OPTLEVEL will be merged back into CFLAGS, further down
 # OPTLEVEL is probably overly elaborate, but it works
 case "$_DEBUG" in
  3)
   OPTLEVEL=" -O2 -g "
   DEBUG=1
   NASM_FLAGS="-g"
   ;;
  2)
   OPTLEVEL=" -O0 -ggdb -DOPTMO0 "
   DEBUG=1
   NASM_FLAGS="-g"
   ;;
  1)
   OPTLEVEL=" -O2 -g "
   DEBUG=1
   NASM_FLAGS="-g"
   j64x=$64x-debug
   ;;
  *)
   OPTLEVEL=" -O2 "
   DEBUG=0
   ;;
 esac
else
 case "$CFLAGS" in *-O0*)
  OPTLEVEL=" -DOPTMO0 "
  DEBUG=1
  ;;
 *) DEBUG=0 ;; esac
fi
echo "jplatform=$jplatform"
echo "j64x=$j64x"

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in gcc 4
# too early to move main linux release package to gcc 5

case "$jplatform/$j64x" in
 darwin/j64iphoneos)
  USE_OPENMP=0
  LDTHREAD=" -pthread "
  if [ -z "$CC" ]; then
   CC="$(xcrun --sdk iphoneos --find clang)"
   AR="$(xcrun --sdk iphoneos --find libtool)"
   macmin="-isysroot $(xcrun --sdk iphoneos --show-sdk-path) -arch arm64"
  else
   macmin="-arch arm64"
  fi
  ;;
 darwin/j64iphonesimulator)
  USE_OPENMP=0
  LDTHREAD=" -pthread "
  if [ -z "$CC" ]; then
   CC="$(xcrun --sdk iphonesimulator --find clang)"
   AR="$(xcrun --sdk iphonesimulator --find libtool)"
   macmin="-isysroot $(xcrun --sdk iphonesimulator --show-sdk-path) -arch x86_64"
  else
   macmin="-arch x86_64"
  fi
  ;;
 darwin/j64arm)
  if [ -z "$CC" ]; then
   CC="$(xcrun --sdk macosx --find clang)"
   AR="$(xcrun --sdk macosx --find libtool)"
   macmin="-isysroot $(xcrun --sdk macosx --show-sdk-path) -arch arm64 -mmacosx-version-min=11"
  else
   macmin="-arch arm64 -mmacosx-version-min=11"
  fi
  ;;
 darwin/*)
  if [ -z "$CC" ]; then
   CC="$(xcrun --sdk macosx --find clang)"
   AR="$(xcrun --sdk macosx --find libtool)"
   macmin="-isysroot $(xcrun --sdk macosx --show-sdk-path) -arch x86_64 -mmacosx-version-min=10.6"
  else
   macmin="-arch x86_64 -mmacosx-version-min=10.6"
  fi
  ;;
 openbsd/*)
  NO_SHA_ASM=1
  make=gmake
  ;;
 freebsd/*) make=gmake ;;
 wasm*)
  USE_OPENMP=0
  LDTHREAD=" -pthread "
  NO_SHA_ASM=1
  USE_PYXES=0
  ;;
esac
case "$j64x" in
 j32*) USE_PYXES="${USE_PYXES:=0}" ;;
 *) USE_PYXES="${USE_PYXES:=1}" ;;
esac
make="${make:=make}"

CC=${CC-"$(which cc clang gcc 2> /dev/null | head -n1 | xargs basename)"}
CXX="${CXX:=$CC}"
echo "CC=$CC"
if [ 1 -eq $($CC -dM -E - < /dev/null | grep -c __clang__) ]; then
 compiler=clang
elif [ 1 -eq $($CC -dM -E - < /dev/null | grep -c __GNUC__) ]; then
 compiler=gcc
else
 compiler=$(readlink -f $(which $CC) || which $CC)
fi
echo "compiler=$compiler"
echo "$($CC --version)"

USE_OPENMP="${USE_OPENMP:=0}"
if [ $USE_OPENMP -eq 1 ]; then
 case "$jplatform/$j64x" in
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
   elif [ -f /etc/redhat-release ]; then
    LDOPENMP32=" -l:libomp.so " # clang
   else
    LDOPENMP32=" -l:libomp.so.5 " # clang
   fi
   ;;
 esac
fi

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
 # gcc
 common="$OPENMP -fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden -fno-strict-aliasing -fwrapv -fno-stack-protector -flax-vector-conversions -ffp-contract=off \
 -Werror -Wextra -Wno-unknown-warning-option \
 -fsignaling-nans \
 -Wno-attributes \
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
 -Wno-shift-negative-value \
 -Wno-sign-compare \
 -Wno-string-plus-int \
 -Wno-type-limits \
 -Wno-uninitialized \
 -Wno-unused-parameter \
 -Wno-unused-value \
 $CFLAGS"

 GNUC_MAJOR=$(echo __GNUC__ | $CC -E -x c - | tail -n 1)
 GNUC_MINOR=$(echo __GNUC_MINOR__ | $CC -E -x c - | tail -n 1)
 if [ $GNUC_MAJOR -ge 5 ]; then
  common="$common -std=gnu17 -Wno-error=stringop-overflow"
 else
  common="$common -std=gnu99 -DC_NOMULTINTRINSIC"
 fi

else
 # clang
 common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing -fwrapv \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wconstant-conversion \
 -Wsign-compare \
 -Wtautological-constant-out-of-range-compare \
 -Wtypedef-redefinition \
 -Wuninitialized \
 -Wno-braced-scalar-init \
 -Wno-cast-function-type-mismatch \
 -Wno-char-subscripts \
 -Wno-consumed \
 -Wno-delete-non-abstract-non-virtual-dtor \
 -Wno-deprecated-non-prototype \
 -Wno-empty-body \
 -Wno-gnu-folding-constant \
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

if [ $DEBUG -eq 1 ]; then
 common="$common -DDEBUG"
fi

case "$jplatform/$j64x" in
 darwin/*) common="$common -fno-common" ;; # other platforms already default to this
esac

case "$jplatform/$j64x" in
 darwin/j64) # darwin intel 64bit nonavx
  common="$common -msse4.1 -msse4.2 "
  ;;
 */j64)
  if [ -n "$_SSE4_2" ]; then
   common="$common -msse4.1 -msse4.2 "
  fi
  ;;
esac

if [ $USE_OPENMP -eq 1 ]; then
 common="$common -I../openmp/include"
fi

USE_BOXEDSPARSE="${USE_BOXEDSPARSE:=0}"
if [ $USE_BOXEDSPARSE -eq 1 ]; then
 common="$common -DBOXEDSPARSE"
fi

if [ $USE_PYXES -eq 1 ]; then
 case "$jplatform/$j64x" in
  windows/j32*)
   common="$common -DPYXES=1 -I../pthreads4w/include"
   LDTHREAD=" ../pthreads4w/lib/pthreadVC3-w32.lib "
   ;;
  windows/j64arm)
   common="$common -DPYXES=1 -I../pthreads4w/include"
   LDTHREAD=" ../pthreads4w/lib/pthreadVC3-arm64.lib "
   ;;
  windows/*)
   common="$common -DPYXES=1 -I../pthreads4w/include"
   LDTHREAD=" ../pthreads4w/lib/pthreadVC3.lib "
   ;;
  *)
   common="$common -DPYXES=1"
   LDTHREAD=" -pthread "
   ;;
 esac
 case "$j64x" in
  j32*) USE_NORMAH8=1 ;;
  *) USE_NORMAH8="${USE_NORMAH8:=0}" ;;
 esac
else
 common="$common -DPYXES=0"
 USE_NORMAH8="${USE_NORMAH8:=0}"
fi

if [ $USE_NORMAH8 -eq 1 ]; then
 common="$common -DNORMAH8=1"
else
 common="$common -DNORMAH8=0"
fi

case "$jplatform/$j64x" in
 */j64)
  USE_SLEEF=0
  USE_SLEEFQUAD=1
  ;;
 raspberry/j32*) USE_SLEEF=0 ;;
 wasm*) USE_SLEEF=0 ;;
 *) USE_SLEEF="${USE_SLEEF:=1}" ;;
esac
USE_SLEEFQUAD="${USE_SLEEFQUAD:=$USE_SLEEF}"
if [ $USE_SLEEF -eq 1 ]; then
 common="$common -DSLEEF=1"
else
 common="$common -DSLEEF=0"
fi
if [ $USE_SLEEFQUAD -eq 1 ]; then
 common="$common -DSLEEFQUAD=1"
else
 common="$common -DSLEEFQUAD=0"
fi

if [ "${USE_GMP_H:=1}" -eq 1 ]; then
 common="$common -I../mpir/include"
fi

if [ -n "$_MEMAUDIT" ]; then
 common="$common -DMEMAUDIT=$_MEMAUDIT"
fi

if [ -n "$_ASSERT" ]; then
 common="$common -D_ASSERT"
fi

# enforce _ASSERT on file level
if [ -n "$_ASSERT2" ]; then
 common="$common -D_ASSERT2"
fi

if [ -n "$_NAN" ]; then
 common="$common -D_NAN"
fi

if [ -n "$_NAMETRACK" ]; then
 common="$common -DNAMETRACK=$_NAMETRACK"
fi

case "$jplatform/$j64x" in
 *32*) USE_EMU_AVX=0 ;;
 wasm*) USE_EMU_AVX=0 ;;
 *) USE_EMU_AVX="${USE_EMU_AVX:=1}" ;;
esac
if [ $USE_EMU_AVX -eq 1 ]; then
 common="$common -DEMU_AVX2=1"
else
 common="$common -DEMU_AVX2=0"
fi

NO_SHA_ASM="${NO_SHA_ASM:=0}"

if [ $NO_SHA_ASM -ne 0 ]; then

 common="$common -DNO_SHA_ASM"

else

 SRC_ASM_LINUXAVX512=" \
 openssl/sha/asm/md5-x86_64-elf.o \
 openssl/sha/asm/keccak1600-avx512-elf.o \
 openssl/sha/asm/sha1-x86_64-elf.o \
 openssl/sha/asm/sha256-x86_64-elf.o \
 openssl/sha/asm/sha512-x86_64-elf.o \
 viixamd64.o "

 SRC_ASM_LINUXAVX2=" \
 openssl/sha/asm/md5-x86_64-elf.o \
 openssl/sha/asm/keccak1600-avx2-elf.o \
 openssl/sha/asm/sha1-x86_64-elf.o \
 openssl/sha/asm/sha256-x86_64-elf.o \
 openssl/sha/asm/sha512-x86_64-elf.o "

 SRC_ASM_LINUX=" \
 openssl/sha/asm/md5-x86_64-elf.o \
 openssl/sha/asm/keccak1600-x86_64-elf.o \
 openssl/sha/asm/sha1-x86_64-elf.o \
 openssl/sha/asm/sha256-x86_64-elf.o \
 openssl/sha/asm/sha512-x86_64-elf.o "

 SRC_ASM_LINUX32=" \
 openssl/sha/asm/md5-586-elf.o \
 openssl/sha/asm/keccak1600-mmx-elf.o \
 openssl/sha/asm/sha1-586-elf.o \
 openssl/sha/asm/sha256-586-elf.o \
 openssl/sha/asm/sha512-586-elf.o "

 SRC_ASM_RASPI=" \
 openssl/sha/asm/md5-aarch64-elf.o \
 openssl/sha/asm/keccak1600-armv8-elf.o \
 openssl/sha/asm/sha1-armv8-elf.o \
 openssl/sha/asm/sha256-armv8-elf.o \
 openssl/sha/asm/sha512-armv8-elf.o "

 SRC_ASM_RASPI32=" \
 openssl/sha/asm/keccak1600-armv4-elf.o \
 openssl/sha/asm/sha1-armv4-elf.o \
 openssl/sha/asm/sha256-armv4-elf.o \
 openssl/sha/asm/sha512-armv4-elf.o "

 SRC_ASM_MAC=" \
 openssl/sha/asm/md5-x86_64-macho.o \
 openssl/sha/asm/keccak1600-x86_64-macho.o \
 openssl/sha/asm/sha1-x86_64-macho.o \
 openssl/sha/asm/sha256-x86_64-macho.o \
 openssl/sha/asm/sha512-x86_64-macho.o "

 SRC_ASM_MAC32=" \
 openssl/sha/asm/md5-586-macho.o \
 openssl/sha/asm/keccak1600-mmx-macho.o \
 openssl/sha/asm/sha1-586-macho.o \
 openssl/sha/asm/sha256-586-macho.o \
 openssl/sha/asm/sha512-586-macho.o "

 SRC_ASM_IOS=" \
 openssl/sha/asm/md5-aarch64-ios.o \
 openssl/sha/asm/keccak1600-armv8-ios.o \
 openssl/sha/asm/sha1-armv8-ios.o \
 openssl/sha/asm/sha256-armv8-ios.o \
 openssl/sha/asm/sha512-armv8-ios.o "

 OBJS_ASM_WIN=" \
 ../openssl-asm/md5-x86_64-nasm.o \
 ../openssl-asm/keccak1600-x86_64-nasm.o \
 ../openssl-asm/sha1-x86_64-nasm.o \
 ../openssl-asm/sha256-x86_64-nasm.o \
 ../openssl-asm/sha512-x86_64-nasm.o "

 OBJS_ASM_WIN32=" \
 ../openssl-asm/md5-586-nasm.o \
 ../openssl-asm/keccak1600-mmx-nasm.o \
 ../openssl-asm/sha1-586-nasm.o \
 ../openssl-asm/sha256-586-nasm.o \
 ../openssl-asm/sha512-586-nasm.o "

fi

OBJS_BASE64=" \
  ../base64/lib/arch/avx2/codec.o \
  ../base64/lib/arch/avx512/codec.o \
  ../base64/lib/arch/generic/codec.o \
  ../base64/lib/arch/neon32/codec.o \
  ../base64/lib/arch/neon64/codec.o \
  ../base64/lib/arch/ssse3/codec.o \
  ../base64/lib/arch/sse41/codec.o \
  ../base64/lib/arch/sse42/codec.o \
  ../base64/lib/arch/avx/codec.o \
  ../base64/lib/lib.o \
  ../base64/lib/codec_choose.o \
  ../base64/lib/tables/tables.o "

OBJS_SIMDUTF8_ASM=" \
  utf/utf8_to_utf16le_avx512.o \
  utf/utf16le_to_utf8_avx512.o "

case "$jplatform/$j64x" in

 linux/j32*) # linux x86
  TARGET=libj.so
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -m32 -msse2 -mfpmath=sse "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -shared -Wl,-soname,libj.so -m32 -lm -ldl $LDOPENMP32 $LDTHREAD -Wl,-z,noexecstack "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX32}"
  GASM_FLAGS="-m32"
  FLAGS_BASE64=""
  ;;

 linux/j64avx512*) # linux intel 64bit avx512
  TARGET=libj.so
  CFLAGS="$common -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  CFLAGS_SIMD=" -march=skylake-avx512 -mtune=skylake-avx512 -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  OBJS_SIMDUTF8="${OBJS_SIMDUTF8_ASM}"
  SRC_ASM="${SRC_ASM_LINUXAVX512}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX512F=1 "
  ;;

 linux/j64avx2*) # linux intel 64bit avx2
  TARGET=libj.so
  CFLAGS="$common -DC_AVX2=1 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  CFLAGS_SIMD=" -march=skylake -mtune=skylake -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX2}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
  ;;

 linux/j64*) # linux intel 64bit nonavx
  TARGET=libj.so
  CFLAGS="$common -msse3 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 raspberry/j32*) # linux raspbian arm
  TARGET=libj.so
  CFLAGS="$common -Wno-overflow -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  SRC_ASM="${SRC_ASM_RASPI32}"
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 raspberry/j64*) # linux arm64
  TARGET=libj.so
  CFLAGS="$common -march=armv8-a+crc -DRASPI " # mno-outline-atomics unavailable on clang-7
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_RASPI}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
  ;;

 openbsd/j32*) # openbsd x86
  TARGET=libj.so
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -m32 -msse2 -mfpmath=sse "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -shared -Wl,-soname,libj.so -m32 -lm -lkvm $LDTHREAD $LDOPENMP32 -Wl,-z,noexecstack "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX32}"
  GASM_FLAGS="-m32"
  FLAGS_BASE64=""
  ;;

 openbsd/j64arm) # openbsd arm64
  TARGET=libj.so
  CFLAGS="$common -march=armv8-a+crc " # mno-outline-atomics unavailable on clang-7
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -lkvm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_RASPI}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
  ;;

 openbsd/j64avx512*) # openbsd intel 64bit avx512
  TARGET=libj.so
  CFLAGS="$common -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -lkvm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  CFLAGS_SIMD=" -march=skylake-avx512 -mtune=skylake-avx512 -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  OBJS_SIMDUTF8="${OBJS_SIMDUTF8_ASM}"
  SRC_ASM="${SRC_ASM_LINUXAVX512}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX512F=1 "
  ;;

 openbsd/j64avx2*) # openbsd intel 64bit avx2
  TARGET=libj.so
  CFLAGS="$common -DC_AVX2=1 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -lkvm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  CFLAGS_SIMD=" -march=skylake -mtune=skylake -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX2}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
  ;;

 openbsd/j64*) # openbsd intel 64bit nonavx
  TARGET=libj.so
  CFLAGS="$common -msse3 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm -lkvm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 freebsd/j32*) # freebsd x86
  TARGET=libj.so
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -m32 -msse2 -mfpmath=sse "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  LDFLAGS=" -shared -Wl,-soname,libj.so -m32 -lm $LDOPENMP32 $LDTHREAD -Wl,-z,noexecstack "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX32}"
  GASM_FLAGS="-m32"
  FLAGS_BASE64=""
  ;;

 freebsd/j64arm) # freebsd arm64
  TARGET=libj.so
  CFLAGS="$common -march=armv8-a+crc " # mno-outline-atomics unavailable on clang-7
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_RASPI}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
  ;;

 freebsd/j64avx512*) # freebsd intel 64bit avx512
  TARGET=libj.so
  CFLAGS="$common -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  CFLAGS_SIMD=" -march=skylake-avx512 -mtune=skylake-avx512 -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  OBJS_SIMDUTF8="${OBJS_SIMDUTF8_ASM}"
  SRC_ASM="${SRC_ASM_LINUXAVX512}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX512F=1 "
  ;;

 freebsd/j64avx2*) # freebsd intel 64bit avx2
  TARGET=libj.so
  CFLAGS="$common -DC_AVX2=1 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  CFLAGS_SIMD=" -march=skylake -mtune=skylake -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUXAVX2}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
  ;;

 freebsd/j64*) # freebsd intel 64bit nonavx
  TARGET=libj.so
  CFLAGS="$common -msse3 "
  LDFLAGS=" -shared -Wl,-soname,libj.so -lm $LDTHREAD $LDOPENMP -Wl,-z,noexecstack "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_LINUX}"
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 darwin/j32*) # darwin x86
  TARGET=libj.dylib
  CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm -ldl $LDTHREAD $LDOPENMP -m32 $macmin -framework Accelerate "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC32}"
  GASM_FLAGS="-m32 $macmin "
  FLAGS_BASE64=""
  ;;

 darwin/j64avx512*) # darwin intel 64bit
  TARGET=libj.dylib
  CFLAGS="$common $macmin -DC_AVX2=1 -DC_AVX512=1 "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm -ldl $LDTHREAD $LDOPENMP $macmin -framework Accelerate "
  CFLAGS_SIMD=" -march=skylake-avx512 -mtune=skylake-avx512 -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  OBJS_SIMDUTF8="${OBJS_SIMDUTF8_ASM}"
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_BASE64=" -DHAVE_AVX512F=1 "
  ;;

 darwin/j64avx2*) # darwin intel 64bit
  TARGET=libj.dylib
  CFLAGS="$common $macmin -DC_AVX2=1 "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm -ldl $LDTHREAD $LDOPENMP $macmin -framework Accelerate "
  CFLAGS_SIMD=" -march=skylake -mtune=skylake -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
  ;;

 darwin/j64arm*) # darwin arm
  TARGET=libj.dylib
  CFLAGS="$common $macmin -march=armv8-a+crc -mno-outline-atomics "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm -ldl $LDTHREAD $LDOPENMP $macmin -framework Accelerate "
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_IOS}"
  GASM_FLAGS="$macmin"
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
  ;;

 darwin/j64iphoneos) # iphone
  TARGET_a=libj.a
  CFLAGS="$common $macmin -D IMPORTGMPLIB -march=armv8-a+crc -mno-outline-atomics "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm $LDTHREAD $LDOPENMP $macmin -framework Accelerate "
  LDFLAGS_a=" rcs "
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM="${SRC_ASM_IOS}"
  GASM_FLAGS="$macmin"
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
  ;;

 darwin/j64iphonesimulator) # iphone simulator
  TARGET_a=libj.a
  CFLAGS="$common $macmin -D IMPORTGMPLIB -msse4.1 -msse4.2 "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm $LDTHREAD $LDOPENMP $macmin -framework Accelerate "
  LDFLAGS_a=" rcs "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_BASE64=""
  ;;

 darwin/j64*) # darwin intel 64bit nonavx
  TARGET=libj.dylib
  CFLAGS="$common $macmin -msse3 "
  LDFLAGS=" -dynamiclib -install_name libj.dylib -lm -ldl $LDTHREAD $LDOPENMP $macmin -framework Accelerate "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_MAC}"
  GASM_FLAGS="$macmin"
  FLAGS_BASE64=" -DHAVE_SSE42=1 "
  ;;

 windows/j32*) # windows x86
  jolecom="${jolecom:=1}"
  if [ $jolecom -eq 1 ]; then
   DOLECOM="-DOLECOM"
  fi
  WINDRES="${WINDRES:=windres}"
  TARGET=j.dll
  # faster, but sse2 not available for 32-bit amd cpu
  # sse does not support mfpmath=sse in 32-bit gcc
  CFLAGS="$common -Wno-psabi -Wno-incompatible-pointer-types -m32 -msse2 -mfpmath=sse $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 "
  # slower, use 387 fpu and truncate extra precision
  # CFLAGS="$common -m32 -ffloat-store "
  CPPFLAGS="-fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden -Wno-psabi $DOLECOM -m32 -msse2 -mfpmath=sse -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 "
  # windows j32 still needs libwinpthread-1.dll even with the static switch
  LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic -lole32 -ladvapi32 -loleaut32 -lsynchronization -lpsapi -luuid $LDTHREAD $LDOPENMP "
  if [ $jolecom -eq 1 ]; then
   DLLOBJS=" ../dllsrc/jdll.o ../dllsrc/jdllcomx.o "
   LIBJDEF=" ../dllsrc/jdll.def "
  else
   DLLOBJS=" ../dllsrc/jdll.o "
   LIBJDEF=" ../dllsrc/jdll2.def "
  fi
  LIBJRES=" ../dllsrc/jdll.res "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM=""
  OBJS_ASM="${OBJS_ASM_WIN32}"
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 windows/j64arm) # windows arm64
  jolecom="${jolecom:=1}"
  if [ $jolecom -eq 1 ]; then
   DOLECOM="-DOLECOM"
  fi
  WINDRES="${WINDRES:=windres}"
  TARGET=j.dll
  CFLAGS="$common -march=armv8-a+crc -Wno-incompatible-pointer-types -DNO_SHA_ASM $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  CPPFLAGS="-fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic -lole32 -ladvapi32 -loleaut32 -lsynchronization -luuid $LDTHREAD $LDOPENMP "
  if [ $jolecom -eq 1 ]; then
   DLLOBJS=" ../dllsrc/jdll.o ../dllsrc/jdllcomx.o "
   LIBJDEF=" ../dllsrc/jdll.def "
  else
   DLLOBJS=" ../dllsrc/jdll.o "
   LIBJDEF=" ../dllsrc/jdll2.def "
  fi
  LIBJRES=" ../dllsrc/jdll.res "
  OBJS_AESARM=" aes-arm.o "
  SRC_ASM=""
  OBJS_ASM=""
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_NEON64=1 "
  ;;

 windows/j64avx512*) # windows intel 64bit avx512
  jolecom="${jolecom:=1}"
  if [ $jolecom -eq 1 ]; then
   DOLECOM="-DOLECOM"
  fi
  WINDRES="${WINDRES:=windres}"
  TARGET=j.dll
  CFLAGS="$common -Wno-incompatible-pointer-types $DOLECOM -DC_AVX2=1 -DC_AVX512=1 -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  CPPFLAGS="-fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden $DOLECOM -DC_AVX2=1 -DC_AVX512=1 -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic -lole32 -ladvapi32 -loleaut32 -lsynchronization -luuid $LDTHREAD $LDOPENMP "
  CFLAGS_SIMD=" -march=skylake-avx512 -mtune=skylake-avx512 -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  if [ $jolecom -eq 1 ]; then
   DLLOBJS=" ../dllsrc/jdll.o ../dllsrc/jdllcomx.o "
   LIBJDEF=" ../dllsrc/jdll.def "
  else
   DLLOBJS=" ../dllsrc/jdll.o "
   LIBJDEF=" ../dllsrc/jdll2.def "
  fi
  LIBJRES=" ../dllsrc/jdll.res "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  OBJS_SIMDUTF8="${OBJS_SIMDUTF8_ASM}"
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX512F=1 "
  ;;

 windows/j64avx2*) # windows intel 64bit avx2
  jolecom="${jolecom:=1}"
  if [ $jolecom -eq 1 ]; then
   DOLECOM="-DOLECOM"
  fi
  WINDRES="${WINDRES:=windres}"
  TARGET=j.dll
  CFLAGS="$common -Wno-incompatible-pointer-types $DOLECOM -DC_AVX2=1 -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  CPPFLAGS="-fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden $DOLECOM -DC_AVX2=1 -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic -lole32 -ladvapi32 -loleaut32 -lsynchronization -luuid $LDTHREAD $LDOPENMP "
  CFLAGS_SIMD=" -march=skylake -mtune=skylake -msse4.1 -msse4.2 -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt -mno-vzeroupper "
  if [ $jolecom -eq 1 ]; then
   DLLOBJS=" ../dllsrc/jdll.o ../dllsrc/jdllcomx.o "
   LIBJDEF=" ../dllsrc/jdll.def "
  else
   DLLOBJS=" ../dllsrc/jdll.o "
   LIBJDEF=" ../dllsrc/jdll2.def "
  fi
  LIBJRES=" ../dllsrc/jdll.res "
  OBJS_FMA=" blis/gemm_int-fma.o "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_BASE64=" -DHAVE_AVX2=1 "
  ;;

 windows/j64*) # windows intel 64bit nonavx
  jolecom="${jolecom:=1}"
  if [ $jolecom -eq 1 ]; then
   DOLECOM="-DOLECOM"
  fi
  WINDRES="${WINDRES:=windres}"
  TARGET=j.dll
  CFLAGS="$common -Wno-incompatible-pointer-types -msse3 $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  CPPFLAGS="-fPIC $OPTLEVEL -falign-functions=4 -fvisibility=hidden $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL -D_WIN32 -D_WIN64 "
  LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic -lole32 -ladvapi32 -loleaut32 -lsynchronization -luuid $LDTHREAD $LDOPENMP "
  if [ $jolecom -eq 1 ]; then
   DLLOBJS=" ../dllsrc/jdll.o ../dllsrc/jdllcomx.o "
   LIBJDEF=" ../dllsrc/jdll.def "
  else
   DLLOBJS=" ../dllsrc/jdll.o "
   LIBJDEF=" ../dllsrc/jdll2.def "
  fi
  LIBJRES=" ../dllsrc/jdll.res "
  OBJS_AESNI=" aes-ni.o "
  SRC_ASM="${SRC_ASM_WIN}"
  OBJS_ASM="${OBJS_ASM_WIN}"
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 wasm/j32) # webassembly
  TARGET_a=libj.a
  # 948KB stack on v8
  CFLAGS="$common -m32 -D IMPORTGMPLIB -D CSTACKSIZE=1007616 -D CSTACKRESERVE=100000 -Wno-cast-function-type-mismatch "
  # these flags do not work on iOS
  # -msse2 -msimd128
  # EMSCRIPTEN_KEEPALIVE instead of -s LINKABLE=1 -s EXPORT_ALL=1
  LDFLAGS_a=" rcs "
  SRC_ASM=""
  GASM_FLAGS=""
  FLAGS_BASE64=""
  ;;

 *)
  echo no case for those parameters
  exit
  ;;
esac

echo "CFLAGS=$CFLAGS"

if [ ! -f ../jsrc/jversion.h ]; then
 cp ../jsrc/jversion-x.h ../jsrc/jversion.h
fi

mkdir -p ../bin/$jplatform/$j64x
export CC AR CFLAGS CPPFLAGS LDFLAGS LDFLAGS_a LDFLAGS_b TARGET TARGET_a CFLAGS_SIMD GASM_FLAGS NASM_FLAGS FLAGS_BASE64 DLLOBJS LIBJDEF LIBJRES WINDRES OBJS_BASE64 OBJS_FMA OBJS_AESNI OBJS_AESARM OBJS_SIMDUTF8 OBJS_ASM SRC_ASM jplatform j64x WINDRES LDFLAGS_b
if [ "x$MAKEFLAGS" = x'' ]; then
 if ([ "$unameop" = "Linux" ] || [ "$unameop" = "GNU/Linux" ]); then
  par=$(nproc)
 elif [ "$unameop" = "Darwin" ] || [ "$unameop" = "OpenBSD" ] || [ "$unameop" = "FreeBSD" ]; then
  par=$(sysctl -n hw.ncpu)
 else
  par=2
 fi
 export MAKEFLAGS=-j$par
fi
echo "MAKEFLAGS=$MAKEFLAGS"
cd ../jsrc
if [ "1" != "$NOCLEAN" ]; then
$make -f ../make2/makefile-libj clean
fi
$make -f ../make2/makefile-libj all
retval=$?
cd -
exit $retval
