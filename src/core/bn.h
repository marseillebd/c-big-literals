#ifndef BN_CORE_BN_H
#define BN_CORE_BN_H

// This is the core interface for big natural numbers ℕ. It is only meant to be
// exposed to advanced users, such as those implementing a more convenient API
// on top of this one. Its functions do not allocate, and in the interests of
// avoiding possibly unnecessary work, do not maintain all invariants.

// The names in this header are prefixed `bn__`, and the core `bn_` also
// ends in an underscore. It is expected that the layer above will define `bn`
// and functions `bn_*` that wrap these ones, so the unusual namign nkeeps the
// core out of the way of API builders.

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common.h"

static_assert(CHAR_BIT == 8, "target does not have an 8-bit byte");
static_assert(sizeof(uint8_t) == 1, "target uses more than one byte to store uint8_t");
static_assert((int16_t)-1 >> 1 == -1, "target does not perform arithmetic shift on uint16_t");

typedef struct bn_ {
  size_t len;
  uint8_t base256le[];
} bn_;

////// Initialization //////

// Return the size in bytes of the memory required to store a number with `nbits` bits.
size_t bn__sizeof(size_t nbits);

// Set all digits of the destination to zero.
// The desintation is _not_ normalized.
void bn__blank(bn_* dst);

// Copy the digits of the source to the destination without changing the length.
// The most significant bits in the destination are zeroed when the source is smaller.
// The desintation is _not_ normalized.
bl_result bn__copy(bn_*restrict dst, const bn_*restrict src);

// Place the bytes of the source integer into the destination.
// The destination _is_ normalized.
// If the destination is too small to fit all the bits of src, BN_OVERFLOW is returned.
bl_result bn__u64(bn_* dst, uint64_t src);

////// Queries //////

// Return whether bit `i` (zero-indexed, little-endian) is set (return one) or not (return zero).
bool bn__bit(const bn_* src, size_t i);

// Return if the first number is les sthan/equal to/greater than the second.
// This function requires both inputs to be normalized.
bl_ord bn__cmp(const bn_* a, const bn_* b);

////// Bitwise //////

void bn__and(bn_* dst, const bn_* a, const bn_* b);
size_t bn__sizeof_and(const bn_* a, const bn_* b);

void bn__or(bn_* dst, const bn_* a, const bn_* b);
size_t bn__sizeof_or(const bn_* a, const bn_* b);

void bn__xor(bn_* dst, const bn_* a, const bn_* b);
size_t bn__sizeof_xor(const bn_* a, const bn_* b);

////// Arithmetic //////

// Place the result of `a + b` in `dst`.
// `a` and `b` must be normalized.
// The destination _is_ normalized.
// This does _not_ require the destination to be blank.
// If the destination does not have enough space to store the result (as determiend by bn__sizeof_add),
// `BN_OVERFLOW` is returned, and the contents of `dst` are undefined.
bl_result bn__add(bn_* dst, const bn_* a, const bn_* b);
// Return the maximum size (in base256 digits) that must be allocated to hold a `bn_` for the result of `a + b`.
size_t bn__sizeof_add(const bn_* a, const bn_* b);

// Place the result of `a - b` in `dst`.
// `a` and `b` must be normalized.
// The destination _is_ normalized.
// This does _not_ require the destination to be blank.
// If `b` is larger than `a`, or `dst` does not have enough space to hold the result (as determiend by bn__sizeof_sub),
// `BN_OVERFLOW` is returned, and the contents of `dst` are undefined.
bl_result bn__sub(bn_* dst, const bn_* a, const bn_* b);
// Return the maximum size (in base256 digits) that must be allocated to hold a `bn_` for the result of `a - b`.
size_t bn__sizeof_sub(const bn_* a, const bn_* b);

// Place the result of `a * b` in `dst`.
// `a` and `b` must be normalized.
// The destination _must_ be provided blank.
// The destination _is_ normalized.
// If the destination does not have enough space to store the result (as determiend by bn__sizeof_mul),
// `BN_OVERFLOW` is returned, and the contents of `dst` are undefined.
bl_result bn__mul(bn_* dst, const bn_* a, const bn_* b);
// Return the maximum size (in base256 digits) that must be allocated to hold a `bn_` for the result of `a * b`.
size_t bn__sizeof_mul(const bn_* a, const bn_* b);

// Place the result of `n / d` in `q` and `n % d` in `r`.
// `a` and `b` must be normalized.
// The destinations _must_ be provided blank.
// The destinations _are_ normalized.
// If `q` and `r` do not have enough space to store intermediate results
//   (as determiend by bn__sizeof_div and bn__sizeof_mod respectively),
//   `BN_OVERFLOW` is returned, and the contents of `q` and `r` are undefined.
// If `d` is zero, `BN_DIVZERO` will be returned, and teh contents of `q` and `r` are undefined.
//
// Of course the resulting `q * d + r == n`, but we also have the restriction that `0 ≤ r < d`.
// The latter actually also trivial in the unsigned case here,
//   but I point it out here because there is a choice in the signed case;
//   modular arithmetic requires the latter property to hold even in ℤ,
//   and this library's namign conventions will stay consistend with that.
bl_result bn__divmod(bn_* q, bn_* r, const bn_* n, const bn_* d);
// Return the maximum size (in base256 digits) that must be allocated to hold a `bn_` for the result of `a / b`.
size_t bn__sizeof_div(const bn_* n, const bn_* d);
// Return the maximum size (in base256 digits) that must be allocated to hold a `bn_` for the result of `a % b`.
size_t bn__sizeof_mod(const bn_* n, const bn_* d);


////// Destructive Operations //////

// These operations are are implemented as in-place updates because they are
// used to create intermediate results for other operations. They aren't meant
// to be exported directly, but since this is the core interface, we leave it to
// other interfaces to hide these and expose non-destructive versions (e.g. copy
// and delegate).

// Set bit `i` (zero indexed from the least significant) to zero (true) or one (false).
bl_result bn__wrbit(bn_* dst, size_t i, bool val);

// Shift right `amt` bits.
// Since `bn` is an unsigned type, there is no difference between logical and arithmetic here.
// The desintation is _not_ normalized, but NOTE that this function shrinks the length by `floor(amt/8)`.
// The shrink is done instead of (possibly unnessecarily) writing zeros over the most significant bits;
// if a client API needs the size to remain the same, this is easily accomplished by resetting length and calling memset appropriately.
void bn__shr(bn_* dst, size_t amt);

// Shift left `amt` bits.
// If high bits are lost, this is not detected.
// The desintation is _not_ normalized.
void bn__shl(bn_* dst, size_t amt);

// Normalize the destinateion.
// This is given to help implement a nicer interface over those core functions that do not normalize.
void bn__normalize(bn_* dst);


#endif