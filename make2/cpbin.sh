#!/bin/sh
set -e

_DEBUG="${_DEBUG:=0}"
if [ $_DEBUG -eq 1 ]; then
 DEBUGDIR="-debug"
else
 DEBUGDIR=
fi

realpath() {
 oldpath=$(pwd)
 if ! cd $1 > /dev/null 2>&1; then
  cd ${1##*/} > /dev/null 2>&1
  echo $(pwd -P)/${1%/*}
 else
  pwd -P
 fi
 cd $oldpath > /dev/null 2>&1
}

cd "$(realpath $(dirname "$0"))"
echo "entering $(pwd)"

# copy binaries in bin/ to jlibrary/bin

eval "$(./jplatform64.sh)"
jplatform64="$jplatform"/"$j64x"
echo \# jplatform $jplatform64

cop() {
 # $1 src
 # $2 srclib
 # $3 dest
 # $4 destlib
 # MUST rename/remove object first; overwrite cause cache error
 if [ -f "../bin/${jplatform64}$DEBUGDIR/$2" ]; then
  if [ -f "../jlibrary/$3/$4" ]; then
   mv -f "../jlibrary/$3/$4" "/tmp/$4.old.$$" || true
  fi
  echo \# cp "../bin/${jplatform64}$DEBUGDIR/$2" "../jlibrary/$3/$4"
  cp "../bin/${jplatform64}$DEBUGDIR/$2" "../jlibrary/$3/$4" || true
 fi
}

case "$jplatform" in
 darwin)
  # copy libgmp to bin
  if ! [ -f "../jlibrary/bin/libgmp.dylib" ]; then
   cp ../mpir/apple/macos/libgmp.dylib ../jlibrary/bin/libgmp.dylib
  fi

  # macos 64-bit
  if [ -f "../bin/${jplatform}/j64$DEBUGDIR/jconsole" ] && [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/jconsole" ]; then
   # fat binary
   if [ -f "../jlibrary/bin/jconsole" ]; then
    mv -f "../jlibrary/bin/jconsole" "/tmp/jconsole.old.$$"
   fi
   echo \# lipo "../bin/${jplatform}/j64$DEBUGDIR/jconsole" "../bin/${jplatform}/j64arm$DEBUGDIR/jconsole" -create -output "../jlibrary/bin/jconsole-mac"
   lipo "../bin/${jplatform}/j64$DEBUGDIR/jconsole" "../bin/${jplatform}/j64arm$DEBUGDIR/jconsole" -create -output "../jlibrary/bin/jconsole-mac"
   cp ../jlibrary/bin/jconsole-mac ../jlibrary/bin/jconsole
  elif [ -f "../bin/${jplatform}/j64$DEBUGDIR/jconsole" ]; then
   cop j64 jconsole bin jconsole-mac
   cp ../jlibrary/bin/jconsole-mac ../jlibrary/bin/jconsole
  elif [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/jconsole" ]; then
   cop j64arm jconsole bin jconsole-mac
   cp ../jlibrary/bin/jconsole-mac ../jlibrary/bin/jconsole
  fi

  if [ -f "../bin/${jplatform}/j64$DEBUGDIR/libtsdll.dylib" ] && [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/libtsdll.dylib" ]; then
   # fat binary
   if [ -f "../jlibrary/bin/libtsdll.dylib" ]; then
    mv -f "../jlibrary/bin/libtsdll.dylib" "/tmp/libtsdll.dylib.old.$$"
   fi
   echo \# lipo "../bin/${jplatform}/j64$DEBUGDIR/libtsdll.dylib" "../bin/${jplatform}/j64arm$DEBUGDIR/libtsdll.dylib" -create -output "../jlibrary/bin/libtsdll.dylib"
   lipo "../bin/${jplatform}/j64$DEBUGDIR/libtsdll.dylib" "../bin/${jplatform}/j64arm$DEBUGDIR/libtsdll.dylib" -create -output "../jlibrary/bin/libtsdll.dylib"
  elif [ -f "../bin/${jplatform}/j64$DEBUGDIR/libtsdll.dylib" ]; then
   cop j64 libtsdll.dylib bin libtsdll.dylib
  elif [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/libtsdll.dylib" ]; then
   cop j64arm libtsdll.dylib bin libtsdll.dylib
  fi

  if [ -f "../bin/${jplatform}/j64$DEBUGDIR/libj.dylib" ] && [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/libj.dylib" ]; then
   # fat binary
   if [ -f "../jlibrary/bin/libj.dylib" ]; then
    mv -f "../jlibrary/bin/libj.dylib" "/tmp/libj.dylib.old.$$"
   fi
   echo \# lipo "../bin/${jplatform}/j64$DEBUGDIR/libj.dylib" "../bin/${jplatform}/j64arm$DEBUGDIR/libj.dylib" -create -output "../jlibrary/bin/libj.dylib"
   lipo "../bin/${jplatform}/j64$DEBUGDIR/libj.dylib" "../bin/${jplatform}/j64arm$DEBUGDIR/libj.dylib" -create -output "../jlibrary/bin/libj.dylib"
  elif [ -f "../bin/${jplatform}/j64$DEBUGDIR/libj.dylib" ]; then
   cop j64 libj.dylib bin libj.dylib
  elif [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/libj.dylib" ]; then
   cop j64arm libj.dylib bin libj.dylib
  fi

  cop j64avx2 libj.dylib bin libjavx2.dylib
  cop j64avx512 libj.dylib bin libjavx512.dylib

  if [ -f "../bin/${jplatform}/j64$DEBUGDIR/jamalgam" ] && [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/jamalgam" ]; then
   # fat binary
   if [ -f "../jlibrary/bin/jamalgam" ]; then
    mv -f "../jlibrary/bin/jamalgam" "/tmp/jamalgam.old.$$"
   fi
   echo \# lipo "../bin/${jplatform}/j64$DEBUGDIR/jamalgam" "../bin/${jplatform}/j64arm$DEBUGDIR/jamalgam" -create -output "../jlibrary/bin/jamalgam-mac"
   lipo "../bin/${jplatform}/j64$DEBUGDIR/jamalgam" "../bin/${jplatform}/j64arm$DEBUGDIR/jamalgam" -create -output "../jlibrary/bin/jamalgam-mac"
   cp ../jlibrary/bin/jamalgam-mac ../jlibrary/bin/jamalgam
  elif [ -f "../bin/${jplatform}/j64$DEBUGDIR/jamalgam" ]; then
   cop j64 jamalgam bin jamalgam-mac
   cp ../jlibrary/bin/jamalgam-mac ../jlibrary/bin/jamalgam
  elif [ -f "../bin/${jplatform}/j64arm$DEBUGDIR/jamalgam" ]; then
   cop j64arm jamalgam bin jamalgam-mac
   cp ../jlibrary/bin/jamalgam-mac ../jlibrary/bin/jamalgam
  fi
  ;;

 windows)
  case "$j64x" in
   j32)
    # x86 arm32
    cop $j64x jconsole.exe bin32 jconsole.exe
    cop $j64x jamalgam.exe bin32 jamalgam.exe
    cop $j64x tsdll.dll bin32 tsdll.dll
    cop $j64x j.dll bin32 j.dll
    ;;
   j64arm)
    # arm64
    cop $j64x jconsole.exe bin jconsole.exe
    cop $j64x jamalgam.exe bin jamalgam.exe
    cop $j64x tsdll.dll bin tsdll.dll
    cop $j64x j.dll bin j.dll
    ;;
   j64avx512)
    # x86_64
    cop $j64x jconsole.exe bin jconsole.exe
    cop $j64x jamalgam.exe bin jamalgam.exe
    cop $j64x tsdll.dll bin tsdll.dll
    cop $j64x j.dll bin javx512.dll
    ;;
   j64avx2)
    # x86_64
    cop $j64x jconsole.exe bin jconsole.exe
    cop $j64x jamalgam.exe bin jamalgam.exe
    cop $j64x tsdll.dll bin tsdll.dll
    cop $j64x j.dll bin javx2.dll
    ;;
   *)
    # x86_64
    cop $j64x jconsole.exe bin jconsole.exe
    cop $j64x jamalgam.exe bin jamalgam.exe
    cop $j64x tsdll.dll bin tsdll.dll
    cop $j64x j.dll bin j.dll
    ;;
  esac
  ;;

 *)
  case "$j64x" in
   j32)
    # x86 arm32
    cop $j64x jconsole bin32 jconsole
    cop $j64x jamalgam bin32 jamalgam
    cop $j64x libtsdll.so bin32 libtsdll.so
    cop $j64x libj.so bin32 libj.so
    ;;
   j64arm)
    # arm64
    cop $j64x jconsole bin jconsole
    cop $j64x jamalgam bin jamalgam
    cop $j64x libtsdll.so bin libtsdll.so
    cop $j64x libj.so bin libj.so
    ;;
   j64avx512)
    # x86_64
    cop $j64x jconsole bin jconsole
    cop $j64x jamalgam bin jamalgam
    cop $j64x libtsdll.so bin libtsdll.so
    cop $j64x libj.so bin libjavx512.so
    ;;
   j64avx2)
    # x86_64
    cop $j64x jconsole bin jconsole
    cop $j64x jamalgam bin jamalgam
    cop $j64x libtsdll.so bin libtsdll.so
    cop $j64x libj.so bin libjavx2.so
    ;;
   *)
    # x86_64
    cop $j64x jconsole bin jconsole
    cop $j64x jamalgam bin jamalgam
    cop $j64x libtsdll.so bin libtsdll.so
    cop $j64x libj.so bin libj.so
    ;;
  esac
  ;;
esac
