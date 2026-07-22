#!/bin/sh
set -e

cd "$(dirname "$0")"
echo "entering $(pwd)"

unameop=$(uname -o || uname -s)
eval "$(./jplatform64.sh)"

OPTL=${OPTL:="-O2"}
OPTLD=${OPTLD:="-O2"}

if [ "" = "$CFLAGS" ]; then
 # OPTLEVEL will be merged back into CFLAGS, further down
 # OPTLEVEL is probably overly elaborate, but it works
 case "$_DEBUG" in
  1|2|3)
   if [ "$OPTLD" = "-O0" ]; then
    OPTLEVEL=" $OPTLD -g -DOPTMO0 "
   else
    OPTLEVEL=" $OPTLD -g "
   fi
   DEBUG=1
   NASM_FLAGS="-g"
   ;;
  *)
   if [ "$OPTL" = "-O0" ]; then
    OPTLEVEL=" $OPTL -DOPTMO0 "
   else
    OPTLEVEL=" $OPTL "
   fi
   DEBUG=0
   NASM_FLAGS=""
   ;;
 esac
else
 case "$CFLAGS" in
  *-O0*) OPTLEVEL=" -DOPTMO0 " ;;
 esac
 case "$CFLAGS" in
  *\ -g\ *)
   DEBUG=1
   NASM_FLAGS="-g"
   ;;
  *\ -ggdb\ *)
   DEBUG=1
   NASM_FLAGS="-g"
   ;;
  *)
   DEBUG=0 ;;
 esac
fi
echo "jplatform=$jplatform"
echo "j64x=$j64x"

LIBBACKTRACE="${LIBBACKTRACE:=1}"
USE_READLINE="${USE_READLINE:=1}"
USE_LINENOISE="${USE_LINENOISE:=1}"
echo "USE_READLINE=$USE_READLINE"

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in gcc 4
# too early to move main linux release package to gcc 5

