#include "core/bn.h"

#include <string.h>

static inline size_t min(size_t a, size_t b) {
  return a < b ? a : b;
}

static inline size_t max(size_t a, size_t b) {
  return a > b ? a : b;
}

////// Initialization helpers //////

size_t bn__sizeof(size_t nbits) {
  size_t ndigits = nbits / 8;
  if ((nbits % 8) != 0) { ndigits += 1; } // ceiling instead of floor
  assert(ndigits <= SIZE_MAX - sizeof(bn_));
  return sizeof(bn_) + ndigits;
}


void bn__blank(bn_* dst) {
  memset(&dst->base256le[0], 0, dst->len);
}

bl_result bn__copy(bn_*restrict dst, const bn_*restrict src) {
  if (dst->len < src->len) { return BL_OVERFLOW; }
  memcpy(&dst->base256le[0], &src->base256le[0], src->len);
  memset(&dst->base256le[src->len], 0, dst->len - src->len);
  return BL_OK;
}

bl_result bn__umax(bn_* dst, uintmax_t src) {
  for (size_t i = 0; i < dst->len; ++i) {
    dst->base256le[i] = src & 0xFF;
    src = src >> 8;
  }
  if (src != 0) {
    return BL_OVERFLOW;
  }
  else {
    dst->len = sizeof(src);
    bn__normalize(dst);
    return BL_OK;
  }
}

////// Queries //////


bool bn__bit(const bn_* src, size_t i) {
  size_t digit = i / 8,
         bit = i % 8;
  if (digit < src->len) {
    return (src->base256le[digit] >> bit) & 0x01;
  }
  else {
    return 0;
  }
}

bl_ord bn__cmp(const bn_* a, const bn_* b) {
  // ensure a is longer than b
  // and remember to flip the results if we had to flip inputs
  bool flip = false;
  if (a->len < b->len) {
    const bn_* tmp = a; a = b; b = tmp;
    flip = true;
  }
  // if high bytes of `a` are non-zero, `a` is larger
  for (size_t i = a->len; i > b->len; --i) {
    if (a->base256le[i - 1] != 0) { return !flip ? BL_GT : BL_LT; }
  }
  // otherwise, lexicographic compare in reverse
  for (size_t i = b->len; i > 0; --i) {
    uint8_t aDigit = a->base256le[i - 1];
    uint8_t bDigit = b->base256le[i - 1];
    if (aDigit > bDigit) { return !flip ? BL_GT : BL_LT; }
    else if (aDigit < bDigit) { return !flip ? BL_LT : BL_GT; }
  }
  // we made it to digit zero
  return BL_EQ;
}

////// Bitwise //////

size_t bn__sizeof_and(const bn_* a, const bn_* b) {
  return min(a->len, b->len);
}
void bn__and(bn_* dst, const bn_* a, const bn_* b) {
  size_t bigI = min(dst->len, min(a->len, b->len));
  for (size_t i = 0; i < bigI; ++i) {
    dst->base256le[i] = a->base256le[i] & b->base256le[i];
  }
  dst->len = bigI;
}

size_t bn__sizeof_or(const bn_* a, const bn_* b) {
  return max(a->len, b->len);
}
void bn__or(bn_* dst, const bn_* a, const bn_* b) {
  // ensure a is the longer argument
  if (a->len < b->len) {
    const bn_* tmp = a; a = b; b = tmp;
  }
  // iterate until we reach the end of b (the shorter one), `or`ing bits
  for (size_t i = 0; i < min(dst->len, b->len); ++i) {
    dst->base256le[i] = a->base256le[i] | b->base256le[i];
  }
  // iterate until we raech the end of a (the longer one), copying bits
  for (size_t i = b->len; i < min(dst->len, a->len); ++i) {
    dst->base256le[i] = a->base256le[i];
  }
}

size_t bn__sizeof_xor(const bn_* a, const bn_* b) {
  return max(a->len, b->len);
}
void bn__xor(bn_* dst, const bn_* a, const bn_* b) {
  // ensure a is the longer argument
  if (a->len < b->len) {
    const bn_* tmp = a; a = b; b = tmp;
  }
  // iterate until we reach the end of b (the shorter one), `xor`ing bits
  for (size_t i = 0; i < min(dst->len, b->len); ++i) {
    dst->base256le[i] = a->base256le[i] ^ b->base256le[i];
  }
  // iterate until we raech the end of a (the longer one), copying bits
  for (size_t i = b->len; i < min(dst->len, a->len); ++i) {
    dst->base256le[i] = a->base256le[i];
  }
}



////// Arithmetic //////

size_t bn__sizeof_add(const bn_* a, const bn_* b) {
  return (a->len >= b->len ? a->len : b->len) + 1;
}
bl_result bn__add(bn_* dst, const bn_* a, const bn_* b) {
  // commute so that `a` is at least as long as `b`
  if (a ->len < b->len) {
    const bn_* tmp = a; a = b; b = tmp;
  }
  uint16_t carry = 0;
  // perform additions when both a and b have digits
  for (size_t i = 0; i < min(dst->len, b->len); ++i) {
    carry += a->base256le[i];
    carry += b->base256le[i];
    dst->base256le[i] = carry & 0xFF;
    carry = carry >> 8;
  }
  // overflow if there are digits left in `b`
  if (dst->len < b->len) { return BL_OVERFLOW; }
  // propagate any carry through the digits of `a`
  for (size_t i = b->len; i < min(dst->len, a->len); ++i) {
    carry += a->base256le[i];
    dst->base256le[i] = carry & 0xFF;
    carry = carry >> 8;
  }
  // overflow if there are any digits left in `a`
  if (dst->len < a->len) { return BL_OVERFLOW; }
  // if there is a final carry, put it in the first digit of `dst` after `a`
  if (carry != 0) {
    if (dst->len == a->len) { return BL_OVERFLOW; }
    dst->base256le[a->len] = carry;
    // the size of the output is bounded by the size of `a` plus one
    dst->len = a->len + 1;
  }
  // otherwise, the size of `dst` is bounded by the size of `a`
  return BL_OK;
}

