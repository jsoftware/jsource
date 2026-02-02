#!/bin/sh
sudo dpkg --add-architecture armhf
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y build-essential gcc-multilib libc6-dev autoconf automake libtool gdb gdb-multiarch
sudo apt-get install -y nasm libomp5-14 libopenblas0-pthread
sudo apt-get install -y linux-libc-dev-armhf-cross libc6-dev-armhf-cross libc6-armhf-cross gcc-arm-linux-gnueabihf
sudo apt-get install -y libc6:armhf libstdc++6:armhf libc6-dev:armhf
echo "fr_FR.UTF-8 UTF-8" | sudo tee -a /etc/locale.gen
sudo locale-gen
clang --version
gcc --version
arm-linux-gnueabihf-gcc --version
