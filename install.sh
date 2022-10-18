#!/bin/bash

apt-get update
apt-get upgrade -y
apt-get install build-essential -y
apt-get install make -y
apt-get install gcc -y          # Not cross.
apt-get install g++ -y          # Not cross.
apt-get install flex -y
apt-get install bison -y
apt-get install libgmp3-dev -y
apt-get install libmpc-dev -y
apt-get install texinfo -y      # Optional.
apt-get install libisl-dev -y
sudo apt-get install libmpfr-dev libmpfr-doc libmpfr4 libmpfr4-dbg -y
sudo apt-get install build-essential -y
sudo apt-get install glibc-devel -y
sudo apt-get install gcc-multilib libc6-i386 -y