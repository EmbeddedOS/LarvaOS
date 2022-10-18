#!/bin/bash

export PREFIX="$(pwd)/bin/cross/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

make all
