Basic Integer API
=================

:API Version: 0.1.0
:Date: 2022-02-??
:Copyright: 2022 Eric Demko
:License: BSD3

Overview
--------

This interface provides the simplest access to numerical operations on big integers, though it is the most memory-hungry (within reason).
The advantage we gain for the cost of memory is that it computations can be written in a very straightforward style, and it is easy to reason about which memory is still in-use.

Mutation
  The functions of this library are pure, up to memory allocation.

Memory Management
  Inputs are taken by-reference, and all outputs are placed in freshly-allocated memory
  obtained from the system ``aligned_alloc``.

  All output pointers are at least 4-byte aligned, which
  on most machines, should allow the two low-order bits to be used for tagging.

Error Handling
  In the case of divide-by-zero, we ``raise(SIGFPE)``, just as would normally
  happen with using C's division and modulo operators.

  Most functions in this library allocate memory, and could therefore fail, but we
  do not consider this likely and mostly don't bother checking. Normally, we
  ``assert`` that the results of allocations are non-null, but if asserts are
  turned off, it is likely that the program will quickly segfault in the event of
  an allocation failure.

Table of Contents
-----------------

  - `Construction and Destruction`_
  - `Conversions`_
  - `Arithmetic Operations`_
  - `Relational Operations`_
  - `Version Information`_
  - `Limits`_


Construction and Destruction
----------------------------

TODO
  Type ``bz``
  void bz_free(bz src);
  bz bz_imax(intmax_t src);
  bz bz_copy(bz src);


Conversions
-----------

TODO
  bz bz_positive(const bn* src);
  bz bz_negative(const bn* src);
  bn* bz_magnitude(bz src);


Arithmetic Operations
---------------------

TODO
  bz bz_neg(bz a);
  bz bz_add(bz a, bz b);
  bz bz_sub(bz a, bz b);
  bz bz_inc(bz a);
  bz bz_dec(bz a);
  bz bz_mul(bz a, bz b);
  struct bz_divmod
  struct bz_divmod bz_divmod(bz a, bz b);
  bz bz_div(bz a, bz b);
  bz bz_mod(bz a, bz b);


Relational Operations
---------------------

TODO
  bl_ord bz_cmp(bz a, bz b);
  bool bz_eq(bz a, bz b);
  bool bz_neq(bz a, bz b);
  bool bz_lt(bz a, bz b);
  bool bz_lte(bz a, bz b);
  bool bz_gt(bz a, bz b);
  bool bz_gte(bz a, bz b);
