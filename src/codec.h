#ifndef CODEC_H
#define CODEC_H

#include "fpzip.h"
#include "types.h"

#ifndef FPZIP_FP
  #error "floating-point mode FPZIP_FP not defined"
#elif FPZIP_FP < 1 || FPZIP_FP > 4
  #error "invalid floating-point mode FPZIP_FP"
#endif

#if FPZIP_FP == FPZIP_FP_INT
// identity map for integer arithmetic
template <typename T, unsigned width>
struct PCmap<T, width, T> {
  typedef T Domain;
  typedef T Range;
  static const uint bits = width;
  static const T    mask = ~T(0) >> (bitsizeof(T) - bits);
  Range forward(Domain d) const { return d & mask; }
  Domain inverse(Range r) const { return r & mask; }
  Domain identity(Domain d) const { return d & mask; }
};
#endif

#define FPZ_MAJ_VERSION 0x0110
#define FPZ_MIN_VERSION FPZIP_FP

#endif
