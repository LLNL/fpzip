template <typename UINT>
inline UINT RCdecoder::decode(uint n)
{
  UINT s = 0;
  uint m = 0;
  for (uint i = 1; i < (uint)sizeof(s) / 2; i++)
    if (n > 16) {
      s += UINT(decode_shift(16)) << m;
      m += 16;
      n -= 16;
    }
  return (UINT(decode_shift(n)) << m) + s;
}

template <typename UINT>
inline UINT RCdecoder::decode(UINT l, UINT h)
{
  UINT s = l;
  uint m = 0;
  h -= l;
  for (uint i = 1; i < (uint)sizeof(s) / 2; i++)
    if ((h >> 16)) {
      s += UINT(decode_shift(16)) << m;
      m += 16;
      h >>= 16;
      h++;
    }
  return (UINT(decode_ratio(h)) << m) + s;
}

// input n bytes
inline void RCdecoder::get(uint n)
{
  for (uint i = 0; i < n; i++) {
    code <<= 8;
    code |= getbyte();
    low <<= 8;
  }
}
