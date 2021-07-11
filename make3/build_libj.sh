#!/usr/bin/env sh
set -o errexit -o nounset -o noclobber

host=$(uname --machine)

case "$host" in
	armv6l|aarch64) jplatform=${jplatform:-raspberry};;
	Darwin) jplatform=${jplatform:-darwin};;
	*) jplatform=${jplatform:-linux};;
esac
[ "${RASPI:-}" ] && jplatform=${jplatform:-raspberry};

case "$host" in
	x86_64) j64x=${j64x:-j64avx};;
	aarch64) j64x=${j64x:-j64};;
	*) j64x=${j64x:-j32};;
esac

[ -f /usr/bin/cc ] && CC=${CC:-cc}
[ -f /usr/bin/clang ] && CC=${CC:-clang}
CC=${CC:-gcc}

macmin="-mmacosx-version-min=10.6"
# compiler=$("$CC" --version | head -n 1)
compiler=$(readlink -f "$(command -v "$CC")" 2> /dev/null || echo "$CC")
[ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ] && gcc=1

[ "${USE_OPENMP:-}" ] && LDOPENMP32="-l:libomp.so.5" # clang
[ "${USE_OPENMP:-}" ] && [ "${gcc:-}" ] && LDOPENMP32="-l:libgomp.so.1"
[ "${USE_OPENMP:-}" ] && OPENMP="-fopenmp" && LDOPENMP="-fopenmp"

[ "${USE_THREAD:-}" ] && USETHREAD="=DUSE_THREAD"
[ "${USE_THREAD:-}" ] && LDTHREAD='-pthread'
[ "${VERBOSELOG:-}" ] && FVERBOSELOG="-DVERBOSELOG"

CFLAGS="${CFLAGS:-} ${OPENMP:-} ${USETHREAD:-} ${FVERBOSELOG:-} -Werror -fPIC -O2 -fvisibility=hidden -fwrapv -fno-strict-aliasing -Wextra"
# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5
if [ "${gcc:-}" ]; then
	CFLAGS="${CFLAGS:-} -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-type-limits"
	GNUC_MAJOR=$(echo __GNUC__ | "$CC" -E -x c - | tail -n 1)
	# shellcheck disable=SC2034
	GNUC_MINOR=$(echo __GNUC_MINOR__ | "$CC" -E -x c - | tail -n 1)
	[ "$GNUC_MAJOR" -lt 5 ] && CFLAGS="${CFLAGS:-} -DC_NOMULTINTRINSIC -Wno-uninitialized"
	[ "$GNUC_MAJOR" -ge 5 ] && CFLAGS="${CFLAGS:-} -Wno-maybe-uninitialized"
	[ "$GNUC_MAJOR" -ge 6 ] && CFLAGS="${CFLAGS:-} -Wno-shift-negative-value"
	# alternatively, add comment /* fall through */
	[ "$GNUC_MAJOR" -ge 7 ] && CFLAGS="${CFLAGS:-} -Wno-implicit-fallthrough"
	[ "$GNUC_MAJOR" -ge 8 ] && CFLAGS="${CFLAGS:-} -Wno-cast-function-type"
else # clang 3.4
	CFLAGS="${CFLAGS:-} -Wno-consumed -Wuninitialized -Wno-unused-parameter -Wsign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wunsequenced -Wno-string-plus-int -Wtautological-constant-out-of-range-compare"
	CLANG_MAJOR=$(echo __clang_major__ | "$CC" -E -x c - | tail -n 1)
	CLANG_MINOR=$(echo __clang_minor__ | "$CC" -E -x c - | tail -n 1)
	[ "$CLANG_MAJOR" -eq 3 ] && [ "$CLANG_MINOR" -ge 8 ] && CFLAGS="${CFLAGS:-} -Wno-pass-failed"
	[ "$CLANG_MAJOR" -ge 4 ] && CFLAGS="${CFLAGS:-} -Wno-pass-failed"
	[ "$CLANG_MAJOR" -ge 10 ] && CFLAGS="${CFLAGS:-} -Wno-implicit-float-conversion"
