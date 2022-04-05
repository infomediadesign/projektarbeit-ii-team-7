all: build

build:
	cmake -B build -S src
	cmake --build build

clean:
	rm build/
