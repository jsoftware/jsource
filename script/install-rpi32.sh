#!/bin/sh
sudo pkg --add-architecture armhf
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y build-essential libc6-dev-armhf-cross autoconf automake libtool gdb
sudo apt-get install -y nasm
sudo apt-get install -y libc6:armhf libstdc++6:armhf libc6-dev:armhf
echo "fr_FR.UTF-8 UTF-8" | sudo tee -a /etc/locale.gen
sudo locale-gen
clang --version
gcc --version
