#!/bin/bash
#
# test linux/macOS on github actions
#
# argument is linux|darwin
#
# current macOS github builder only supports avx
# cpu is Xeon E5-1650 v2 (from 2013)

if [ $1 == "linux" ]; then
  ext="so"
else
  ext="dylib"
fi

ls -l j64

j64/jconsole -lib libj.$ext testga.ijs
j64/jconsole -lib libjavx.$ext testga.ijs

if [ $1 == "linux" ]; then
  j64/jconsole -lib libjavx2.$ext testga.ijs
fi
