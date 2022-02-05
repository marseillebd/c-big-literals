#ifndef BIGLIT_BASIC_BZ
#define BIGLIT_BASIC_BZ

#include "basic/bn.h"
#include "core/bz_type.h"

// This interface uses the system allocator (specifically `aligned_alloc`) to create memory for all results.
// Results are always placed in a fresh pointer rather than attempt to share pointers.
// Thus, to deallocate later, call `bz_free` on any `bz` returned by the functions of this library.
// The only arithmetic error condition allowed is divide-by-zero, and this raises the FPE signal.

typedef bz_ bz;

////// Initialization //////

void bz_free(bz src);

// Create an integer from a native signed integer.
bz bz_imax(intmax_t src);

bz bz_copy(bz src);

////// Conversion //////

bz bz_positive(const bn* src);

bz bz_negative(const bn* src);

bn* bz_magnitude(bz src);

////// Queries //////

bl_ord bz_cmp(bz a, bz b);

bool bz_eq(bz a, bz b);
bool bz_neq(bz a, bz b);
bool bz_lt(bz a, bz b);
bool bz_lte(bz a, bz b);
bool bz_gt(bz a, bz b);
bool bz_gte(bz a, bz b);

////// Arithmetic //////

bz bz_neg(bz a);

bz bz_inc(bz a);

bz bz_add(bz a, bz b);

bz bz_dec(bz a);

bz bz_sub(bz a, bz b);

bz bz_mul(bz a, bz b);

// We use Euclidean division (E-division), for its excellent mathematical properties.
// Essentially, in addition to the usual requirements of correct integer division,
// we also require the remainder to be non-negative.
// One nice thing is the `isOdd(n) = n % 2 == 1` is correct under E-division,
//   though it is a common pitfall in most languages (which tend to use T- or F-division).
// Boute in "The Euclidean definition of the functions div and mod" has further details about the advantages.
//
// The intuition behind Boute is somewhat advanced (referring to Euclid's theorem and Euclidean ring axioms),
//   so I will offer another justification here, requiring only some experience with elementary algebra and modular arithmetic
//   (which you likely learned in primary school, and in undergrad respectively).
// In addition to the obvious properties that any sane integer division algorithm must abide by
//   * q ∈ ℤ
//   * a = b * q + r
//   * |r| < |b|
// I would like to satisfy
//   * a / b = -a / -b  (cancelling negatives)
//   * a % b = -a % -b  (more cancelling negatives)
//   * 0 ≤ r < |b|      (clocks don't have negative hours on them)
//   * `-a % b ≡ -(a % b) (mod b)`  (a negation goes around the clock backwards)
// What we "sacrifice" is
//   * -a % b = -(a % b)
// which is fine, because such a property would amount to the untrue `floor(-a) = -floor(a)`.
//
// a,b   | 4,3 | -4,3 | 4,-3 | -4,-3
// a / b |  1  |  -2  |  -2  |   1
// a % b |  1  |   2  |   2  |   1
struct bz_divmod {
  bz div;
  bz mod;
};
struct bz_divmod bz_divmod(bz a, bz b);

bz bz_div(bz a, bz b);

bz bz_mod(bz a, bz b);


#endif
