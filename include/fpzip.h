/*
** Copyright (c) 2018-2019, Lawrence Livermore National Security, LLC.
** Produced at the Lawrence Livermore National Laboratory.
** Author: Peter Lindstrom.
** LLNL-CODE-764017.
** All rights reserved.
**
** This file is part of the fpzip library.
** For details, see http://computing.llnl.gov/casc/fpzip/.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
** 
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and/or other materials provided with the distribution.
** 
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
**
**
**                               SUMMARY
**
** fpzip is a library for lossless or lossy compression of 2D and 3D arrays
** of single- or double-precision floating-point scalars.  Although linear
** (1D) streams of scalars may be compressed as well, the library has
** primarily been designed to exploit higher-dimensional structure in the
** data.  fpzip may not perform well on 1D data or on data not stored as a
** spatially correlated contiguous array, with 'smoothly' varying values.
**
** The library supports compression to either a file or to main memory and
** allows specifying how many bits of precision to retain by truncating
** each floating-point value and discarding the least significant bits; the
** remaining bits are compressed losslessly.  The precision is limited to
** integers 2-32 for floats.  For doubles, precisions 4-64 are supported in
** increments of two bits.  The decompressed data is returned in full
** precision with any truncated bits zeroed.
**
** Because floating-point arithmetic may be affected by factors such as
** register precision, rounding mode, and compiler optimizations,
** precautions have been taken to ensure correctness and portability via a
** set of compile-time macros.  For example, it is possible to specify that
** floating-point operations be emulated via integer arithmetic, or to
** treat the binary representation of floating-point numbers as integers.
** Please consult the Config file for choosing among these settings.  The
** compressor works correctly on the IEEE 754 floating-point format, though
** no particular assumption is made on the floating-point representation
** other than the most significant bit being the sign bit.  Special values
** such as infinities, NaNs, and subnormal numbers should be handled
** correctly by the compressor in lossless mode.
**
** For convenience, functions are provided for reading and writing a header
** that stores version specific information and metadata such as array
** dimensions.  Such metadata must be provided to both compressor and
** decompressor.  It is up to the caller to decide whether or not to use
** such fpzip headers, as this information may already be available
** externally from container formats like HDF5.  If fpzip headers are not
** used, then the FPZ fields must be set by the caller in both read and
** write mode.
**
** A single compressed stream may store multiple contiguous fields (e.g.,
** for multiple arrays with the same dimensions that represent different
** variables).  Similarly, a stream may store multiple arrays of different
** dimensions and types, possibly with one header per array.  It is up to
** the caller to interleave read/write calls that perform (de)compression
** of floating-point data with read/write calls of header data.
**
** The return value of each function should be checked in case invalid
** arguments are passed or a run-time error occurs.  In this case, the
** variable fpzip_errno is set and can be examined to determine the cause
** of the error.
**
** fpzip is distributed as Open Source under a BSD-3 license.  The core
** library is written in C++ and applications need to be linked with a C++
** linker.  The library can, however, be called from C.  For further
** information, please e-mail fpzip@llnl.gov.
*/

#ifndef FPZIP_H
#define FPZIP_H

/* extern_ macro for exporting and importing symbols */
#if defined(_MSC_VER) && defined(FPZIP_SHARED_LIBS)
  /* export (import) symbols when FPZIP_SOURCE is (is not) defined */
  #ifdef FPZIP_SOURCE
    /* export symbols */
    #ifdef __cplusplus
      #define extern_ extern "C" __declspec(dllexport)
    #else
      #define extern_ extern     __declspec(dllexport)
    #endif
  #else
    /* import symbols */
    #ifdef __cplusplus
      #define extern_ extern "C" __declspec(dllimport)
    #else
      #define extern_ extern     __declspec(dllimport)
    #endif
  #endif
#else /* !(_MSC_VER && FPZIP_SHARED_LIBS) */
  #ifdef __cplusplus
    #define extern_ extern "C"
  #else
    #define extern_ extern
  #endif
#endif

/* stringification */
#define _fpzip_str_(x) # x
#define _fpzip_str(x) _fpzip_str_(x)

/* library version information */
#define FPZIP_VERSION_MAJOR 1 /* library major version number */
#define FPZIP_VERSION_MINOR 3 /* library minor version number */
#define FPZIP_VERSION_PATCH 0 /* library patch version number */

