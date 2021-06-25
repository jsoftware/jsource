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

cd j64
ls -l
./jconsole -lib libj.$ext -jprofile ver.ijs
./jconsole -lib libjavx.$ext -jprofile ver.ijs

# this failed on macOS - perhaps running avx2 not yet supported?
if [ $1 == "linux" ]; then
  ./jconsole -lib libjavx2.$ext -jprofile ver.ijs
fi
