#!/bin/sh
sudo dpkg --add-architecture i386
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y build-essential gcc-multilib g++-multilib libc6-dev libc6-dev-i386-cross g++-mingw-w64-x86-64 g++-mingw-w64-i686 autoconf automake libtool
sudo apt-get install -y nasm libopenblas0-pthread:i386 libopenblas0-pthread libomp-dev
echo "fr_FR.UTF-8 UTF-8" | sudo tee -a /etc/locale.gen
sudo locale-gen
clang --version
gcc --version