fi

NO_SHA_ASM="${NO_SHA_ASM:=0}"

if [ "$NO_SHA_ASM" -ne 0 ] ; then

CFLAGS="${CFLAGS:-} -DNO_SHA_ASM"

else

SRC_ASM_LINUX=" \
 keccak1600-x86_64-elf.o \
 sha1-x86_64-elf.o \
 sha256-x86_64-elf.o \
 sha512-x86_64-elf.o "

SRC_ASM_LINUX32=" \
 keccak1600-mmx-elf.o \
 sha1-586-elf.o \
 sha256-586-elf.o \
 sha512-586-elf.o "

SRC_ASM_RASPI=" \
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
 keccak1600-x86_64-macho.o \
 sha1-x86_64-macho.o \
 sha256-x86_64-macho.o \
 sha512-x86_64-macho.o "

SRC_ASM_MAC32=" \
 keccak1600-mmx-macho.o \
 sha1-586-macho.o \
 sha256-586-macho.o \
 sha512-586-macho.o "

OBJS_ASM_WIN=" \
 ../../../../openssl-asm/keccak1600-x86_64-nasm.o \
 ../../../../openssl-asm/sha1-x86_64-nasm.o \
 ../../../../openssl-asm/sha256-x86_64-nasm.o \
 ../../../../openssl-asm/sha512-x86_64-nasm.o "

OBJS_ASM_WIN32=" \
 ../../../../openssl-asm/keccak1600-mmx-nasm.o \
 ../../../../openssl-asm/sha1-586-nasm.o \
 ../../../../openssl-asm/sha256-586-nasm.o \
 ../../../../openssl-asm/sha512-586-nasm.o "

fi

case "$j64x" in
	j64avx2) j64=1; javx=1; javx2=1;;
	j64avx) j64=1; javx=1;;
	j64) j64=1;;
	j32) j32=1;;
esac

case "$jplatform" in
	linux|raspberry) TARGET="libj.so";;
	darwin) TARGET="libj.dylib";;
	windows) TARGET="j.dll";;
esac

case "$jplatform" in
	linux) LDFLAGS="${LDFLAGS:-} ${j32:+-m32} -shared -Wl,-soname,libj.so -lm -ldl ${j32:+$LDOPENMP32} ${j64:+$LDOPENMP} ${LDTHREAD:-}";;
	raspberry) LDFLAGS="${LDFLAGS:-} -shared -Wl,-soname,libj.so -lm -ldl ${LDOPENMP:-} ${LDTHREAD:-}";;
	darwin) LDFLAGS="${LDFLAGS:-} ${j32:+-m32} -dynamiclib -lm -ldl ${LDOPENMP:-} ${LDTHREAD:-} $macmin";;
	windows) LDFLAGS="${LDFLAGS:-} ${j32:+$LDOPENMP32} ${j64:-$LDOPENMP} -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ ${LDTHREAD:-}";;
esac

case "$jplatform" in
	# faster, but sse2 not available for 32-bit amd cpu
	# sse does not support mfpmath=sse in 32-bit gcc
	# slower, use 387 fpu and truncate extra precision
	# CFLAGS="${CFLAGS:-} -m32 -ffloat-store "
	linux) CFLAGS="${CFLAGS:-} ${j32:+-m32 -msse2 -mfpmath=sse} ${javx:+-DC_AVX=1} ${javx2:+-DC_AVX2=1}";;
	raspberry) CFLAGS="${CFLAGS:-} ${j32:+-marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI} ${j64:+-march=armv8-a+crc -DRASPI -DC_CRC32C=1}";;
	darwin) CFLAGS="${CFLAGS:-} $macmin ${j32:+-m32} ${javx:+-DC_AVX=1} ${javx2:+-DC_AVX2=1}";;
	windows) CFLAGS="${CFLAGS:-} ${DOLECOM:-} ${j32:+-m32 -msse2 -mfpmath=sse} -D_FILE_OFFSET_BITS=64 -D_JDLL ${javx:+-DC_AVX=1} ${javx2:+-DC_AVX2=1}"
