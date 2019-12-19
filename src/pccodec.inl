template <typename U>
uint bsr(U x)
{
  uint k;
#if __i386__ && USEASM
  __asm__("bsr %1, %0" : "=r"(k) : "r"(x));
#else
  k = 0;
  do k++; while (x >>= 1);
  k--;
#endif
  return k;
}

