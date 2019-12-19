// specialization for small alphabets -----------------------------------------

template <typename T, class M>
class PCdecoder<T, M, false> {
public:
  PCdecoder(RCdecoder* rd, RCmodel*const* rm) : rd(rd), rm(rm) {}
  ~PCdecoder() {}
  T decode(T pred, uint context = 0);
  static const uint symbols = 2 * (1 << M::bits) - 1;
private:
  static const uint bias = (1 << M::bits) - 1;
  M                 map;            // maps T to some unsigned integer type
  RCdecoder*const   rd;             // entropy decoder
  RCmodel*const*    rm;             // probability modeler(s)
};

// decode narrow range type
template <typename T, class M>
T PCdecoder<T, M, false>::decode(T pred, uint context)
{
  // map type T to unsigned integer type
  typedef typename M::Range U;
  U p = map.forward(pred);
  // entropy decode d = r - p
  U r = p + rd->decode(rm[context]) - bias;
  return map.inverse(r);
}

// specialization for large alphabets -----------------------------------------

template <typename T, class M>
class PCdecoder<T, M, true> {
public:
  PCdecoder(RCdecoder* rd, RCmodel*const* rm) : rd(rd), rm(rm) {}
  ~PCdecoder() {}
  T decode(T pred, uint context = 0);
  static const uint symbols = 2 * M::bits + 1;
private:
  static const uint bias = M::bits;
  M                 map;            // maps T to some unsigned integer type
  RCdecoder*const   rd;             // entropy decoder
  RCmodel*const*    rm;             // probability modeler(s)
};

// decode wide range type
template <typename T, class M>
T PCdecoder<T, M, true>::decode(T pred, uint context)
{
  typedef typename M::Range U;
  uint s = rd->decode(rm[context]);
  if (s > bias) {      // underprediction
    uint k = s - bias - 1;
    U d = (U(1) << k) + rd->template decode<U>(k);
    U p = map.forward(pred);
    U r = p + d;
    return map.inverse(r);
  }
  else if (s < bias) { // overprediction
    uint k = bias - 1 - s;
    U d = (U(1) << k) + rd->template decode<U>(k);
    U p = map.forward(pred);
    U r = p - d;
    return map.inverse(r);
  }
  else                 // perfect prediction
    return map.identity(pred);
}
