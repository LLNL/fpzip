// The static for loops below enable unrolling or even elimination
// of otherwise data dependent loops for basic integer types.  Here
// 'for () if (c)' replaces 'while (c)', and we (safely) assume that
// n does not exceed the number of bits in a UINT.

template <typename UINT>
inline void RCencoder::encode(UINT s, uint n)
{
  for (uint i = 1; i < (uint)sizeof(s) / 2; i++)
    if (n > 16) {
      encode_shift(s & 0xffff, 16);
      s >>= 16;
      n -= 16;
    }
  encode_shift(static_cast<uint>(s), n);
}

template <typename UINT>
inline void RCencoder::encode(UINT s, UINT l, UINT h)
{
  s -= l;
  h -= l;
  for (uint i = 1; i < (uint)sizeof(s) / 2; i++)
    if ((h >> 16)) {
      encode_shift(s & 0xffff, 16);
      s >>= 16;
      h >>= 16;
      h++;
    }
  encode_ratio(s, h);
}

// output n bytes
inline void RCencoder::put(uint n)
{
  for (uint i = 0; i < n; i++) {
    putbyte(low >> 24);
    low <<= 8;
  }
}
