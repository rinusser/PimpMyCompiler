LLVMCFLAGS := $(shell llvm-config-4.0 --cflags)
LLVMFLAGS := $(shell llvm-config-4.0 --cxxflags --ldflags --libs core)

$(shell mkdir -p bin lib llvm)

all: run


run: llvm/stage2.bc
	llvm-link-4.0 llvm/stage2.bc -o - | lli-4.0

llvm/stage2.bc: bin/stage1
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage1 2>&1 | llvm-as-4.0 -o llvm/stage2.bc

ir: bin/stage1
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage1

bin/stage1: src/stage1.c
	gcc -c -g $(LLVMCFLAGS) -o bin/stage1.o src/stage1.c
	g++ -g $(LLVMFLAGS) -o bin/stage1 bin/stage1.o

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
