#!/bin/bash
# list globals for J64 libj.so

cd ~/jbld/j64/bin
# unitialized
objdump -t libj.so | grep ' \.bss'    >  globals.txt
# initialized
objdump -t libj.so | grep ' \.data'   >> globals.txt
# const
objdump -t libj.so | grep ' \.rodata' >> globals.txt