case "$jplatform/$j64x" in
 darwin/j64iphoneos)
  LIBBACKTRACE=0
  USE_READLINE=0
  USE_LINENOISE=0
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
  LIBBACKTRACE=0
  USE_READLINE=0
  USE_LINENOISE=0
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
  LIBBACKTRACE=0
   make=gmake
   ;;
 freebsd/*)
  LIBBACKTRACE=0
   make=gmake
   ;;
 windows/*)
  LIBBACKTRACE=0
  USE_READLINE=0
  ;;
 wasm*)
  LIBBACKTRACE=0
  USE_READLINE=0
  USE_LINENOISE=0
  USE_OPENMP=0
  USE_PYXES=0
  ;;
esac
case "$j64x" in
 j32*) USE_PYXES="${USE_PYXES:=0}" ;;
 *) USE_PYXES="${USE_PYXES:=1}" ;;
esac
make="${make:=make}"

CC=${CC-$(which cc clang gcc 2> /dev/null | head -n1 | xargs basename)}
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

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
 # gcc
 common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing -flax-vector-conversions \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wno-cast-function-type \
 -Wno-clobbered \
 -Wno-empty-body \
 -Wno-format-overflow \
 -Wno-implicit-fallthrough \
 -Wno-maybe-uninitialized \
 -Wno-missing-field-initializers \
 -Wno-parentheses \
 -Wno-pointer-sign \
 -Wno-shift-negative-value \
 -Wno-sign-compare \
 -Wno-type-limits \
 -Wno-uninitialized \
 -Wno-unused-parameter \
 -Wno-unused-value \
 $CFLAGS"

else
 # clang
 common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wsign-compare \
 -Wtautological-constant-out-of-range-compare \
 -Wuninitialized \
 -Wno-char-subscripts \
 -Wno-consumed \
 -Wno-delete-non-abstract-non-virtual-dtor \
 -Wno-empty-body \
 -Wno-gnu-folding-constant \
 -Wno-implicit-float-conversion \
 -Wno-implicit-int-float-conversion \
 -Wno-int-in-bool-context \
 -Wno-missing-braces \
 -Wno-parentheses \
 -Wno-pass-failed \
 -Wno-pointer-sign \
 -Wno-string-plus-int \
 -Wno-unknown-pragmas \
 -Wno-unsequenced \
 -Wno-unused-function \
 -Wno-unused-parameter \
 -Wno-unused-value \
 -Wno-unused-variable \
 $CFLAGS"

fi

TARGET=jconsole

case "$jplatform/$j64x" in
 *32*) USE_EMU_AVX=0 ;;
 *) USE_EMU_AVX="${USE_EMU_AVX:=1}" ;;
esac
if [ $USE_EMU_AVX -eq 1 ]; then
 common="$common -DEMU_AVX2=1"
else
 common="$common -DEMU_AVX2=0"
fi

if [ $USE_PYXES -eq 1 ]; then
 common="$common -DPYXES=1"
else
 common="$common -DPYXES=0"
fi

if [ "${USE_GMP_H:=1}" -eq 1 ]; then
 common="$common -I../mpir/include"
fi

if [ -n "$MAX_ERRORS" ]; then
  if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
   common="$common -fmax-errors=$MAX_ERRORS "
  else
   common="$common -ferror-limit=$MAX_ERRORS "
  fi
fi

if [ $jplatform = wasm ]; then
 true
elif [ "$USE_READLINE" -eq 1 ]; then
 common="$common -DREADLINE"
elif [ "$USE_LINENOISE" -eq 1 ]; then
 common="$common -DREADLINE -DUSE_LINENOISE"
 OBJSLN="linenoise.o"
fi

if [ $LIBBACKTRACE -eq 1 ]; then
# elf.o
# pecoff.o
# alloc.o -- alternative to mmap.o
# read.o -- alternative to mmapio.o; probably mmap is more robust in this case
# not included
#  alloc.o
#  allocfail.o
#  instrumented_alloc.o
#  nounwind.o
#  pecoff.o
#  read.o
#  testlib.o

case "$jplatform/$j64x" in
 darwin/*)
  BACKTRACE_OBJS=" \
   ../libbacktrace/atomic.o \
   ../libbacktrace/backtrace.o \
   ../libbacktrace/dwarf.o \
   ../libbacktrace/fileline.o \
   ../libbacktrace/mmap.o \
   ../libbacktrace/mmapio.o \
   ../libbacktrace/posix.o \
   ../libbacktrace/print.o \
   ../libbacktrace/simple.o \
   ../libbacktrace/sort.o \
   ../libbacktrace/state.o \
   ../libbacktrace/macho.o "
  ;;
 *)
  BACKTRACE_OBJS=" \
   ../libbacktrace/atomic.o \
   ../libbacktrace/backtrace.o \
   ../libbacktrace/dwarf.o \
   ../libbacktrace/fileline.o \
   ../libbacktrace/mmap.o \
   ../libbacktrace/mmapio.o \
   ../libbacktrace/posix.o \
   ../libbacktrace/print.o \
   ../libbacktrace/simple.o \
   ../libbacktrace/sort.o \
   ../libbacktrace/state.o \
   ../libbacktrace/elf.o "
  ;;
esac
fi

case "$jplatform/$j64x" in

 linux/j32)
  CFLAGS="$common -march=i686 -m32 -msse2 -mfpmath=sse "
  LDFLAGS=" -m32 -ldl $LDTHREAD "
  ;;
 linux/j64*)
  CFLAGS="$common"
  LDFLAGS=" -ldl $LDTHREAD "
  ;;
 raspberry/j32*)
  CFLAGS="$common -std=gnu99 -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI"
  LDFLAGS=" -ldl $LDTHREAD "
  ;;
 raspberry/j64*)
  CFLAGS="$common -march=armv8-a+crc -DRASPI"
  LDFLAGS=" -ldl $LDTHREAD "
  ;;
 openbsd/j64arm)
  CFLAGS="$common -march=armv8-a+crc"
  LDFLAGS=" $LDTHREAD "
  ;;
 openbsd/j64*)
  CFLAGS="$common"
  LDFLAGS=" $LDTHREAD "
  ;;
 freebsd/j64arm)
  CFLAGS="$common -march=armv8-a+crc"
  LDFLAGS=" $LDTHREAD "
  ;;
 freebsd/j64*)
  CFLAGS="$common"
  LDFLAGS=" $LDTHREAD "
  ;;
 darwin/j64arm) # darwin arm
  CFLAGS="$common $macmin -march=armv8-a+crc "
  LDFLAGS=" -Wl,-stack_size,0xc00000 -ldl $LDTHREAD $macmin "
  ;;
 darwin/j64iphoneos) # iphone
  CFLAGS="$common $macmin -march=armv8-a+crc "
  LDFLAGS=" -Wl,-stack_size,0xc00000 -ldl $LDTHREAD $macmin "
  ;;
 darwin/j64iphoneimulator) # iphone simulator
  CFLAGS="$common $macmin "
  LDFLAGS=" -Wl,-stack_size,0xc00000 -ldl $LDTHREAD $macmin "
  ;;
 darwin/j64*)
  CFLAGS="$common $macmin "
  LDFLAGS=" -ldl $LDTHREAD $macmin "
  ;;
 windows/j32)
  TARGET=jconsole.exe
  CFLAGS="$common -Wno-psabi -march=i686 -m32 -msse2 -mfpmath=sse -D_FILE_OFFSET_BITS=64 -D_WIN32 "
  LDFLAGS=" -m32 -Wl,--stack=0xc00000,--subsystem,console -static-libgcc $LDTHREAD "
  LIBJRES=" ../makevs/jconsole/jconsole.res "
  ;;
 windows/j64*)
  TARGET=jconsole.exe
  CFLAGS="$common -D_FILE_OFFSET_BITS=64 -D_WIN32 -D_WIN64 "
  LDFLAGS=" -Wl,--stack=0xc00000,--subsystem,console -static-libgcc $LDTHREAD "
  LIBJRES=" ../makevs/jconsole/jconsole.res "
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
export AR LIBBACKTRACE BACKTRACE_OBJS CC CFLAGS LDFLAGS TARGET OBJSLN LIBJRES jplatform j64x
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
cd ../jsrc/
if [ "1" != "$NOCLEAN" ] && [ "$1" != "noclean" ]; then
 $make -f ../make2/makefile-jconsole clean
fi
$make -f ../make2/makefile-jconsole all
retval=$?
cd -
exit $retval
