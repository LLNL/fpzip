// specialization for small alphabets -----------------------------------------

template <typename T, class M>
class PCencoder<T, M, false> {
public:
  PCencoder(RCencoder* re, RCmodel*const* rm) : re(re), rm(rm) {}
  T encode(T real, T pred, uint context = 0);
  static const uint symbols = 2 * (1 << M::bits) - 1;
private:
  static const uint bias = (1 << M::bits) - 1; // perfect prediction symbol
  M                 map;                       // maps T to integer type
  RCencoder*const   re;                        // entropy encoder
  RCmodel*const*    rm;                        // probability modeler(s)
};

// encode narrow range type
template <typename T, class M>
T PCencoder<T, M, false>::encode(T real, T pred, uint context)
{
  // map type T to unsigned integer type
  typedef typename M::Range U;
  U r = map.forward(real);
  U p = map.forward(pred);
  // entropy encode d = r - p
  re->encode(static_cast<uint>(bias + r - p), rm[context]);
  // return decoded value
  return map.inverse(r);
}

// specialization for large alphabets -----------------------------------------

template <typename T, class M>
class PCencoder<T, M, true> {
public:
  PCencoder(RCencoder* re, RCmodel*const* rm) : re(re), rm(rm) {}
  T encode(T real, T pred, uint context = 0);
  static const uint symbols = 2 * M::bits + 1;
private:
  static const uint bias = M::bits; // perfect prediction symbol
  M                 map;            // maps T to integer type
  RCencoder*const   re;             // entropy encoder
  RCmodel*const*    rm;             // probability modeler(s)
};

// encode wide range type
template <typename T, class M>
T PCencoder<T, M, true>::encode(T real, T pred, uint context)
{
  // map type T to unsigned integer type
  typedef typename M::Range U;
  U r = map.forward(real);
  U p = map.forward(pred);
  // compute (-1)^s (2^k + m) = r - p, entropy code (s, k),
  // and encode the k-bit number m verbatim
  if (p < r) {      // underprediction
    U d = r - p;
    uint k = PC::bsr(d);
    re->encode(bias + 1 + k, rm[context]);
    re->encode(d - (U(1) << k), k);
  }
  else if (p > r) { // overprediction
    U d = p - r;
    uint k = PC::bsr(d);
    re->encode(bias - 1 - k, rm[context]);
    re->encode(d - (U(1) << k), k);
  }
  else              // perfect prediction
    re->encode(bias, rm[context]);
  // return decoded value
  return map.inverse(r);
}
