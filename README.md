# Bignum Literals

This is a C library optimized for portability and simplicity at the cost of performance.
It is _not_ meant for serious numerical calculation, there are already mature libraries for that.
Its primary reason for existence is to support the kind of arithmetic needed by portable cross-compilers to store and manipulate numerical literals in source code.
In such cases, literals are usually not large, and actual arithmetic is limited to one (or perhaps a few) optimizations.

I want this interface to balance the needs of reusability against ease-of-use.
In C, this is quite difficult to achieve, as memory management is tedious and verbose, but reusable functions should not assume the user has any particular memory-management strategy.
As such, the interface is split into two: a core interface which performs no allocations, and a small wrapper layer that uses system `malloc`.

The algorithms implemented aren't just simple, but even _naïve_.
Complexity will only be added when it makes a large difference in the performance of expected use-cases.
If you need more performance, it is very likely that a different bignum library would be better for you.

## Building

Clone the repository and run`./BUILD.sh`.
This will also run some basic tests.

## The Interface(s)

The interface of this library is split into core- and user-level, as well as by which number system it belongs to (ℕ, ℤ, ℚ, ℝ).
When building, you will choose which number systems you need support for, as well as what sort of interface you would like (usually a choice of user-level API).

### Common Interface

These interfaces are required by the core interface for every number system, but also are expected to be exported directly by user-level interfaces.

This defines the `bl_result` and `bl_ord` enums.
We use `bl_result` to report success vs. any possible error conditions (e.g. overflow, divide-by-zero).
We use `bl_ord` to describe orderings (`BL_{LT,EQ,GT}`).

### The Core Interface

The core interface is tedious to work with, but has the advantage of not allocating, and performs few checks and normalizations that might be duplicated by a user-level interface.
It is not meant to be used directly for applications, but instead only given the minimal primitives needed to implement a more friendly interface on top.

### User-level Interfaces

There is no definitive list of user-level libraries, as we cannot hope to enumerate all the options users have when it comes to memory management, and error handling.

It is not expected that an application will use more than one user-level interface.
Therefore, each of these should use similar naming conventions for exposed types and operations.

#### Basic Interface

One user-level library we do provide is one based on the system `malloc`.
It never performs in-place update on a number after it is created, and all results are passed back to the user in freshly-allocated memory that must later be `free`d.
The only possible error conditions are divide-by-zero and out-of-memory, both of which simply crash the program.
This provides an easy-to-use high-level interface, but its liberal use of allocation and pessimistic view on error recovery will certainly make it unsuitable in certain contexts.
Nevertheless, it is likely that it will cover most needs that are not already better covered by a more performance-sensitive bignum implementation.

One neat feature of this otherwise simplistic interface is that these align allocations on 4-byte boundaries.
This leaves the low two pits of a pointer free to pack in a sign bit and tag bit.

### Number Systems

#### Natural Numbers, ℕ

These numbers support:
  * arithmetic (`+`, `-`, `*`, `/`, `%`)
  * ordering
  * bitwise operations
  * shifting
  * digit extraction

The type here is called `bn`, and operations on it are prefixed with `bn_`.

- [ ] core
  - [x] bn
  - [x] bn_result
  - [x] sizeof
  - [x] blank, copy, small
  - [x] bit test
  - [x] bit assign, shift{l,r} (in-place)
  - [x] {,sizeof_}{and,or,xor} (complement would require infinite leading ones)
  - [x] {,sizeof_}{add,sub}
  - [x] bn_ord, cmp
  - [x] {,sizeof_}mul
  - [x] {,sizeof_}divmod
  - [ ] FIXME: core should not require normalization
- [ ] malloc
  - [ ] add, sub, mul, divmod, div, mod
  - [ ] bit test/assign
  - [ ] shl, shr
  - [ ] and, or, xor
  - [ ] cmp, eq, neq, lt, lte, gt, gte
  - [ ] read base256 digit

#### Integers, ℤ

These support:
  * arithmetic
  * ordering
  * conversion to/from ℕ

The type here is called `bz`, and operations on it are prefixed with `bz_`.


Importantly, integers do not support bitwise or shifting operations.
The semantics of these operations depend on the representation of sign.
Two's-complement is commonly assumed by programmers, but this requires there to be a "largest bit" in which to store the sign.
Arbitrary-precision numbers do not have such a bit, and so a common choice is sign-and-magnitude.
Between these two representations, which intuition should a user have?

In lieu of answering the question of representing sign, we defer it to applications.
It is here that where one or the other representation will be "more natural".
Internally, we use sign-and-magnitude, but thankfully, it is reasonably easy to implement either representation using digit extraction and logical operators.

- [ ] core
- [ ] malloc
  - [ ] magnitude, bn2bz
  - [ ] ℤ

#### Rationals, ℚ (TODO)

#### Floating-point Reals, ℝ (TODO)

  - [ ] support for NaN, ±∞, -0; as these may occur as literals

  - serialization
    - [ ] leb128, vlq
- [ ] system malloc
- [ ] system malloc, with tagged small numbers

#### Binary-coded Decimal (TODO)

Given the prevalence of decimal number literals in source code, and how likely number are to be printed in decimal, it may be worthwhile to provide BCD interfaces.
I'm still considering it though; if I get good enough performance without it, then it's probably not worth the effort of `bcd_adjust` functions.

#### Other Number Systems

Many languages often include support for ℂ, so one may wonder why we stop at ℝ.

To me, ℂ does not seem to follow the pattern established by ℕ → ℤ → ℚ → ℝ, for which every entry describes a total order.
The next logical extension of this pattern would be to a hyperreal system, which is usually defined in terms of equivalence classes over ℝ-valued series (similar to how ℝ is usually defined with equivalence classes over ℚ-values series.
In contrast, ℂ discards total ordering, and the next logical extensions (quaternions ℍ, octonians 𝕆, sednions, and so on using the Cayley-Dickson construction) sacrifice further algebraic properties as dimensionality is increased.

Further, ℂ is a sub-algebra of a more general 2-d geometric algebra, which does a much better job of describing the full range of space-like (and time-like) symmetries.
Essentially, any area in which you would use vectors for position and ℂ (or more usually ℍ) for rotations, a geometric algebra will handle both uniformly.
Since this is the major use-case of ℂ in computing, I consider it out of scope; we are here interested in numbering, not geometry.

## Related Work

  * gmp is a mature, if somewhat arcane, high-performance multi-precision library.
    It is GPL-licensed, which limits its flexibility, but is widely available to dynamically link with.
  * libtommath is a reasonably high-performance, reasonably-small library for bignum arithmetic.
    The tradeoffs it makes are certainly not bad for its use cases (supporting libtomcrypto, and teaching the basics of high-performance arithmetic).
    However, its design goals are broader than my own, which limits its ability to KISS.
    In cases where gmp is too arcane or restrictive, libtommath may be a good alternative.
  * tiny-bignum-c takes a similar KISS approach to my own.
    However, it is unsuitable for my particular purposes, because it sets a fixed upper limit on the size of numbers.
    While this is suitable for many applications, especially embedded ones, we all know that programmer who would paste a million digits of π into their source code and complain when the compiler truncates the result.
