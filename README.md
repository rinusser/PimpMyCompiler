# Synopsis

A proof of concept of an LLVM-based compiler capable of producing LLVM-based compilers.

The code is hosted on [GitHub](https://github.com/rinusser/PimpMyCompiler).


# General

### Short Version

I'm trying to figure out a multistage bootstrap process for a (mostly) self-compiling compiler. This project helps me
figure out the critical parts.

### Details

I imagined a build process for LLVM-based compilers handling an arbitrary syntax (e.g. a new programming language) that
can use a minimal subset of the same syntax (or any arbitrary small syntax, really) to build themselves in multiple
stages, with each stage adding new features to the resulting binary. The initial seed (the bootstrap code) needs to be
in a language with an available compiler and ideally decent LLVM bindings.

This repository contains a (still evolving) proof of concept for such a build setup: The bootstrap code ("stage 1") is
written in C and uses LLVM to build LLVM IR code for stage 2. Stage 2 compiles into a code generator that produces a
stage 3 LLVM module that, at the time of writing this, pretty much just invokes a shared library function that outputs
a greeting to stdout:

1. stage 1 (C) produces stage 2 LLVM IR
2. stage 2 (LLVM) produces stage 3 LLVM IR
3. stage 3 (LLVM) prints to stdout

Stage 2 will get support for reading some kind of source code and acting upon it. The source code fed into stage 2
should produce a stage 3 that can do everything stage 2 can and more, i.e. stage 3 should be able to parse the same
source file to reproduce itself and additionally support more source grammar.

### Scope

I won't implement a fully fledged language compiler here. This repository is just to show that such compilers _can_ be
built without too much difficulty (as far as writing compilers goes, anyway).

What I'm trying to get out of this project is:

1. a working self-compiling LLVM-based compiler
2. insights into what difficulties need to be addressed when writing a general-purpose LLVM-based compiler.

### Results

So far:

* generating an LLVM code-generator with LLVM works!
* the LLVM C++ API is better, but difficult to use outside C++. Some of the difficulties aren't even apparent in the API
  documentation, e.g. there are (easy to use) static factory methods that the C++ compiler optimized away in favor of
  (difficult to reproduce) instantiating objects directly.
* the LLVM C API seems to work just as well so far, is easier to use and results in easier to read stage 1 code (compare
  src/stage1.c with the old src/compiler.cpp if you want, you'll see)
* the LLVM C API documentation is lacking in a few places, maybe reading the C++ API equivalents will help.
  It's often easier to `grep` the header files for keywords than to browse the documentation.


# Requirements

* LLVM (tested with 4.0)
* gcc (tested with 6.3.0)
* GNU Make


# Usage

There's a Makefile that chains all the stages together. If you just run

    make

you'll get the ultimate (if currently not very exciting) stage's output. At the time of writing this it'll show:

    greetings from C! c_noargs() says hi.

This indicates that the stage 1 C code produced the stage 2 LLVM IR, which in turn produced the stage 3 LLVM IR that
compiled and linked into the above library function call.

There are 2 other particularly interesting `make` targets: `ir` and `ir2`. `make ir` will run stage 1 and output
stage 2's LLVM IR. Likewise, `make ir2`, will run stage 2 and output stage 3's LLVM IR.


# Legal

### Copyright

Copyright (C) 2018 Richard Nusser

### License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
