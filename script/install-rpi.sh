#!/bin/sh
set -e
cd
apt-get update -y --allow-releaseinfo-change
apt-get install -y build-essential clang wget
apt-get install -y nasm
git clone https://github.com/jsoftware/jsource.git
cd jsource
script/buildga.sh raspberry
script/testga.sh raspberry
