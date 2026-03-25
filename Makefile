CLANG_FORMAT ?= clang-format

SRC := $(shell find src -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \))

.PHONY: format build run

format:
	$(CLANG_FORMAT) -i $(SRC)

build:
	cmake --preset=vcpkg
	cmake -S . -B build
	cmake --build build
	ln -sf build/compile_commands.json compile_commands.json

run:
	./build/neo-edo
