#!/bin/sh
if [ "$(uname -m)" = "arm64" ] || [ "$(uname -m)" = "aarch64" ]; then
sudo dpkg --add-architecture armhf
else
sudo dpkg --add-architecture i386
fi
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y build-essential gcc clang libc6-dev autoconf automake libtool gdb gdb-multiarch lldb zip
sudo apt-get install -y nasm libomp5
# sudo apt-get install -y libopenblas0-pthread
if [ "$(uname -m)" = "arm64" ] || [ "$(uname -m)" = "aarch64" ]; then
sudo apt-get install -y linux-libc-dev-armhf-cross libc6-dev-armhf-cross libc6-armhf-cross gcc-arm-linux-gnueabihf || true
sudo apt-get install -y libc6:armhf libstdc++6:armhf libc6-dev:armhf
# sudo apt-get install -y libopenblas0-pthread:armhf
else
sudo apt-get install -y gcc-multilib g++-multilib libc6-dev-i386-cross g++-mingw-w64-x86-64 g++-mingw-w64-i686
fi
echo "fr_FR.UTF-8 UTF-8" | sudo tee -a /etc/locale.gen
sudo locale-gen
clang --version
gcc --version
arm-linux-gnueabihf-gcc --version || true
apt-cache pkgnames clang || true
apt-cache pkgnames gcc || true
