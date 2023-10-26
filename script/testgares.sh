#!/bin/sh
#
# read any test results into tests.txt on github actions

T=tests.txt
> $T

f() { if [ -e $1 ]; then cat $1 >> $T; fi }

f "testrpi32.txt"
f "testrpi64.txt"
f "testlinux.txt"
f "testdarwin.txt"
f "testwin.txt"
f "testwin32.txt"
f "testopenbsd.txt"
f "testfreebsd.txt"
