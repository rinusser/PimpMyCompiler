LLVMFLAGS := $(shell llvm-config-4.0 --cxxflags --ldflags --libs core)

run: llvm/generated.bc
	llvm-link-4.0 llvm/generated.bc -o - | LD_PRELOAD=$(PWD)/lib/libpmcc.so lli-4.0

llvm/generated.bc: bin/compiler
	bin/compiler 2>&1 | llvm-as-4.0 -o llvm/generated.bc

bin/compiler: src/compiler.cpp libraries
	g++ -g src/compiler.cpp $(LLVMFLAGS) -o bin/compiler

libraries: lib/libpmcc.so

lib/libpmcc.so: src/libpmcc.c
	gcc -c -Wall -Werror -fpic -o lib/libpmcc.o src/libpmcc.c
	gcc -shared -o lib/libpmcc.so lib/libpmcc.o

clean:
	rm -f bin/* lib/* llvm/*
