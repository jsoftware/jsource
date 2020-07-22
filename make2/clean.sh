#!/bin/sh
# rm all *.o for clean builds - makefile dependencies are not set

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

find ../jsrc -name "*.o" -type f -delete
find ../dllsrc -name "*.o" -type f -delete
find ../sleef/src -name "*.o" -type f -delete
find ../base64 -name "*.o" -type f -delete
find obj -name "*.o" -type f -delete || true
