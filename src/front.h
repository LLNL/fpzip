#ifndef FRONT_H
#define FRONT_H

#include "types.h"

// front of encoded but not finalized samples
template <typename T>
class Front {
public:
  Front(uint nx, uint ny, T zero = 0)
    : zero(zero), dx(1), dy(nx + 1), dz(dy * (ny + 1)), m(mask(dx + dy + dz)),
      i(0), a(new T[m + 1]) {}
  ~Front() { delete[] a; }

  // fetch neighbor relative to current sample
  const T& operator()(uint x, uint y, uint z) const
  {
    return a[(i - dx * x - dy * y - dz * z) & m];
  }

  // add n copies of sample f to front
  void push(T f, uint n = 1)
  {
    do
      a[i++ & m] = f;
    while (--n);
  }

  // advance front to (x, y, z) relative to current sample and fill with zeros
  void advance(uint x, uint y, uint z)
  {
    push(zero, dx * x + dy * y + dz * z);
  }

private:
  const T    zero; // default value
  const uint dx;   // front index x offset
  const uint dy;   // front index y offset
  const uint dz;   // front index z offset
  const uint m;    // index mask
  uint       i;    // modular index of current sample
  T*const    a;    // circular array of samples

  // return m = 2^k - 1 >= n - 1
  uint mask(uint n) const
  {
    for (n--; n & (n + 1); n |= n + 1);
    return n;
  }
};

#endif
