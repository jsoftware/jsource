#!/bin/bash
#
# read any test results into tests.txt

T=tests.txt
> $T

f() { if [ -e $1 ]; then cat $1 >> $T; fi }

# f "testrpi32.txt"
# f "testrpi64.txt"
f "testlinux.txt"
# f "testdarwin.txt"
# f "testwin.txt"