/* library version number (see also fpzip_codec_version) */
#define FPZIP_VERSION \
  ((FPZIP_VERSION_MAJOR << 8) + \
   (FPZIP_VERSION_MINOR << 4) + \
   (FPZIP_VERSION_PATCH << 0))

/* library version string (see also fpzip_version_string) */
#define FPZIP_VERSION_STRING \
  _fpzip_str(FPZIP_VERSION_MAJOR) "." \
  _fpzip_str(FPZIP_VERSION_MINOR) "." \
  _fpzip_str(FPZIP_VERSION_PATCH)

/* floating-point implementation */
#define FPZIP_FP_FAST 1
#define FPZIP_FP_SAFE 2
#define FPZIP_FP_EMUL 3
#define FPZIP_FP_INT  4

/* codec version number (see also fpzip_codec_version) */
#define FPZIP_CODEC ((0x0110u << 8) + (FPZIP_FP))

#define FPZIP_TYPE_FLOAT  0 /* single-precision data (see FPZ.type) */
#define FPZIP_TYPE_DOUBLE 1 /* double-precision data */

#ifdef __cplusplus
#include <cstdio>
extern "C" {
#else
#include <stdio.h>
#endif

/* array meta data and stream handle */
typedef struct {
  int type; /* single (0) or double (1) precision */
  int prec; /* number of bits of precision (zero = full) */
  int nx;   /* number of x samples */
  int ny;   /* number of y samples */
  int nz;   /* number of z samples */
  int nf;   /* number of fields */
} FPZ;

/* public data */
extern_ const unsigned int fpzip_codec_version;   /* codec version FPZIP_CODEC */
extern_ const unsigned int fpzip_library_version; /* library version FPZIP_VERSION */
extern_ const char* const fpzip_version_string;   /* verbose version string */
extern_ const unsigned int fpzip_data_model;      /* encoding of data model */

/* associate file with compressed input stream */
FPZ*                  /* compressed stream */
fpzip_read_from_file(
  FILE* file          /* binary input stream */
);

/* associate memory buffer with compressed input stream */
FPZ*                  /* compressed stream */
fpzip_read_from_buffer(
  const void* buffer  /* pointer to compressed input data */
);

/* read FPZ meta data (use only if previously written) */
int                   /* nonzero upon success */
fpzip_read_header(
  FPZ* fpz            /* compressed stream */
);

/* decompress array */
size_t                /* number of compressed bytes read (zero = error) */
fpzip_read(
  FPZ*  fpz,          /* compressed stream */
  void* data          /* uncompressed floating-point data */
);

/* close input stream and deallocate fpz */
void
fpzip_read_close(
  FPZ* fpz            /* compressed stream */
);

/* associate file with compressed output stream */
FPZ*                  /* compressed stream */
fpzip_write_to_file(
  FILE* file          /* binary output stream */
);

/* associate memory buffer with compressed output stream */
FPZ*                  /* compressed stream */
fpzip_write_to_buffer(
  void*  buffer,      /* pointer to compressed output data */
  size_t size         /* size of allocated storage for buffer */
);

/* write FPZ meta data */
int                   /* nonzero upon success */
fpzip_write_header(
  FPZ* fpz            /* compressed stream */
);

/* compress array */
size_t                /* number of compressed bytes written (zero = error) */
fpzip_write(
  FPZ*        fpz,    /* compressed stream */
  const void* data    /* uncompressed floating-point data */
);

/* close output stream and deallocate fpz */
void
fpzip_write_close(
  FPZ* fpz            /* compressed stream */
);

/*
** Error codes.
*/

typedef enum {
  fpzipSuccess             = 0, /* no error */
  fpzipErrorReadStream     = 1, /* cannot read stream */
  fpzipErrorWriteStream    = 2, /* cannot write stream */
  fpzipErrorBadFormat      = 3, /* magic mismatch; not an fpz stream */
  fpzipErrorBadVersion     = 4, /* fpz format version not supported */
  fpzipErrorBadPrecision   = 5, /* precision not supported */
  fpzipErrorBufferOverflow = 6, /* compressed buffer overflow */
  fpzipErrorInternal       = 7  /* exception thrown */
} fpzipError;

extern_ fpzipError fpzip_errno; /* error code */
extern_ const char* const fpzip_errstr[]; /* error message indexed by fpzip_errno */

#ifdef __cplusplus
}
#endif

#endif
