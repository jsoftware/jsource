#!/bin/bash
#
# test linux/macOS on github actions
#
# argument is linux|darwin

if [ $1 == "linux" ]; then
  ext="so"
else
  ext="dylib"
fi

ls -l j64
j64/jconsole -lib libj.$ext -jprofile ver.ijs
j64/jconsole -lib libjavx.$ext -jprofile ver.ijs

# this failed on macOS - perhaps running avx2 not yet supported?
#if [ $1 == "linux" ]; then
  j64/jconsole -lib libjavx2.$ext -jprofile ver.ijs
#fi

j64/jconsole -lib libjavx.$ext testga.ijs
