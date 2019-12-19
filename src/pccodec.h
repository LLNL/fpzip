#ifndef PC_CODEC_H
#define PC_CODEC_H

#include "types.h"

#ifndef PC_BIT_MAX
  #define PC_BIT_MAX 8 // maximum bit width of directly encodable integers
#endif

namespace PC {
  template <typename U>
  uint bsr(U x);
  #include "pccodec.inl"
}

#endif
