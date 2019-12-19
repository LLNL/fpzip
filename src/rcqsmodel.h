#ifndef RC_QSMODEL_H
#define RC_QSMODEL_H

#include "types.h"
#include "rcmodel.h"

class RCqsmodel : public RCmodel {
public:
  // initialization of model
  // compress: true for compression, false for decompression
  // symbols:  number of symbols
  // bits:     log2 of total frequency count (must be <= 16)
  // period:   max symbols between normalizations (must be < 1<<(bits+1))
  RCqsmodel(bool compress, uint symbols, uint bits = 16, uint period = 0x400);
  ~RCqsmodel();

  // reinitialize model
  void reset();

  // get frequencies for a symbol s
  void encode(uint s, uint& l, uint& r);

  // return symbol corresponding to cumulative frequency l
  uint decode(uint& l, uint& r);

  // normalize range
  void normalize(uint &r);

private:

  void update();
  void update(uint s);

  const uint bits;     // number of bits of precision for frequencies

  uint  left;          // number of symbols until next normalization
  uint  more;          // number of symbols with larger increment
  uint  incr;          // increment per update
  uint  rescale;       // current interval between normalizations
  uint  targetrescale; // target interval between rescales
  uint* symf;          // array of partially updated frequencies
  uint* cumf;          // array of cumulative frequencies

  uint  searchshift;   // difference of frequency bits and table bits
  uint* search;        // structure for searching on decompression
};

#include "rcqsmodel.inl"

#endif