esac

case "$jplatform" in
	linux|darwin|windows) OBJS_AESNI="aes-ni.o";;
	raspberry);;
esac

case "$jplatform" in
	darwin) GASM_FLAGS="${j32:+-m32} $macmin";;
	linux) GASM_FLAGS="${j32:+-m32}";;
	raspberry|widows);;
esac

case "$jplatform" in
	linux) SRC_ASM="${j32:+$SRC_ASM_LINUX32}${j64:+$SRC_ASM_LINUX}";;
	raspberry) SRC_ASM="${j32:+$SRC_ASM_RASPI32}${j64:+$SRC_ASM_RASPI}";;
	darwin) SRC_ASM="${j32:+$SRC_ASM_MAC32}${j64:+$SRC_ASM_MAC}";;
	windows) SRC_ASM="${j32:+$SRC_ASM_WIN32}${j64:+$SRC_ASM_WIN}";;
esac

case "$jplatform" in
	raspberry) OBJS_AESARM="${j64:+aes-arm.o}";;
	linux|darwin|windows);;
esac

CFLAGS_SIMD="${javx:+-mavx} ${javx2:+-mavx2 -mfma}"
OBJS_FMA="${javx:+gemm_int-fma.o}"

# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
# slower, use 387 fpu and truncate extra precision
# CFLAGS="${CFLAGS:-} -m32 -ffloat-store "
if [ "$jplatform" = "windows" ]; then
	DLLOBJS="jdll.o"
	LIBJDEF="../../../../dllsrc/jdll2.def"
	[ "${jolecom:-}" ] && DLLOBJS="$DLLOBJS jdllcomx.o"
	[ "${jolecom:-}" ] && LIBJDEF="../../../../dllsrc/jdll.def"
	[ "${jolecom:-}" ] && DOLECOM="-DOLECOM"
	LIBJRES="jddlres.o"
	OBJS_ASM="${j32:+$OBJS_ASM_WIN32}${j64:+$OBJS_ASM_WIN}"
fi

case "${jplatform}_$j64x" in
	linux_j32|linux_j64|linux_j64avx|linux_j64avx2);;
	raspberry_j32|raspberry_j64);;
	darwin_j32|darwin_j64|darwin_j64avx|darwin_j64avx2);;
	windows_j32|windows_j64|windows_j64avx|windows_j64avx2);;
	*) >&2 echo "ERROR: Incompatible parameter set: $jplatform $j64x"; exit 1;;
esac

cat <<-VARS
CFLAGS=${CFLAGS:-}
LDFLAGS=${LDFLAGS:-}
TARGET=${TARGET:-}
CFLAGS_SIMD=${CFLAGS_SIMD:-}
GASM_FLAGS=${GASM_FLAGS:-}
DLLOBJS=${DLLOBJS:-}
LIBJDEF=${LIBJDEF:-}
LIBJRES=${LIBJRES:-}
OBJS_FMA=${OBJS_FMA:-}
OBJS_AESNI=${OBJS_AESNI:-}
OBJS_AESARM=${OBJS_AESARM:-}
OBJS_ASM=${OBJS_ASM:-}
SRC_ASM=${SRC_ASM:-}
jplatform=${jplatform:-}
j64x=${j64x:-}
VARS

jmake=$(dirname "$0")
jsource=$jmake/..

[ -r "$jsource/jsrc/jversion.h" ] || cp "$jsource/jsrc/jversion-x.h" "$jsource/jsrc/jversion.h"
mkdir -p "$jsource/bin/$jplatform/$j64x"
mkdir -p "$jmake/obj/$jplatform/$j64x/"
cp makefile-libj "obj/$jplatform/$j64x/."

export CC CFLAGS LDFLAGS TARGET CFLAGS_SIMD GASM_FLAGS DLLOBJS LIBJDEF LIBJRES OBJS_FMA OBJS_AESNI OBJS_AESARM OBJS_ASM SRC_ASM jplatform j64x
make -C "$jmake/obj/$jplatform/$j64x/" -f makefile-libj
