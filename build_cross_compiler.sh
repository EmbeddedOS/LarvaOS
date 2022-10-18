#!/bin/bash


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export PREFIX="$SCRIPT_DIR/bin/cross/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

function build_binutils 
{
    cd $SCRIPT_DIR/build
    rm -rf build-gcc
    mkdir build-binutils
    cd build-binutils
    ../../cross_compiler/binutils/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make -j4
    make -j4 install
}

function build_gcc
{
    cd $SCRIPT_DIR/build
    # The $PREFIX/bin dir _must_ be in the PATH. We did that above.
    which -- $TARGET-as || echo $TARGET-as is not in the PATH
    rm -rf build-gcc
    mkdir build-gcc
    cd build-gcc
    ../../cross_compiler/gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
    make -j4 all-gcc
    make -j4 all-target-libgcc
    make -j4 install-gcc
    make -j4 install-target-libgcc
}

build_binutils
build_gcc



