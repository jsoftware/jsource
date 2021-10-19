#!/bin/sh

realpath()
{
 oldpath=`pwd`
 if ! cd $1 > /dev/null 2>&1; then
  cd ${1##*/} > /dev/null 2>&1
  echo $( pwd -P )/${1%/*}
 else
  pwd -P
 fi
 cd $oldpath > /dev/null 2>&1
}

cd "$(realpath "$0")"
echo "entering `pwd`"

if [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ] || [ "$RASPI" = 1 ]; then
jplatform="${jplatform:=raspberry}"
elif [ "`uname`" = "Darwin" ]; then
jplatform="${jplatform:=darwin}"
else
jplatform="${jplatform:=linux}"
fi
if [ "`uname -m`" = "x86_64" ]; then
j64x="${j64x:=j64avx}"
elif [ "`uname -m`" = "aarch64" ]; then
j64x="${j64x:=j64}"
elif [ "`uname -m`" = "arm64" ] && [ -z "${jplatform##*darwin*}" ]; then
j64x="${j64x:=j64arm}"
else
j64x="${j64x:=j32}"
fi

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

if [ -z "${jplatform##*darwin*}" ]; then
if [ -z "${j64x##*j64arm*}" ]; then
macmin="-target arm64-apple-macos11 -mmacosx-version-min=11"
else
macmin="-target x86_64-apple-macos10.6 -mmacosx-version-min=10.6"
fi
fi

if [ "x$CC" = x'' ] ; then
if [ -f "/usr/bin/cc" ]; then
CC=cc
else
if [ -f "/usr/bin/clang" ]; then
CC=clang
else
CC=gcc
fi
fi
export CC
fi
# compiler=`$CC --version | head -n 1`
compiler=$(readlink -f $(command -v $CC) 2> /dev/null || echo $CC)
echo "CC=$CC"
echo "compiler=$compiler"

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
# gcc
common="$OPENMP -fPIC -O2 -fvisibility=hidden -fno-strict-aliasing  \
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
 -Wno-unused-value "

else
# clang
common="$OPENMP -fPIC -O2 -fvisibility=hidden -fno-strict-aliasing \
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
 -Wno-unused-variable "

fi

case $jplatform\_$j64x in

linux_j32)
TARGET=libjnative.so
CFLAGS="$common -m32 -msse2 -mfpmath=sse -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so  -m32 "
;;
linux_j64)
TARGET=libjnative.so
CFLAGS="$common -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
linux_j64avx)
TARGET=libjnative.so
CFLAGS="$common -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
linux_j64avx2)
TARGET=libjnative.so
CFLAGS="$common -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
raspberry_j32)
TARGET=libjnative.so
CFLAGS="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
raspberry_j64)
TARGET=libjnative.so
CFLAGS="$common -march=armv8-a+crc -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LDFLAGS=" -shared -Wl,-soname,libjnative.so "
;;
darwin_j32)
TARGET=libjnative.dylib
CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" -m32 $macmin -dynamiclib "
;;
darwin_j64)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib "
;;
darwin_j64avx)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib "
;;
darwin_j64avx2)
TARGET=libjnative.dylib
CFLAGS="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib "
;;
darwin_j64arm) # darwin arm
TARGET=libjnative.dylib
CFLAGS="$common $macmin -march=armv8-a+crc -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LDFLAGS=" $macmin -dynamiclib "
;;
*)
echo no case for those parameters
exit
esac

echo "CFLAGS=$CFLAGS"

mkdir -p ../bin/$jplatform/$j64x
mkdir -p obj/$jplatform/$j64x/
cp makefile-jnative obj/$jplatform/$j64x/.
export CFLAGS LDFLAGS TARGET jplatform j64x
cd obj/$jplatform/$j64x/
make -f makefile-jnative
cd -
