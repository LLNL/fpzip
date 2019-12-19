#ifndef RC_DECODER_H
#define RC_DECODER_H

#include "types.h"
#include "rcmodel.h"

class RCdecoder {
public:
  RCdecoder() : error(false), low(0), range(-1u), code(0) {}
  virtual ~RCdecoder() {}

  // initialize decoding
  void init();

  // decode a bit
  bool decode();

  // decode a number s : 0 <= s < 2^n
  template <typename UINT>
  UINT decode(uint n);

  // decode a number s : l <= s < h
  template <typename UINT>
  UINT decode(UINT l, UINT h);

  // decode a symbol using probability modeling
  uint decode(RCmodel* rm);

  // virtual function for reading byte stream
  virtual uint getbyte() = 0;

  // number of bytes read
  virtual size_t bytes() const = 0;

  bool error;

private:
  uint decode_shift(uint n);
  uint decode_ratio(uint n);
  void get(uint n);
  void normalize();

  uint low;   // low end of interval
  uint range; // length of interval
  uint code;  // incoming data
};

#include "rcdecoder.inl"

#endif
