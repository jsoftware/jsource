#!/bin/bash
#
# test linux/macOS on github actions
#
# argument is linux|darwin|raspberry
#
# current Linux github builder supports avx512
# cpu is Intel(R) Xeon(R) Platinum 8272CL CPU @ 2.60GHz
#
# current macOS github builder only supports avx
# cpu is Intel(R) Xeon(R) CPU E5-2697 v2 @ 2.70GHz

set -e

if [ $1 == "linux" ]; then
  ext="so"
elif [ $1 == "raspberry" ]; then
  ext="so"
elif [ "$1" == "darwin" ]; then
  ext="dylib"
else
  echo "argument is linux|darwin|raspberry"
  exit 1
fi
if [ "`uname -m`" != "armv6l" ] && [ "`uname -m`" != "i386" ] && [ "`uname -m`" != "i686" ] ; then
 m64=1
else
 m64=0
fi
if [ "$1" == "darwin" ]; then
sysctl -a | grep cpu
else
cat /proc/cpuinfo
fi

if [ $m64 -eq 1 ]; then
ls -l j64
j64/jconsole -lib libj.$ext testga.ijs
else
ls -l j32
j32/jconsole -lib libj.$ext testga.ijs
fi
if [ $1 != "raspberry" ]; then
j64/jconsole -lib libjavx.$ext testga.ijs
fi

if [ $1 == "linux" ]; then
  j64/jconsole -lib libjavx2.$ext testga.ijs
  j64/jconsole -lib libjavx512.$ext testga.ijs
  j32/jconsole -lib libj.$ext testga.ijs
fi
