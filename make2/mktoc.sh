#!/bin/sh
set -e

cd "$(dirname "$0")"
echo "generate jsrc/toc.txt"

cd ../jsrc
rm -f toc.txt
for f in *.c; do 
echo "$f\c" >>toc.txt
echo "\t\c" >>toc.txt
sed -n 4p $f >>toc.txt
done
