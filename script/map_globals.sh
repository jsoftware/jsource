#!/bin/bash
# list globals for J64 libj.so

cd ~/jbld/j64/bin
# unitialized
objdump -t libjavx2.so | grep ' \.bss'    >  globals.txt
# initialized
objdump -t libjavx2.so | grep ' \.data'   >> globals.txt
# const
objdump -t libjavx2.so | grep ' \.rodata' >> globals.txt

