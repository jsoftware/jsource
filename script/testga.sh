#!/bin/sh
#
# test linux/macOS on github actions
#
# argument is linux|windows|darwin|raspberry|android|openbsd|freebsd|wasm [arm64|armv6l|i386|x86_64|wasm32]
# openbsd/freebsd is experimental
#
# if !x86_64 skip test avx2 and avx512
#
# current Linux github builder supports avx512
# when cpu is Intel(R) Xeon(R) Platinum 8272CL CPU @ 2.60GHz
#
# current macOS github builder only supports avx
# cpu is Intel(R) Xeon(R) CPU E5-2697 v2 @ 2.70GHz

set -e

if [ "$1" = "linux" ]; then
 ext="so"
elif [ $1 = "raspberry" ]; then
 ext="so"
elif [ "$1" = "darwin" ]; then
 ext="dylib"
elif [ $1 = "openbsd" ]; then
 ext="so"
elif [ $1 = "freebsd" ]; then
 ext="so"
elif [ "$1" = "windows" ]; then
 ext="dll"
elif [ $1 = "wasm" ]; then
 ext=""
else
 echo "argument is linux|windows|darwin|raspberry|android|openbsd|freebsd|wasm"
 exit 1
fi
uname -a
uname
uname -m
if [ "$2" = "arm64" ] || [ "$2" = "x86_64" ]; then
 m64=1
elif [ "$2" = "armv6l" ] || [ "$2" = "i386" ] || [ "$2" = "wasm32" ]; then
 m64=0
else
 echo "argument is [arm64|armv6l|i386|x86_64|wasm32]"
 exit 1
fi

dest=$1

if [ "$(uname)" = "Darwin" ]; then
 sysctl -a | grep cpu
elif [ "$(uname)" = "OpenBSD" ] || [ "$(uname)" = "FreeBSD" ]; then
 grep -i cpu /var/run/dmesg.boot
else
 cat /proc/cpuinfo || true
fi
ulimit -a || true

if [ "$1" = "wasm" ]; then
 ls -l j32
 cd j32
 node jamalgam.js
 exit $?
fi

# avx2
if [ "$2 = "x86_64" ]; then
 if [ $1 = "darwin" ]; then
  if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ]; then
   if [ "$_DEBUG" = "3" ]; then
    echo "running debug"
    LC_ALL=fr_FR.UTF-8 lldb -b -o run -k bt -k quit -- j64/jconsole -lib libjavx2.$ext testga.ijs
   else
    LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx2.$ext testga.ijs
   fi
  fi
 elif [ $1 = "linux" ]; then
  if [ "$(cat /proc/cpuinfo | grep -c avx2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ]; then
   if [ "$_DEBUG" = "3" ]; then
    echo "running debug"
    LC_ALL=fr_FR.UTF-8 gdb -batch -return-child-result -ex "run" -ex "thread apply all bt" --args j64/jconsole -lib libjavx2.$ext testga.ijs
   else
    LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx2.$ext testga.ijs
   fi
  fi
 elif [ $1 = "openbsd" ] || [ $1 = "freebsd" ]; then
  if [ "$(cat /var/run/dmesg.boot | grep -c AVX2)" -ne 0 ] && [ -f "j64/libjavx2.$ext" ]; then
   LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx2.$ext testga.ijs
  fi
 fi
fi

# non avx2
if [ $m64 -eq 1 ]; then
 ls -l j64
 if [ $1 = "darwin" ] && [ "$(uname -m)" = "arm64" ]; then
  if [ "$_DEBUG" = "3" ]; then
   echo "running debug"
   LC_ALL=fr_FR.UTF-8 APPLEM1=APPLEM1 arch -arm64 lldb -b -o run -k bt -k quit -- j64/jconsole -lib libj.$ext testga.ijs
   # LC_ALL=fr_FR.UTF-8 APPLEM1=APPLEM1 arch -x86_64 lldb -b -o run -k bt -k quit -- j64/jconsole -lib libj.$ext testga.ijs
  else
   LC_ALL=fr_FR.UTF-8 APPLEM1=APPLEM1 arch -arm64 j64/jconsole -lib libj.$ext testga.ijs
   LC_ALL=fr_FR.UTF-8 APPLEM1=APPLEM1 arch -x86_64 j64/jconsole -lib libj.$ext testga.ijs
  fi
 elif [ $1 = "darwin" ]; then
  # lldb -b -o run -k bt -k quit -- j64/jconsole -lib libj.$ext testga.ijs
  if [ "$_DEBUG" = "3" ]; then
   echo "running debug"
   LC_ALL=fr_FR.UTF-8 lldb -b -o run -k bt -k quit -- j64/jconsole -lib libj.$ext testga.ijs
  else
   LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libj.$ext testga.ijs
  fi
 elif [ "$_DEBUG" = "3" ]; then
  echo "running debug"
  LC_ALL=fr_FR.UTF-8 gdb -batch -return-child-result -ex "run" -ex "thread apply all bt" --args j64/jconsole -lib libj.$ext testga.ijs
 else
  LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libj.$ext testga.ijs
 fi
else
 ls -l j32
 if [ "$_DEBUG" = "3" ]; then
  echo "running debug"
  if [ $1 = "raspberry" ]; then
   LC_ALL=fr_FR.UTF-8 gdb-multiarch -batch -return-child-result -ex "set architecture arm6" -ex "run" -ex "thread apply all bt" --args j32/jconsole -lib libj.$ext testga.ijs
  else
   LC_ALL=fr_FR.UTF-8 gdb -batch -return-child-result -ex "run" -ex "thread apply all bt" --args j32/jconsole -lib libj.$ext testga.ijs
  fi
 else
  LC_ALL=fr_FR.UTF-8 j32/jconsole -lib libj.$ext testga.ijs
 fi
fi

# avx512
if [ "$2 = "x86_64" ]; then
 if [ $1 = "darwin" ]; then
  if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ]; then
   if [ "$_DEBUG" = "3" ]; then
    echo "running debug"
    LC_ALL=fr_FR.UTF-8 lldb -b -o run -k bt -k quit -- j64/jconsole -lib libjavx512.$ext testga.ijs
   else
    LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx512.$ext testga.ijs
   fi
  fi
 elif [ $1 = "linux" ]; then
  if [ "$(cat /proc/cpuinfo | grep -c avx512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ]; then
   if [ "$_DEBUG" = "3" ]; then
    echo "running debug"
    LC_ALL=fr_FR.UTF-8 gdb -batch -return-child-result -ex "run" -ex "thread apply all bt" --args j64/jconsole -lib libjavx512.$ext testga.ijs
   else
    LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx512.$ext testga.ijs
   fi
  fi
 elif [ $1 = "openbsd" ] || [ $1 = "freebsd" ]; then
  if [ "$(cat /var/run/dmesg.boot | grep -c AVX512)" -ne 0 ] && [ -f "j64/libjavx512.$ext" ]; then
   LC_ALL=fr_FR.UTF-8 j64/jconsole -lib libjavx512.$ext testga.ijs
  fi
 fi
fi

