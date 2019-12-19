#include <stdexcept>
#include "rcqsmodel.h"

// table size for binary search
#define TBLSHIFT 7

RCqsmodel::RCqsmodel(bool compress, uint symbols, uint bits, uint period) : RCmodel(symbols), bits(bits), targetrescale(period)
{
  if (bits > 16)
    throw std::domain_error("fpzip RCqsmodel bits too large");
  if (period >= (1u << (bits + 1)))
    throw std::domain_error("fpzip RCqsmodel period too large");

  uint n = symbols;
  symf = new uint[n + 1];
  cumf = new uint[n + 1];
  cumf[0] = 0;
  cumf[n] = 1u << bits;
  if (compress)
    search = 0;
  else {
    searchshift = bits - TBLSHIFT;
    search = new uint[(1 << TBLSHIFT) + 1];
  }
  reset();
}

RCqsmodel::~RCqsmodel()
{
  delete [] symf;
  delete [] cumf;
  delete [] search;
}

// reinitialize model
void RCqsmodel::reset()
{
  uint n = symbols;
  rescale = (n >> 4) | 2;
  more = 0;
  uint f = cumf[n] / n;
  uint m = cumf[n] % n;
  for (uint i = 0; i < m; i++)
    symf[i] = f + 1;
  for (uint i = m; i < n; i++)
    symf[i] = f;
  update();
}

// return symbol corresponding to cumulative frequency l
uint RCqsmodel::decode(uint& l, uint& r)
{
  uint i = l >> searchshift;
  uint s = search[i];
  uint h = search[i + 1] + 1;

  // find symbol via binary search
  while (s + 1 < h) {
    uint m = (s + h) / 2;
    if (l < cumf[m])
      h = m;
    else
      s = m;
  }

  l = cumf[s];
  r = cumf[s + 1] - l;
  update(s);

  return s;
}

// update probability table
void RCqsmodel::update()
{
  if (more) { // we have some more symbols before update
    left = more;
    more = 0;
    incr++;
    return;
  }
  if (rescale != targetrescale) {
    rescale *= 2;
    if (rescale > targetrescale)
      rescale = targetrescale;
  }

  // update symbol frequencies
  uint n = symbols;
  uint cf = cumf[n];
  uint count = cf;
  for (uint i = n; i--; ) {
    uint sf = symf[i];
    cf -= sf;
    cumf[i] = cf;
    sf = (sf >> 1) | 1;
    count -= sf;
    symf[i] = sf;
  }
  // assert(cf == 0);

  // count is now difference between target cumf[n] and sum of symf;
  // next actual rescale happens when sum of symf equals cumf[n]
  incr = count / rescale;
  more = count % rescale;
  left = rescale - more;

  // build lookup table for fast symbol searches
  if (search)
    for (uint i = n, h = 1 << TBLSHIFT; i--; h = cumf[i] >> searchshift)
      for (uint l = cumf[i] >> searchshift; l <= h; l++)
        search[l] = i;
}

// update frequency for symbol s
void RCqsmodel::update(uint s)
{
  if (!left)
    update();
  left--;
  symf[s] += incr;
}
