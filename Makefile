LLVMFLAGS := $(shell llvm-config-4.0 --cxxflags --ldflags --libs core)

$(shell mkdir -p bin lib llvm)

all: run


run: llvm/generated.bc
	llvm-link-4.0 llvm/generated.bc -o - | LD_PRELOAD="$(PWD)/lib/libpmcc.so $(PWD)/lib/libpmccpp.so $(PWD)/lib/libllvmwrapper.so" lli-4.0

llvm/generated.bc: bin/compiler
	LD_LIBRARY_PATH=$(PWD)/lib bin/compiler 2>&1 | llvm-as-4.0 -o llvm/generated.bc

assembly: bin/compiler
	LD_LIBRARY_PATH=$(PWD)/lib bin/compiler

bin/compiler: src/compiler.cpp libraries
	g++ -g src/compiler.cpp $(LLVMFLAGS) -lllvmwrapper -L$(PWD)/lib/ -o bin/compiler

libraries: lib/libpmcc.so lib/libpmccpp.so lib/libllvmwrapper.so

lib/libpmcc.so: src/libpmcc.c
	gcc -c -Wall -Werror -fpic -o lib/libpmcc.o src/libpmcc.c
	gcc -shared -o lib/libpmcc.so lib/libpmcc.o

lib/libpmccpp.so: src/libpmccpp.cpp
	g++ -c -Wall -Werror -fpic -o lib/libpmccpp.o src/libpmccpp.cpp
	g++ -shared -o lib/libpmccpp.so lib/libpmccpp.o

lib/libllvmwrapper.so: src/libllvmwrapper.cpp
	g++ $(LLVMFLAGS) -shared -fpic -o lib/libllvmwrapper.so src/libllvmwrapper.cpp

clean:
	rm -f bin/* lib/* llvm/*
