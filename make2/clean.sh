#!/bin/sh
# rm all *.o for clean builds - makefile dependencies are not set
set -e

if [ "$1" = "noclean" ] ; then
 exit 0
fi

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

cd "$(realpath $(dirname "$0"))"
echo "entering `pwd`"

find .. -not -path "*/.*" -not -path "../openssl-asm/*" -not -path "../asm/*" \( -name "*.o" -o -name "*.tmp" \) -type f -delete || true
find .. -not -path "*/.*" -not -path "../openssl-asm/*" -not -path "../asm/*" -name "*.dSYM" -type d -delete || true
