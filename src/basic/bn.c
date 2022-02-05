#include "basic/bn.h"
#include "core/bn.h"

#include <signal.h>
#include <stdlib.h>

// All `bn` results must be normalized (no leading zeros), because we depend on normalization in several places.
// typedef struct bn { bn_ bn; } bn;

static inline size_t min(size_t a, size_t b) {
  return a < b ? a : b;
}

static inline size_t max(size_t a, size_t b) {
  return a > b ? a : b;
}

static inline bn_* alloc(size_t nDigits) {
  assert(SIZE_MAX - sizeof(bn) >= nDigits);
  bn_* out = aligned_alloc(4, sizeof(bn_) + nDigits);
  assert(out != NULL);
  out->len = nDigits;
  return out;
}

static inline bn* newZero() {
  bn* dst = alloc(0);
  dst->len = 0;
  return dst;
}

////// Initialization //////

void bn_free(bn* src) {
  free(src);
}

bn* bn_umax(uintmax_t src) {
  bn* dst = alloc(sizeof(uintmax_t));
  bl_result err = bn__umax(dst, src);
  assert(err == BL_OK);
  return dst;
}

bn* bn_copy(const bn* src) {
  bn* dst = alloc(src->len);
  dst->len = src->len;
  bl_result err = bn__copy(dst, src);
  assert(err == BL_OK);
  return dst;
}

////// Queries //////

bool bn_bit(const bn* src, size_t i) {
  return bn__bit(src, i);
}

size_t bn_nBits(const bn* src) {
  if (src->len == 0) { return 0; }
  size_t out = 8 * src->len;
  uint8_t hiByte = src->base256le[src->len - 1];
  assert(hiByte != 0);
  while (!(hiByte & 0x80)) {
    out -= 1;
    hiByte = hiByte << 1;
  }
  return out;
}

uint8_t bn_byte(const bn* src, size_t i) {
  if (i >= src->len) { return 0; }
  return src->base256le[i];
}

size_t bn_nBytes(const bn* src) {
  return src->len;
}

bl_ord bn_cmp(const bn* a, const bn* b) {
  if (a == b) { return BL_EQ; }
  else if (a->len > b->len) { return BL_GT; }
  else if (a->len < b->len) { return BL_LT; }
  else { return bn__cmp(a, b); }
}

bool bn_eq(const bn* a, const bn* b) {
  return bn_cmp(a, b) == BL_EQ;
}
bool bn_neq(const bn* a, const bn* b) {
  return bn_cmp(a, b) != BL_EQ;
}
bool bn_lt(const bn* a, const bn* b) {
  return bn_cmp(a, b) == BL_LT;
}
bool bn_lte(const bn* a, const bn* b) {
  return bn_cmp(a, b) != BL_GT;
}
bool bn_gt(const bn* a, const bn* b) {
  return bn_cmp(a, b) == BL_GT;
}
bool bn_gte(const bn* a, const bn* b) {
  return bn_cmp(a, b) != BL_LT;
}

////// Bitwise //////

bn* bn_and(const bn* a, const bn* b) {
  bn* dst = alloc(bn__sizeof_and(a, b));
  bn__and(dst, a, b);
  bn__normalize(dst);
  return dst;
}

bn* bn_or(const bn* a, const bn* b) {
  bn* dst = alloc(bn__sizeof_or(a, b));
  bn__or(dst, a, b);
  bn__normalize(dst);
  return dst;
}

bn* bn_xor(const bn* a, const bn* b) {
  bn* dst = alloc(bn__sizeof_xor(a, b));
  bn__xor(dst, a, b);
  bn__normalize(dst);
  return dst;
}

////// Arithmetic //////

bn* bn_inc(const bn* a) {
  bn* dst = alloc(bn__sizeof_inc(a));
  dst->base256le[dst->len - 1] = 0;
  bl_result err = bn__inc(dst, a);
  assert(err == BL_OK);
  bn__normalize(dst);
  return dst;
}

bn* bn_add(const bn* a, const bn* b) {
  bn* dst = alloc(bn__sizeof_add(a, b));
  dst->base256le[dst->len - 1] = 0;
  bl_result err = bn__add(dst, a, b);
  assert(err == BL_OK);
  bn__normalize(dst);
  return dst;
}

bn* bn_dec(const bn* a) {
  bn* dst = alloc(bn__sizeof_dec(a));
  bl_result err = bn__dec(dst, a);
  assert(err == BL_OK);
  bn__normalize(dst);
  return dst;
}

bn* bn_sub(const bn* a, const bn* b) {
  if (b->len > a->len) { return newZero(); }
  bn* dst = alloc(bn__sizeof_sub(a, b));
  bl_result err = bn__sub(dst, a, b);
  if (err == BL_OVERFLOW) {
    dst->len = 0;
    return dst;
  }
  bn__normalize(dst);
  return dst;
}

bn* bn_mul(const bn* a, const bn* b) {
  if (a->len == 0 || b->len == 0) { return newZero(); }
  bn* dst = alloc(bn__sizeof_mul(a, b));
  bn__blank(dst);
  bl_result err = bn__mul(dst, a, b);
  assert(err == BL_OK);
  bn__normalize(dst);
  return dst;
}

struct bn_divmod bn_divmod(const bn* a, const bn* b) {
  if (b->len == 0) {
    raise(SIGFPE);
    assert(false && "raise(SGFPE) returned");
    exit(1);
  }
  struct bn_divmod dst;
  dst.div = alloc(bn__sizeof_div(a, b));
  dst.mod = alloc(bn__sizeof_mod(a, b));
  assert(dst.div != NULL);
  assert(dst.mod != NULL);
  bn__blank(dst.div);
  bn__blank(dst.mod);
  bl_result err = bn__divmod(dst.div, dst.mod, a, b);
  assert(err != BL_DIVZERO);
  assert(err == BL_OK);
  bn__normalize(dst.div);
  bn__normalize(dst.mod);
  return dst;
}


bn* bn_div(const bn* a, const bn* b) {
  struct bn_divmod r = bn_divmod(a, b);
  free(r.mod);
  return r.div;
}

bn* bn_mod(const bn* a, const bn* b) {
  struct bn_divmod r = bn_divmod(a, b);
  free(r.div);
  return r.mod;
}

////// Shifting //////

bn* bn_shr(bn* src, size_t amt) {
  bn* dst = bn_copy(src);
  bn__shr(dst, amt);
  bn__normalize(dst);
  return dst;
}

bn* bn_shl(bn* src, size_t amt) {
  size_t dstDigits = src->len + amt / 8 + 1; // not quite ceiling: pad the floor with an extra byte just in case
  bn* dst = alloc(dstDigits);
  bn__copy(dst, src);
  bn__shl(dst, amt);
  bn__normalize(dst);
  return dst;
}

////// Destructive Operations //////

bn_buffer* bn_new(size_t nBytes, bn* src) {
  bn_* dst = alloc(nBytes);
  if (src != NULL) {
    bn__copy(dst, src);
  }
  else {
    bn__blank(dst);
  }
  return dst;
}

bn* bn_create(bn_buffer* src, size_t maxBytes) {
  if (maxBytes < src->len) {
    src->len = maxBytes;
  }
  bn__normalize(src);
  return src;
}

void bn_writeBit(bn_buffer* dst, size_t i, bool bit) {
  bn__wrbit(dst, i, bit);
}

void bn_writeByte(bn_buffer* dst, size_t i, uint8_t byte) {
  if (i >= dst->len) { return; }
  dst->base256le[i] = byte;
}
