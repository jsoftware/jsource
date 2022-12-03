#!/bin/bash
#
# test linux/macOS on github actions
#
# argument is linux|darwin
#
# current macOS github builder only supports avx
# cpu is Xeon E5-1650 v2 (from 2013)

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

ls -l j64

j64/jconsole -lib libj.$ext testga.ijs
if [ $1 != "raspberry" ]; then
j64/jconsole -lib libjavx.$ext testga.ijs
fi

if [ $1 == "linux" ]; then
  j64/jconsole -lib libjavx2.$ext testga.ijs
  j32/jconsole -lib libj.$ext testga.ijs
fi

if [ $1 == "linux" ] && [ -d "j64gcc" ]; then
  j64gcc/jconsole -lib libj.$ext testga.ijs
  j64gcc/jconsole -lib libjavx.$ext testga.ijs
  j64gcc/jconsole -lib libjavx2.$ext testga.ijs
fi

if [ $1 == "linux" ] && [ -d "j32gcc" ]; then
  j32gcc/jconsole -lib libj.$ext testga.ijs
fi
