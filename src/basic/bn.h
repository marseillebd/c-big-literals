#ifndef BIGLIT_BASIC_BN
#define BIGLIT_BASIC_BN

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common.h"

// This interface uses the system allocator (specifically `aligned_alloc`) to create memory for all results.
// Results are always placed in a fresh pointer rather than attempt to share pointers.
// Thus, to deallocate later, call `bn_free` on any `bn*` returned by the functions of this library.
// The only arithmetic error condition allowed is divide-by-zero, and this raises the FPE signal.

typedef struct bn_ bn;

////// Initialization //////

void bn_free(bn* src);

// Create a natural number from a native unsigned integer.
bn* bn_umax(uintmax_t src);

bn* bn_copy(const bn* src);

////// Queries //////

// Return whether bit `i` is set in `src`.
bool bn_bit(const bn* src, size_t i);

size_t bn_nBits(const bn* src);

// Return the `i`th byte of `src` (zero-indexed, little-endian).
uint8_t bn_byte(const bn* src, size_t i);

size_t bn_nBytes(const bn* src);

// Return if the first number is less than/equal to/greater than the second.
bl_ord bn_cmp(const bn* a, const bn* b);

bool bn_eq(const bn* a, const bn* b);
bool bn_neq(const bn* a, const bn* b);
bool bn_lt(const bn* a, const bn* b);
bool bn_lte(const bn* a, const bn* b);
bool bn_gt(const bn* a, const bn* b);
bool bn_gte(const bn* a, const bn* b);

////// Bitwise //////

bn* bn_and(const bn* a, const bn* b);

bn* bn_or(const bn* a, const bn* b);

bn* bn_xor(const bn* a, const bn* b);

////// Arithmetic //////

bn* bn_inc(const bn* a);

bn* bn_add(const bn* a, const bn* b);

// if `a = 0`, returns zero.
bn* bn_dec(const bn* a);

// if `b > a`, returns zero.
bn* bn_sub(const bn* a, const bn* b);

bn* bn_mul(const bn* a, const bn* b);

struct bn_divmod {
  bn* div;
  bn* mod;
};
struct bn_divmod bn_divmod(const bn* a, const bn* b);

bn* bn_div(const bn* a, const bn* b);

bn* bn_mod(const bn* a, const bn* b);

////// Shifting //////

bn* bn_shr(bn* src, size_t amt);

bn* bn_shl(bn* src, size_t amt);

////// Destructive Operations //////

typedef struct bn_ bn_buffer;

// Create a new buffer with nBytes base256 digits.
// It is initialized to src if it is non-null (ignoring overflow), or else zero.
// When finished with the buffer, either pass to `bn_create` or `free`.
bn_buffer* bn_new(size_t nBytes, bn* src);

// Create a big natural from a buffer.
// Any bytes larger than `maxBytes` will be truncated; set to `SIZE_MAX` to never truncate.
bn* bn_create(bn_buffer* src, size_t maxBytes);

// If the index is out of bounds, the write is ignored.
void bn_writeBit(bn_buffer* dst, size_t i, bool bit);

// If the index is out of bounds, the write is ignored.
void bn_writeByte(bn_buffer* dst, size_t i, uint8_t byte);

#endif
