#!/bin/sh
set -e

cd "$(dirname "$0")"
echo "entering `pwd`"

jplatform64=$(./jplatform64.sh)

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

case "$jplatform64" in
	darwin/j64arm) macmin="-arch arm64 -mmacosx-version-min=11";;
	darwin/*) macmin="-arch x86_64 -mmacosx-version-min=10.6";;
esac

CC=${CC-$(which cc clang gcc 2>/dev/null | head -n1 | xargs basename)}
compiler=$(readlink -f $(which $CC))
echo "CC=$CC"
echo "compiler=$compiler"

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
# gcc
common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing  \
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

USE_PYXES="${USE_PYXES:=1}"
if [ $USE_PYXES -eq 1 ] ; then
common="$common -DPYXES=1"
LDTHREAD=" -pthread "
else
common="$common -DPYXES=0"
fi

if [ -z "${j64x##*32*}" ]; then
USE_EMU_AVX=0
else
USE_EMU_AVX="${USE_EMU_AVX:=1}"
fi
if [ $USE_EMU_AVX -eq 1 ] ; then
common="$common -DEMU_AVX=1"
fi

if [ "${USE_GMP_H:=1}" -eq 1 ] ; then
 common="$common -I../../../../mpir/include"
fi

case $jplatform64 in

linux/j32)
TARGET=libjnative.so
CFLAGS="$common -m32 -msse2 -mfpmath=sse -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so  -m32 "
;;
linux/j6*)
TARGET=libjnative.so
CFLAGS="$common -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
raspberry/j32)
TARGET=libjnative.so
CFLAGS="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
raspberry/j64)
TARGET=libjnative.so
CFLAGS="$common -march=armv8-a+crc -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
darwin/j32)
TARGET=libjnative.dylib
CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" -m32 $macmin -dynamiclib -install_name libjnative.dylib "
;;
darwin/j64)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib -install_name libjnative.dylib "
;;
darwin/j64avx)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib -install_name libjnative.dylib "
;;
darwin/j64avx2)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib -install_name libjnative.dylib "
;;
darwin/j64avx512)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib -install_name libjnative.dylib "
;;
darwin/j64arm) # darwin arm
TARGET=libjnative.dylib
CFLAGS="$common $macmin -march=armv8-a+crc -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib -install_name libjnative.dylib "
;;
*)
echo no case for those parameters
exit
esac

echo "CFLAGS=$CFLAGS"

mkdir -p ../bin/$jplatform64
mkdir -p obj/$jplatform64/
cp makefile-jnative obj/$jplatform64/.
export CFLAGS LDFLAGS TARGET jplatform64
cd obj/$jplatform64/
make -f makefile-jnative
retval=$?
cd -
exit $retval
