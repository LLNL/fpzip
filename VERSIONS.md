# fpzip Release Notes


## 1.3.0 (December 20, 2019)

- Changed license to BSD.

- Added CMake support.

- Added regression tests.

- Added library version string and identifiers.

- Added user-defined 64-bit integer types for code portability.

- Renamed include directory and reorganized source file structure.


## 1.2.0 (June 10, 2017)

- Added fpzip command-line utility for (de)compressing files.

- Replaced C99 comments in fpzip.h with C89 comments.


## 1.1.0 (June 8, 2014)

- Modified API to pass arguments via a parameter object (FPZ).

- Modified API to support reading the header before decompressing the
  floating-point data, thus allowing the floating-point array to be
  allocated based on the dimensions stored in the header.

- Modified API to allow header to be included/excluded at run time
  rather than at compile time.

- Modified testfpzip to allow the floating-point type to be specified
  on the command line.

- Deprecated Fortran interface.


## 1.0.2 (May 30, 2014; internal release only)

- Added support for specifying the precision in increments of one
  (floats) or two (doubles) bits for lossy compression.

- Added support for data sets larger than 4 GB.

- Added compile-time support for excluding header information, e.g.
  for in-memory compression of many small arrays.

- Modified API to use size_t in place of unsigned int where appropriate.

- Changed error handling to set fpzip_errno instead of writing to
  stderr and aborting.

- Added more functionality to the testfpzip example program.


## 1.0.1 (August 7, 2008)

- Added modes for bitwise type conversions beyond reinterpret_cast,
  which caused aliasing problems with gcc 4.1.2.


## 1.0.0 (March 20, 2008)

- Initial release.
