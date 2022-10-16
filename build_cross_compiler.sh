#!/bin/bash

./cross_compiler/install_dependences.sh

CURRENT_DIR=$(pwd)
export PREFIX="$CURRENT_DIR/bin/cross/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir build
mkdir bin

# build  binutils.
cd build
mkdir build-binutils
cd build-binutils
../../cross_compiler/binutils/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install


cd $CURRENT_DIR

# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH

# build gcc.
cd build
mkdir build-gcc
cd build-gcc
../../cross_compiler/gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc


cd $CURRENT_DIR
