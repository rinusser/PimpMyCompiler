LLVMCFLAGS := $(shell llvm-config-4.0 --cflags)
LLVMFLAGS := $(shell llvm-config-4.0 --cxxflags --ldflags --libs core)

$(shell mkdir -p bin lib llvm)

all: run


run: llvm/stage3.bc lib/libpmcc.so
	llvm-link-4.0 llvm/stage3.bc -o - | LD_PRELOAD="$(PWD)/lib/libpmcc.so" lli-4.0

bin/stage3: llvm/stage3.bc lib/libpmcc.so
	llc-4.0 -relocation-model=pic -filetype=obj -o llvm/stage3.o llvm/stage3.bc
	g++ $(LLVMFLAGS) -L$(PWD)/lib -lpmcc -o bin/stage3 llvm/stage3.o

llvm/stage3.bc: bin/stage2
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage2 2>&1 >/dev/null | llvm-as-4.0 -o llvm/stage3.bc

ir2: bin/stage2
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage2

sourcedemo: bin/stage2
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage2 2>/dev/null

llvm/stage2.bc: bin/stage1
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage1 2>&1 | llvm-as-4.0 -o llvm/stage2.bc

ir: bin/stage1
	LD_LIBRARY_PATH=$(PWD)/lib bin/stage1

bin/stage1: src/stage1.c
	gcc -c -g $(LLVMCFLAGS) -o bin/stage1.o src/stage1.c
	g++ -g $(LLVMFLAGS) -o bin/stage1 bin/stage1.o

bin/stage2: llvm/stage2.bc lib/libpmcc.so lib/libsource.so
	llc-4.0 -relocation-model=pic -filetype=obj -o llvm/stage2.o llvm/stage2.bc
	g++ $(LLVMFLAGS) -o bin/stage2 -L$(PWD)/lib -lpmcc -lsource llvm/stage2.o

stage1cpp: src/compiler.cpp lib/libpmcc.so lib/libpmccpp.so lib/libllvmwrapper.so
	g++ -g src/compiler.cpp $(LLVMFLAGS) -lllvmwrapper -L$(PWD)/lib/ -o bin/stage1

lib/libpmcc.so: src/libpmcc.c
	gcc -c -Wall -Werror -fpic -o lib/libpmcc.o src/libpmcc.c
	gcc -shared -o lib/libpmcc.so lib/libpmcc.o

lib/libpmccpp.so: src/libpmccpp.cpp
	g++ -c -Wall -Werror -fpic -o lib/libpmccpp.o src/libpmccpp.cpp
	g++ -shared -o lib/libpmccpp.so lib/libpmccpp.o

lib/libllvmwrapper.so: src/libllvmwrapper.cpp
	g++ $(LLVMFLAGS) -shared -fpic -o lib/libllvmwrapper.so src/libllvmwrapper.cpp

lib/libsource.so: src/libsource.c
	gcc -Wall -Werror -fpic -shared -o lib/libsource.so src/libsource.c

clean:
	rm -f bin/* lib/* llvm/*
