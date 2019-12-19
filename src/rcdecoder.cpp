#include <cstddef>
#include "rcdecoder.h"

// initialize decoder
void RCdecoder::init()
{
  error = false;
  get(4);
}

// decode a bit
bool RCdecoder::decode()
{
  range >>= 1;
  bool s = (code >= low + range);
  if (s)
    low += range;
  normalize();
  return s;
}

// decode a symbol using probability modeling
uint RCdecoder::decode(RCmodel* rm)
{
  rm->normalize(range);
  uint l = (code - low) / range;
  uint r;
  uint s = rm->decode(l, r);
  low += range * l;
  range *= r;
  normalize();
  return s;
}

// decode a number s : 0 <= s < 2^n <= 2^16
uint RCdecoder::decode_shift(uint n)
{
  range >>= n;
  uint s = (code - low) / range;
  low += range * s;
  normalize();
  return s;
}

// decode a number s : 0 <= s < n <= 2^16
uint RCdecoder::decode_ratio(uint n)
{
  range /= n;
  uint s = (code - low) / range;
  low += range * s;
  normalize();
  return s;
}

// normalize the range and input data
void RCdecoder::normalize()
{
  while (!((low ^ (low + range)) >> 24)) {
    // top 8 bits are fixed; output them
    get(1);
    range <<= 8;
  }
  if (!(range >> 16)) {
    // top 8 bits are not fixed but range is small;
    // fudge range to avoid carry and input 16 bits
    get(2);
    range = -low;
  }
}
