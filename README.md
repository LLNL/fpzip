fpzip
=====

INTRODUCTION
------------

fpzip is a library and command-line utility for lossless and optionally lossy
compression of 2D and 3D floating-point arrays.  fpzip assumes spatially
correlated scalar-valued data, such as regularly sampled continuous functions,
and is not suitable for compressing unstructured streams of floating-point
numbers.  In lossy mode, fpzip discards some number of least significant
mantissa bits and losslessly compresses the result.  fpzip currently supports
IEEE-754 single (32-bit) and double (64-bit) precision floating-point data.
fpzip is written in C++ but has a C compatible API that can be called from
C and other languages.  It conforms to the C++98 and C89 language standards.

fpzip is released as Open Source under a three-clause BSD license.  Please
see the file LICENSE for further details.


INSTALLATION
------------

### CMake builds

fpzip was developed for Linux and macOS but can be built on Windows using
[CMake](https://cmake.org).  To use CMake, type:

    cd fpzip
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release

fpzip can be configured using compile-time options, e.g.:

    cmake .. -DFPZIP_FP=FPZIP_FP_SAFE -DBUILD_UTILITIES=OFF

To display the available options, type:

    cmake .. -L

Basic regression testing is available:

    ctest -V -C Release

### GNU builds

fpzip may also be built using [GNU make](https://www.gnu.org/software/make/):

    cd fpzip
    gmake

This builds lib/libfpzip.a and bin/fpzip.

The GNU make options are listed in the file Config and should preferably be
set on the command line, e.g.:

    gmake FPZIP_FP=FPZIP_FP_SAFE BUILD_UTILITIES=0

To run the regression tests, type:

    gmake test


DOCUMENTATION
-------------

Documentation is currently limited to the source files themselves.  For
information on the API, please see the header file include/fpzip.h.  For an
example of how to call fpzip, please see the source file utils/fpzip.cpp.
This utility may be used to compress binary files of raw floating-point
numbers.  Usage is given by:

    fpzip -h


AUTHOR
------

fpzip was written by [Peter Lindstrom](https://people.llnl.gov/pl) at
Lawrence Livermore National Laboratory.


CITING fpzip
------------

If you use fpzip for scholarly research, please cite the following paper:

    Peter Lindstrom and Martin Isenburg
    "Fast and Efficient Compression of Floating-Point Data"
    IEEE Transactions on Visualization and Computer Graphics, 12(5):1245-1250, 2006


LICENSE
-------

fpzip is distributed under the terms of the BSD license.  See the files
[LICENSE](https://github.com/LLNL/fpzip/blob/master/LICENSE) and
[NOTICE](https://github.com/LLNL/fpzip/blob/master/NOTICE) for details.

`SPDX-License-Identifier: BSD`

``LLNL-CODE-764017``


QUESTIONS AND COMMENTS
----------------------

For questions and comments, please contact us at fpzip@llnl.gov.
Please submit bug reports and feature requests via the GitHub
[issue tracker](https://github.com/LLNL/fpzip/issues).
