#!/bin/sh
cd "$(dirname "$0")"
nmake -f makefile.win CC=clang-cl x64=1 JAVX2=1 USE_OPENMP=0 USE_PYXES=1
