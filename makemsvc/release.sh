#!/bin/sh

# copy windows binaries from makemsvc to jlibrary/bin

set -e

cd "$(dirname "$0")"

S=../bin/windows
A=../jlibrary/bin
B=../jlibrary/bin32

cop() {
if [ -f "$1" ]; then
  echo \# cp "$1" "$2"
  cp "$1" "$2" || true
fi
}

mkdir -p $A
mkdir -p $B

cop $S/j64avx512/jconsole.exe $A/.
cop $S/j64avx2/jconsole.exe $A/.
cop $S/j64/jconsole.exe $A/.
cop $S/j32/jconsole.exe $B/.

cop $S/j64avx512/j.dll $A/javx512.dll
cop $S/j64avx2/j.dll $A/javx2.dll
cop $S/j64/j.dll $A/.
cop $S/j64/j.dll $A/j-nonavx.dll
cop $S/j32/j.dll $B/.

cop $S/j64avx512/tsdll.dll $A/.
cop $S/j64avx2/tsdll.dll $A/.
cop $S/j64/tsdll.dll $A/.
cop $S/j32/tsdll.dll $B/.
