OS_DIR=$(shell cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=$(OS_DIR)/build

export OS_DIR
export BUILD_DIR


help:
	@echo "Makefile for Building Larva Operating System."
	@echo "Usage: make [ all | clean | help | build | run] " 
	@echo ""

all:
	make -C ./src/kernel/lib all
	make -C ./src/kernel all

clean:
	make -C ./src/kernel/lib clean
	make -C ./src/kernel clean
