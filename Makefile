.PHONY: all build test clean

all: configure build

configure:
	cmake -B build -S src

build:
	cmake --build build

lint:
	find ./src -type f -name '*.c' -exec clang-format -i {} \;
	find ./src -type f -name '*.h' -exec clang-format -i {} \;

clean:
	rm -rf build/
