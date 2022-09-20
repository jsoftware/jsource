#!/bin/sh
sudo dpkg --add-architecture i386
sudo apt-get install -y build-essential gcc-multilib 
sudo apt-get install -y libedit-dev libncursesw5-dev
