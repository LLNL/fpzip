template <uint width>
uint32
PCmap<float, width, void>::fcast(float d) const
{
#ifdef FPZIP_WITH_REINTERPRET_CAST
  return reinterpret_cast<const Range&>(d);
#elif defined FPZIP_WITH_UNION
  Union shared(d);
  return shared.r;
#else
  Range r;
  memcpy(&r, &d, sizeof(r));
  return r;
#endif
}

template <uint width>
float
PCmap<float, width, void>::icast(uint32 r) const
{
#ifdef FPZIP_WITH_REINTERPRET_CAST
  return reinterpret_cast<const Domain&>(r);
#elif defined FPZIP_WITH_UNION
  Union shared(r);
  return shared.d;
#else
  Domain d;
  memcpy(&d, &r, sizeof(d));
  return d;
#endif
}

template <uint width>
uint32
PCmap<float, width, void>::forward(float d) const
{
  Range r = fcast(d);
  r = ~r;
  r >>= shift;
  r ^= -(r >> (bits - 1)) >> (shift + 1);
  return r;
}

template <uint width>
float
PCmap<float, width, void>::inverse(uint32 r) const
{
  r ^= -(r >> (bits - 1)) >> (shift + 1);
  r = ~r;
  r <<= shift;
  return icast(r);
}

template <uint width>
float
PCmap<float, width, void>::identity(float d) const
{
  Range r = fcast(d);
  r >>= shift;
  r <<= shift;
  return icast(r);
}

template <uint width>
uint64
PCmap<double, width, void>::fcast(double d) const
{
#ifdef FPZIP_WITH_REINTERPRET_CAST
  return reinterpret_cast<const Range&>(d);
#elif defined FPZIP_WITH_UNION
  Union shared(d);
  return shared.r;
#else
  Range r;
  memcpy(&r, &d, sizeof(r));
  return r;
#endif
}

template <uint width>
double
PCmap<double, width, void>::icast(uint64 r) const
{
#ifdef FPZIP_WITH_REINTERPRET_CAST
  return reinterpret_cast<const Domain&>(r);
#elif defined FPZIP_WITH_UNION
  Union shared(r);
  return shared.d;
#else
  Domain d;
  memcpy(&d, &r, sizeof(d));
  return d;
#endif
}

template <uint width>
uint64
PCmap<double, width, void>::forward(double d) const
{
  Range r = fcast(d);
  r = ~r;
  r >>= shift;
  r ^= -(r >> (bits - 1)) >> (shift + 1);
  return r;
}

template <uint width>
double
PCmap<double, width, void>::inverse(uint64 r) const
{
  r ^= -(r >> (bits - 1)) >> (shift + 1);
  r = ~r;
  r <<= shift;
  return icast(r);
}

template <uint width>
double
PCmap<double, width, void>::identity(double d) const
{
  Range r = fcast(d);
  r >>= shift;
  r <<= shift;
  return icast(r);
}
