#!/bin/sh
#
# test linux/macOS on github actions
#
# argument is linux|darwin|raspberry|openbsd|freebsd
# openbsd/freebsd is experimental
#
# current Linux github builder supports avx512
# when cpu is Intel(R) Xeon(R) Platinum 8272CL CPU @ 2.60GHz
#
# current macOS github builder only supports avx
# cpu is Intel(R) Xeon(R) CPU E5-2697 v2 @ 2.70GHz

set -e

if [ $1 = "linux" ]; then
  ext="so"
elif [ $1 = "raspberry" ]; then
  ext="so"
elif [ "$1" = "darwin" ]; then
  ext="dylib"
elif [ $1 = "openbsd" ]; then
  ext="so"
elif [ $1 = "freebsd" ]; then
  ext="so"
elif [ $1 = "wasm" ]; then
  ext=""
else
  echo "argument is linux|darwin|raspberry|openbsd|freebsd|wasm"
  exit 1
fi
if [ "`uname -m`" != "armv6l" ] && [ "`uname -m`" != "i386" ] && [ "`uname -m`" != "i686" ] ; then
if [ "$1" = "wasm" ]; then
 m64=0
else
 m64=1
fi
else
 m64=0
fi
if [ "$1" = "darwin" ]; then
sysctl -a | grep cpu
elif [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ]; then
grep -i cpu /var/run/dmesg.boot
else
cat /proc/cpuinfo
fi
ulimit -a || true

if [ "$1" = "wasm" ]; then
ls -l j32
cd j32
node jamalgam.js
exit 0
fi

if [ $m64 -eq 1 ]; then
ls -l j64
LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libj.$ext testga.ijs
else
ls -l j32
LC_ALL=fr_FR.UTF-8 j32/jconsole -lib libj.$ext testga.ijs
fi
if [ $1 = "darwin" ]; then
if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ] ; then
 LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx2.$ext testga.ijs
fi
if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ] ; then
 LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx512.$ext testga.ijs
fi
elif [ $1 = "linux" ]; then
if [ "$(cat /proc/cpuinfo | grep -c avx2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ] ; then
  LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx2.$ext testga.ijs
elif [ -f "$SDE_PATH/sde" ] && [ -f "j64/libjavx2.$ext" ] ; then
  LC_ALL=fr_FR.UTF-8 $SDE_PATH/sde -skx -- j64/jconsole -lib libjavx2.$ext testga.ijs
fi
if [ "$(cat /proc/cpuinfo | grep -c avx512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ] ; then
  LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx512.$ext testga.ijs
elif [ -f "$SDE_PATH/sde" ] && [ -f "j64/libjavx512.$ext" ] ; then
  #LC_ALL=fr_FR.UTF-8 $SDE_PATH/sde -skx -- j64/jconsole -lib libjavx512.$ext testga.ijs
  echo "Not running AVX512 tests due to missing hardware support" # sde doesn't like fsgsbase?
fi
if [ -f "j32/libj.$ext" ] ; then
  LC_ALL=fr_FR.UTF-8 j32/jconsole -lib libj.$ext testga.ijs
fi
elif [ $1 = "openbsd" ] || [ $1 = "freebsd" ] ; then
if [ "$(cat /var/run/dmesg.boot | grep -c AVX2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ] ; then
  LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx2.$ext testga.ijs
fi
if [ "$(cat /var/run/dmesg.boot | grep -c AVX512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ] ; then
  LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx512.$ext testga.ijs
fi
fi
if [ $m64 -eq 1 ]; then
ls -l j64
if [ "$1" != "openbsd" ] && [ "$1" != "freebsd" ] ; then
LC_ALL=fr_FR.UTF-8 j64/jamalgam testga.ijs
fi
else
ls -l j32
# LC_ALL=fr_FR.UTF-8 j32/jamalgam testga.ijs
fi