size_t bn__sizeof_sub(const bn_* a, const bn_* b) {
  return a->len;
}
bl_result bn__sub(bn_* dst, const bn_* a, const bn_* b) {
  if (b->len > a->len) { dst->len = 0; return BL_OVERFLOW; }
  int16_t borrow = 0;
  // perform subtractions while both `a` and `b` have digits
  for (size_t i = 0; i < min(dst->len, b->len); ++i) {
    borrow += a->base256le[i];
    borrow -= b->base256le[i];
    dst->base256le[i] = borrow & 0xFF;
    borrow = borrow >> 8;
  }
  // overflow if there are digits left in `b`
  if (dst->len < b->len) { return BL_OVERFLOW; }
  // propagate any borrow through the digits of `a`
  for (size_t i = b->len; i < min(dst->len, a->len); ++i) {
    borrow += a->base256le[i];
    dst->base256le[i] = borrow & 0xFF;
    borrow = borrow >> 8;
  }
  // if there are digits left in `a`, or there is a remaining borrow, that's an overflow
  if (dst->len < a->len || borrow != 0) {
    dst->len = 0; return BL_OVERFLOW;
  }
  return BL_OK;
}

size_t bn__sizeof_mul(const bn_* a, const bn_* b) {
  return a->len + b->len;
}
bl_result bn__mul(bn_* dst, const bn_* a, const bn_* b) {
  // check the destination is large enough
  // I'm pessimistic about this so that I can simplify everything else about the algorithm
  if (dst->len < a->len + b->len) { return BL_OVERFLOW; }
  for (size_t i = 0; i < a->len; ++i) {
    for (size_t j = 0; j < b->len; ++j) {
      // create a double-width multiply for the two digits
      uint16_t carry = (uint16_t)a->base256le[i] * b->base256le[j];
      // add them to the existing result, propagating any carries
      // no carry will make it past the last digit of `dst`, most will stop quickly
      for (size_t k = i + j; k < dst->len; k++) {
        carry += dst->base256le[k];
        dst->base256le[k] = carry & 0xFF;
        carry = carry >> 8;
        if (carry == 0) { break; }
      }
    }
  }
  return BL_OK;
}

bl_result bn__divmod(bn_* q, bn_* r, const bn_* n, const bn_* d) {
  // check the destinations are large enough
  // I'm pessimistic about this so that I can simplify everything else about the algorithm
  if (q->len < n->len || r->len < d->len) { return BL_OVERFLOW; }

  for (size_t i = 8*n->len; i > 0; --i) {
    bool nBit = bn__bit(n, i - 1);
    bn__shl(r, 1);
    bn__wrbit(r, 0, nBit);
    if (bn__cmp(r, d) != BL_LT) {
      bn__sub(r, r, d);
      bn__wrbit(q, i - 1, true);
    }
  }
  return BL_OK;
}

size_t bn__sizeof_div(const bn_* n, const bn_* d) {
  return n->len;
}
size_t bn__sizeof_mod(const bn_* n, const bn_* d) {
  return d->len;
}

////// Destructive //////

bl_result bn__wrbit(bn_* dst, size_t i, bool val) {
  size_t digit = i / 8;
  size_t bit = i % 8;
  if (digit >= dst->len) { return BL_OVERFLOW; }
  uint8_t overlay = 1 << bit;

  if (val) {
    dst->base256le[digit] |= overlay;
  }
  else {
    dst->base256le[digit] &= ~overlay;
  }
  return BL_OK;
}

void bn__shr(bn_* dst, size_t amt) {
  size_t digits = amt / 8; // the number of full digits to shift
  size_t bits = amt % 8;
  // work our way from least-to-most significant so as not to overwrite needed bits
  for (size_t dstI = 0, srcI = digits; dstI < dst->len - digits; ++dstI, ++srcI) {
    // access bytes that may have relevant bits
    uint8_t hiByte = (srcI + 1 < dst->len) ? dst->base256le[srcI + 1] : 0;
    uint8_t loByte = dst->base256le[srcI];
    // we now mix the low bits of the hi byte, with the high bits of the low byte
    uint8_t hiBits = hiByte << (8 - bits);
    uint8_t loBits = loByte >> bits;
    // and store it
    dst->base256le[dstI] = hiBits | loBits;
  }
  dst->len -= digits;
}

void bn__shl(bn_* dst, size_t amt) {
  size_t digits = amt / 8; // the number of full digits to shift
  size_t bits = amt % 8;
  // work our way from most-to-least significant so as not to overwrite needed bits
  for (size_t dstI = dst->len; dstI > digits; --dstI) {
    size_t srcI = dstI - digits;
    // access bytes that may have relevant bits
    uint8_t hiByte = dst->base256le[srcI - 1];
    uint8_t loByte = srcI > 1 ? dst->base256le[srcI - 2] : 0;
    // we now mix the low bits of the hi byte, with the high bits of the low byte
    uint8_t hiBits = hiByte << bits;
    uint8_t loBits = loByte >> (8 - bits);
    // and store it
    dst->base256le[dstI - 1] = hiBits | loBits;
  }
  // clear low-order bits; they have been moved
  for (size_t i = digits; i > 0; --i) {
    dst->base256le[i - 1] = 0;
  }
}

void bn__normalize(bn_* dst) {
  while (dst->len != 0) {
    if (dst->base256le[dst->len - 1] == 0) {
      --dst->len;
    }
    else {
      break;
    }
  }
}
