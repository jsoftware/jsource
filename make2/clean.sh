#!/bin/sh
# rm all *.o for clean builds - makefile dependencies are not set

realpath()
{
 oldpath=`pwd`
 if ! cd $1 &> /dev/null; then
  cd ${1##*/} &> /dev/null
  echo $( pwd -P )/${1%/*}
 else
  pwd -P
 fi
 cd $oldpath > /dev/null
}

cd "$(realpath "$0")"
echo "entering `pwd`"

find ../jsrc -name "*.o" -type f -delete
find obj -name "*.o" -type f -delete
