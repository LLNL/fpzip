// get frequencies for a symbol s
inline void RCqsmodel::encode(uint s, uint& l, uint& r)
{
  l = cumf[s];
  r = cumf[s + 1] - l;
  update(s);
}

inline void RCqsmodel::normalize(uint& r)
{
  r >>= bits;
}
