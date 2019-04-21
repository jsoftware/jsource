#!/bin/bash
# rm all *.o for clean builds - makefile dependencies are not set 

cd "$(dirname "$(readlink -f "$0" || realpath "$0")")"

find obj -name "*.o" -type f -delete
