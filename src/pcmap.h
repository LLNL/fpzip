#ifndef PC_MAP_H
#define PC_MAP_H

#include <climits>
#if !defined FPZIP_WITH_REINTERPRET_CAST && !defined FPZIP_WITH_UNION
#include <cstring>
#endif
#include "types.h"

#define bitsizeof(t) ((uint)(CHAR_BIT * sizeof(t)))

template <typename T, uint width = bitsizeof(T), typename U = void>
struct PCmap;

// specialized for integer-to-integer map
template <typename T, uint width>
struct PCmap<T, width, void> {
  typedef T Domain;
  typedef T Range;
  static const uint bits = width;                    // Range bits
  static const uint shift = bitsizeof(Range) - bits; // Domain\Range bits
  Range forward(Domain d) const { return d >> shift; }
  Domain inverse(Range r) const { return r << shift; }
  Domain identity(Domain d) const { return inverse(forward(d)); }
};

// specialized for float type
template <uint width>
struct PCmap<float, width, void> {
  typedef float  Domain;
  typedef uint32 Range;
  union Union {
    Union(Domain d) : d(d) {}
    Union(Range r) : r(r) {}
    Domain d;
    Range r;
  };
  static const uint bits = width;                    // Range bits
  static const uint shift = bitsizeof(Range) - bits; // Domain\Range bits
  Range fcast(Domain d) const;
  Domain icast(Range r) const;
  Range forward(Domain d) const;
  Domain inverse(Range r) const;
  Domain identity(Domain d) const;
};

// specialized for double type
template <uint width>
struct PCmap<double, width, void> {
  typedef double Domain;
  typedef uint64 Range;
  union Union {
    Union(Domain d) : d(d) {}
    Union(Range r) : r(r) {}
    Domain d;
    Range r;
  };
  static const uint bits = width;                    // Range bits
  static const uint shift = bitsizeof(Range) - bits; // Domain\Range bits
  Range fcast(Domain d) const;
  Domain icast(Range r) const;
  Range forward(Domain d) const;
  Domain inverse(Range r) const;
  Domain identity(Domain d) const;
};

#include "pcmap.inl"

#endif
