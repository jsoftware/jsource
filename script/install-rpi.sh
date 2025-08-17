#!/bin/sh
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y build-essential libc6-dev autoconf automake libtool gdb
sudo apt-get install -y nasm libomp5-14 libopenblas0-pthread
echo "fr_FR.UTF-8 UTF-8" | sudo tee -a /etc/locale.gen
sudo locale-gen
clang --version
gcc --version
