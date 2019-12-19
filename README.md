fpzip
=====

## INTRODUCTION

fpzip is a library for lossless and lossy compression of 2D and 3D
floating-point scalar fields.  fpzip was written by Peter Lindstrom at
LLNL, and is based on the algorithm described in the following paper:

    Peter Lindstrom and Martin Isenburg
    "Fast and Efficient Compression of Floating-Point Data"
    IEEE Transactions on Visualization and Computer Graphics,
      12(5):1245-1250, September-October 2006.

fpzip is primarily written in C++ but has a C89 API that can be called from
C and other languages.

For information on the API, please see the header file include/fpzip.h.

fpzip is released as Open Source under a BSD license.  Please see the file
LICENSE for further details.


## INSTALLATION

The library has been written primarily for Linux, but has been successfully
tested with the following compilers and platforms:

    clang 3.4 (Linux)
    clang 4.2 (Mac OS X)
    gcc 4.2.1 (Mac OS X)
    gcc 4.4.7 (Linux)
    icc 12.1.5 (Linux)
    mingw32-gcc 4.8.1 (Windows)

To compile the library, type:

    cd src; make

There are example programs in the examples subdirectory that compress and
decompress multidimensional single- or double-precision floating-point
arrays.  The testfpzip example is meant to illustrate how to use the fpzip
API, and can also be used to verify correct functionality of the library.
If the program fails, please consult src/Makefile for compile-time macros
that may fix the problem.  The fpzip example is a command-line tool for
compressing and decompressing binary floating-point files stored in native
byte order.


## QUESTIONS AND COMMENTS

For bug reports and other questions, please contact us at fpzip@llnl.gov.


## LICENSE

fpzip is distributed under the terms of the BSD license.  See the files
[LICENSE](https://github.com/llnl/fpzip/blob/master/LICENSE) and
[NOTICE](https://github.com/llnl/fpzip/blob/master/NOTICE) for details.

`SPDX-License-Identifier: BSD`

``LLNL-CODE-764017``
