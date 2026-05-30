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

set -evx

export jplatform="$1"
if [ "$1" = "linux" ]; then
 libj="libj"
 ext="so"
 ext1=""
elif [ "$1" = "raspberry" ]; then
 libj="libj"
 ext="so"
 ext1=""
elif [ "$1" = "darwin" ]; then
 libj="libj"
 ext="dylib"
 ext1=""
elif [ "$1" = "android" ]; then
 libj="libj"
 ext="so"
 ext1=""
elif [ "$1" = "openbsd" ]; then
 libj="libj"
 ext="so"
 ext1=""
elif [ "$1" = "freebsd" ]; then
 libj="libj"
 ext="so"
 ext1=""
elif [ "$1" = "windows" ]; then
 libj="j"
 ext="dll"
 ext1=".exe"
elif [ "$1" = "wasm" ]; then
 libj="libj"
 ext=""
 ext1=""
else
 echo "argument is linux|windows|darwin|raspberry|android|openbsd|freebsd|wasm"
 exit 1
fi
uname -a
unameop=$(uname -o || uname -s)
echo $unameop
uname -m
if [ "$2" = "arm64" ] || [ "$2" = "x86_64" ]; then
 m64=1
elif [ "$2" = "armv6l" ] || [ "$2" = "i386" ] || [ "$2" = "wasm32" ]; then
 export j64x=j32
 m64=0
else
 echo "argument is [arm64|armv6l|i386|x86_64|wasm32]"
 exit 1
fi

which gdb
which lldb

echo "DEBUGCMD: $DEBUGCMD"
echo "_DEBUG: $_DEBUG"
echo "USE_EMU_AVX: $USE_EMU_AVX"
echo "USE_PYXES: $USE_PYXES"

if [ -n "$_DEBUG" ] && [ "$_DEBUG" != "0" ]; then
 if [ -z "$DEBUGCMD" ]; then
  # default to lldb
  DEBUGCMD=lldb
 fi
 if [ "$DEBUGCMD" = "gdb" ]; then
  DB1="$DEBUGCMD -batch -return-child-result -ex run -ex bt -ex quit --args"
 elif [ "$DEBUGCMD" = "gdb-multiarch" ]; then
  DB1="$DEBUGCMD -batch -return-child-result -ex \"set architecture $DEBUGCPU\" -ex run -ex bt -ex quit --args"
 elif [ "$DEBUGCMD" = "lldb" ]; then
  DB1="$DEBUGCMD -b -o run -k bt -k quit --"
 else
  echo "unsupported debug command: $DB1"
  exit 1
 fi
 $DEBUGCMD --version
 echo "debug command: $DB1"
else
 DB1=
fi

dest=$1

A=jlibrary
B=jlibrary/bin
C=jlibrary/bin32

if ([ "$unameop" = "Linux" ] || [ "$unameop" = "GNU/Linux" ]); then
 cat /proc/cpuinfo || true
elif [ "$unameop" = "Darwin" ]; then
 sysctl -a | grep cpu
elif [ "$unameop" = "OpenBSD" ] || [ "$unameop" = "FreeBSD" ]; then
 grep -i cpu /var/run/dmesg.boot
elif [ "$unameop" = "MINGW64" ] || [ "$unameop" = "MINGW32" ] || [ "$unameop" = "CYGWIN" ] || [ "$unameop" = "MSYS" ] || [ "$unameop" = "Msys" ]; then
 systeminfo
fi
ulimit -a || true

if [ "$1" = "wasm" ]; then
 ls -l $C
 cd $C
 node jamalgam.js
 exit $?
fi

# avx2
if [ "$2" = "x86_64" ]; then
 if [ "$1" = "darwin" ]; then
  if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX2)" -ne 0 ] && [ -f "$B/${libj}avx2.${ext}" ]; then
   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx2.${ext} script/testga.ijs
  fi
 elif [ "$1" = "linux" ]; then
  if [ "$(cat /proc/cpuinfo | grep -c avx2)" -ne 0 ] && [ -f "$B/${libj}avx2.${ext}" ]; then
   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx2.${ext} script/testga.ijs
  fi
 elif [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ]; then
  if [ "$(cat /var/run/dmesg.boot | grep -c AVX2)" -ne 0 ] && [ -f "$B/${libj}avx2.${ext}" ]; then
   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx2.${ext} script/testga.ijs
  fi
 elif [ "$1" = "windows" ]; then
  if [ -f "$B/${libj}avx2.${ext}" ]; then
   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx2.${ext} script/testga.ijs
  fi
 fi
fi

# non avx2
if [ $m64 -eq 1 ]; then
 ls -l $B
 if [ "$1" = "darwin" ] && [ "$(uname -m)" = "arm64" ]; then
  LC_ALL=fr_FR.UTF-8 arch -arm64 $DB1 $B/jconsole${ext1} -lib ${libj}.${ext} script/testga.ijs
  LC_ALL=fr_FR.UTF-8 arch -x86_64 $DB1 $B/jconsole${ext1} -lib ${libj}.${ext} script/testga.ijs
 else
  LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}.${ext} script/testga.ijs
 fi
else
 ls -l $C
 LC_ALL=fr_FR.UTF-8 $DB1 $C/jconsole${ext1} -lib ${libj}.${ext} script/testga.ijs
fi

# avx512
if [ "$2" = "x86_64" ]; then
 if [ "$1" = "darwin" ]; then
  if [ "$(sysctl -a | grep machdep.cpu | grep -c AVX512)" -ne 0 ] && [ -f "$B/${libj}avx512.${ext}" ]; then
   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx512.${ext} script/testga.ijs
  fi
 elif [ "$1" = "linux" ]; then
  if [ "$(cat /proc/cpuinfo | grep -c avx512)" -ne 0 ] && [ -f "$B/${libj}avx5122.${ext}" ]; then
   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx512.${ext} script/testga.ijs
  fi
 elif [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ]; then
  # if [ "$(cat /var/run/dmesg.boot | grep -c AVX512)" -ne 0 ] && [ -f "$B/${libj}avx512.${ext}" ]; then
  #   LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx512.${ext} script/testga.ijs
  # fi
  true
 elif [ "$1" = "windows" ]; then
  # no way to detect avx512 capacity
  # if [ -f "$B/${libj}avx512.${ext}" ]; then
  #    LC_ALL=fr_FR.UTF-8 $DB1 $B/jconsole${ext1} -lib ${libj}avx512.${ext} script/testga.ijs
  #   fi
  # fi
  true
 fi
fi
touch jobdone || true
