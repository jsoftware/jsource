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
else
  echo "argument is linux|darwin|raspberry|openbsd|freebsd"
  exit 1
fi
if [ "`uname -m`" != "armv6l" ] && [ "`uname -m`" != "i386" ] && [ "`uname -m`" != "i686" ] ; then
 m64=1
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

if [ $m64 -eq 1 ]; then
ls -l j64
j64/jconsole -lib libj.$ext testga.ijs
else
ls -l j32
j32/jconsole -lib libj.$ext testga.ijs
fi
if [ $1 = "darwin" ]; then
if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX)" -ne 0 ] && [ -f "j64/libjavx.$ext" ] ; then
 j64/jconsole -lib libjavx.$ext testga.ijs
fi
if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ] ; then
 j64/jconsole -lib libjavx2.$ext testga.ijs
fi
if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ] ; then
 j64/jconsole -lib libjavx512.$ext testga.ijs
fi
elif [ $1 = "linux" ]; then
if [ "$(cat /proc/cpuinfo | grep -c avx)" -ne 0 ] && [ -f "j64/libjavx.$ext" ] ; then
  j64/jconsole -lib libjavx.$ext testga.ijs
fi
if [ "$(cat /proc/cpuinfo | grep -c avx2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ] ; then
  j64/jconsole -lib libjavx2.$ext testga.ijs
fi
if [ "$(cat /proc/cpuinfo | grep -c avx512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ] ; then
  j64/jconsole -lib libjavx512.$ext testga.ijs
fi
if [ -f "j32/libj.$ext" ] ; then
  j32/jconsole -lib libj.$ext testga.ijs
fi
elif [ $1 = "openbsd" ] || [ $1 = "freebsd" ] ; then
if [ "$(cat /var/run/dmesg.boot | grep -c AVX)" -ne 0 ] && [ -f "j64/libjavx.$ext" ] ; then
  j64/jconsole -lib libjavx.$ext testga.ijs
fi
if [ "$(cat /var/run/dmesg.boot | grep -c AVX2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ] ; then
  j64/jconsole -lib libjavx2.$ext testga.ijs
fi
if [ "$(cat /var/run/dmesg.boot | grep -c AVX512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ] ; then
  j64/jconsole -lib libjavx512.$ext testga.ijs
fi
fi
if [ $m64 -eq 1 ]; then
ls -l j64
j64/jamalgam testga.ijs
else
ls -l j32
j32/jamalgam testga.ijs
fi
