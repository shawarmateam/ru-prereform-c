#! ROOT MAKEFILE
# vars
BUILD_D = bin
CODE_D = src
FILES = ${CODE_D}/main.c

# cmds
all: build

build: ./${CODE_D}/main.c
	mkdir -p bin/
	gcc -o ${BUILD_D}/main ${FILES}

run: build
	./${BUILD_D}/main

clean:
	rm -rf ./${BUILD_D}/*


# aliases
clr: clean
