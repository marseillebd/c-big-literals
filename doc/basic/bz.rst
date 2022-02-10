Basic Integer API
=================

`\<\<prev <Basic Natural Number API_>`_
`^up^ <Big Literals C Library_>`_
`next\>\> <Basic API Versioning_>`_

Overview
--------

This interface provides the simplest access to numerical operations on big integers, though it is the most memory-hungry (within reason).
The advantage we gain for the cost of memory is that it computations can be written in a very straightforward style, and it is easy to reason about which memory is still in-use.

It is closely related to the `Basic Natural Number API`_.

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

  - `ℤ Construction and Destruction`_
    - `Struct bz`_
    - `Function bz_imax`_
    - `Function bz_copy`_
    - `Procedure bz_free`_
  - `ℤ Conversions`_
    - `Function bz_positive`_
    - `Function bz_negative`_
    - `Function bz_magnitude`_
  - `ℤ Arithmetic Operations`_
    - `Function bz_neg`_
    - `Function bz_add`_
    - `Function bz_sub`_
    - `Function bz_inc`_
    - `Function bz_dec`_
    - `Function bz_mul`_
    - `Function bz_div`_
    - `Function bz_mod`_
    - `Struct bz_divmod`_
    - `Function bz_divmod`_
  - `ℤ Relational Operations`_
    - `Enum bl_ord`_
    - `Function bz_cmp`_
    - `Function bz_eq`_
    - `Function bz_neq`_
    - `Function bz_lt`_
    - `Function bz_lte`_
    - `Function bz_gt`_
    - `Function bz_gte`_


ℤ Construction and Destruction
------------------------------

`\<\<prev <Basic Integer API_>`_
`^up^ <Basic Integer API_>`_
`next\>\> <ℤ Arithmetic Operations_>`_

Struct ``bz``
~~~~~~~~~~~~~

This is a type which holds integer representations.
Although it can be created and modified on its own, it is recommended to work with these structures as if they were opaque;
that is, only use the functions of this library, which maintain the invariants and may not work correctly if the invariants are not maintained.

This is stored in sign-and-magnitude representation.
We reuse `Type bn`_ for the magnitude, and set the sign bit for negative.

Members
  - ``bool isNeg``: true iff the number is engative
  - ``bn* magnitude``: the absolute value of the number

Invariants
  - if ``magnitude == 0``, ``isNeg = False``

Note
  The magnitude pointers produced in this library are at least 4-byte aligned.
  On most machines, this should allow the two low-order bits to be used for tagging.

Lifetime & Ownership
  This type acts as a reference type itself.
  Thus, the ownership and lifetime of the ``magnitude`` member is exactly the lifetime and ownership of the struct itself.

See also
  - `Procedure bz_free`_ to release resources held by values of this type.

Function ``bz_imax``
~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz dst = bz_imax(intmax_t src)``

Semantics
  Create a new `Struct bz`_ which represents the same number as ``src``.
  We chose ``intmax_t``, since it is large enough to fit any signed integral C type;
    all other signed integral types can be promoted into this type.

Lifetime & Ownership
  The lifetime of ``dst`` begins, and its ownership resides with the caller.

Function ``bz_copy``
~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn dst = bz_copy(bn src)``

Semantics
  Create a new `Struct bz`_ which is equal to ``src``.

Lifetime & Ownership
  - ``src`` is an immutable borrow.
  - The lifetime of ``dst`` begins, and its ownership resides with the caller.

Procedure ``bz_free``
~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``void bz_free(bz self)``

Semantics
  Free resources associated with ``self``.

Lifetime & Ownership
  Takes ownership of ``self`` and ends its lifetime.


ℤ Conversions
-------------

`\<\<prev <ℤ Construction and Destruction_>`_
`^up^ <Basic Integer API_>`_
`next\>\> <ℤ Arithmetic Operations_>`_

Function ``bz_positive``
~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz dst = bz_positive(const bn* src)``

Semantics
  Create a non-negative integer with the same magnitude as the natural number ``src``.

Lifetime & Ownership
  - ``src`` is an immutable borrow.
  - The lifetime of ``dst`` begins, and its ownership resides with the caller.

See also
  - `Function bz_negative`_

Function ``bz_negative``
~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz dst = bz_negative(const bn* src)``

Semantics
  Create a non-positive integer with the same magnitude as the natural number ``src``.

Lifetime & Ownership
  - ``src`` is an immutable borrow.
  - The lifetime of ``dst`` begins, and its ownership resides with the caller.

See also
  - `Function bz_positive`_

Function ``bz_magnitude``
~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* dst = bz_magnitude(bz src)``

Semantics
  Create a natural number with the same magnitude as the integer ``src``.

Lifetime & Ownership
  - ``src`` is an immutable borrow.
  - The lifetime of ``dst`` begins, and its ownership resides with the caller.


ℤ Arithmetic Operations
-----------------------

`\<\<prev <ℤ Conversions_>`_
`^up^ <Basic Integer API_>`_
`next\>\> <ℤ Relational Operations_>`_

Function ``bz_neg``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_neg(bz a)``

Semantics
  return the negation of the input number

  :math:`\mathtt c = -\mathtt a`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(1)`

Function ``bz_add``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_add(bz a, bz b)``

Semantics
  Add the two numbers.

  :math:`\mathtt c = \mathtt a + \mathtt b`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\max(\mathtt a, \mathtt b) + 1))`

Function ``bz_sub``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_sub(bz a, bz b)``

Semantics
  Subtract the second number from the first.

  :math:`\mathtt c = \mathtt a - \mathtt b`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\max(\mathtt a, \mathtt b)))`

Function ``bz_inc``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_inc(bz a)``

Semantics
  Add one to the number.

  :math:`\mathtt c = \mathtt a + 1`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a + 1))`

Function ``bz_dec``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_dec(bz a)``

Semantics
  Subtract one from the number.

  :math:`\mathtt c = \mathtt a - 1`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a))`

Function ``bz_mul``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_mul(bz a, bz b)``

Semantics
  Multiply the two numbers.

  :math:`\mathtt c = \mathtt a \times \mathtt b`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a) \times \log_{256}(\mathtt b))`

Function ``bz_div``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_div(bz a, bz b)``

Semantics
  Compute the quotient of the two numbers.

  Let :math:`q, r \in \mathbb Z` be the unique integers such that :math:`\mathtt a = q * \mathtt b + r`
  and :math:`0 \leq r < |\mathtt b|`.
  Then :math:`\mathtt c = q`.
  That is, :math:`q = \mathrm{sgn}(\mathtt b) \left\lfloor \mathtt a/|\mathtt b| \right\rfloor`.

  See `Function bz_divmod`_ for more details.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_2(\mathtt a) \times 3\log_{256}(\mathtt b))`

  If both the quotient and remainder are needed, prefer `Function bz_divmod`_.

See also
  - `Function bz_divmod`_
  - `Function bz_mod`_

Function ``bz_mod``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bz c = bz_mod(bz a, bz b)``

Semantics
  Compute the remainder after division of the two numbers.

  Let :math:`q, r \in \mathbb N` be the unique natural numbers such that :math:`\mathtt a = q * \mathtt b + r`
  and :math:`0 \leq r < |\mathtt b|`.
  Then :math:`\mathtt c = r`.
  That is, :math:`r = \mathtt a - |\mathtt b| \left\lfloor \mathtt a/|\mathtt b| \right\rfloor`.

  See `Function bz_divmod`_ for more details.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_2(\mathtt a) \times 3\log_{256}(\mathtt b))`

  If both the quotient and remainder are needed, prefer `Function bz_divmod`_.

See also
  - `Function bz_divmod`_
  - `Function bz_div`_

Struct ``bz_divmod``
~~~~~~~~~~~~~~~~~~~~

Hold both the quotient and remainder for the result of `Function bz_divmod`_.

Members
  - read-only ``bz div``: the quotient, a.k.a. dividend
  - read-only ``bz mod``: the remainder, a.k.a. modulus

Lifetime & Ownership
  Both members are immutable borrows.

Note
  There is no type synonym for this struct, as the name ``bz_divmod`` is already taken by `Function bz_divmod`_.

Function ``bz_divmod``
~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``struct bz_divmod r = bz_divmod(bz a, bz b)``

Semantics
  Compute the quotient and remainder of the two numbers.
  We use Euclidean division.

  :math:`q, r = \mathtt{r.div}, \mathtt{r.mod}` are the unique integers
  such that :math:`\mathtt a = q * \mathtt b + r` and :math:`0 \leq r < |b|`.
  That is, :math:`q = \mathrm{sgn}(\mathtt b) \left\lfloor \mathtt a/|\mathtt b| \right\rfloor`,
  and :math:`r = \mathtt a - |\mathtt b| \left\lfloor \mathtt a/|\mathtt b| \right\rfloor`.
  This can also be expressed as
  :math:`q = \left\lfloor a/b \right\rfloor` when :math:`b > 0`,
  and :math:`q = \left\lceil a/b \right\rceil` when :math:`b < 0`.

  More concretely, for :math:`a, b > 0`:

  - :math:`\mathtt{bz\_divmod}(a, b)\mathtt{.div} = \left\lfloor a/b \right\rfloor`
  - :math:`\mathtt{bz\_divmod}(-a, b)\mathtt{.div} = {-\left\lceil a/b \right\rceil}`
  - :math:`\mathtt{bz\_divmod}(a, -b)\mathtt{.div} = {-\left\lfloor a/b \right\rfloor}`
  - :math:`\mathtt{bz\_divmod}(-a, -b)\mathtt{.div} = \left\lceil a/b \right\rceil`

Lifetime & Ownership
  - The lifetime of ``r.div`` begins, and its ownership resides with the caller.
  - The lifetime of ``r.mod`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_2(\mathtt a) \times 3\log_{256}(\mathtt b))`

See also
  - `Struct bz_divmod`_
  - `Function bz_div`_
  - `Function bz_mod`_


ℤ Relational Operations
-----------------------

`\<\<prev <ℤ Arithmetic Operations_>`_
`^up^ <Basic Integer API_>`_

Function ``bz_cmp``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bl_ord r = bz_cmp(const bz* a, const bz* b)``

Semantics
  Return if ``a`` is less than, equal to, or greater than ``b``.

  - ``r = BL_LT`` if ``a < b``
  - ``r = BL_EQ`` if ``a = b``
  - ``r = BL_GT`` if ``a > b``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_

Function ``bz_eq``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool eq = bz_eq(const bz* a, const bz* b)``

Semantics
  ``bz_eq(a, b) ≡ bz_cmp(a, b) == BL_EQ``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bz_cmp`_

Function ``bz_neq``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool neq = bz_neq(const bz* a, const bz* b)``

Semantics
  ``bz_neq(a, b) ≡ bz_cmp(a, b) != BL_EQ``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bz_cmp`_

Function ``bz_lt``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool lt = bz_lt(const bz* a, const bz* b)``

Semantics
  ``bz_lt(a, b) ≡ bz_cmp(a, b) == BL_LT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bz_cmp`_

Function ``bz_lte``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool lte = bz_lte(const bz* a, const bz* b)``

Semantics
  ``bz_lte(a, b) ≡ bz_cmp(a, b) != BL_GT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bz_cmp`_

Function ``bz_gt``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool gt = bz_gt(const bz* a, const bz* b)``

Semantics
  ``bz_gt(a, b) ≡ bz_cmp(a, b) == BL_GT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bz_cmp`_

Function ``bz_gte``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool gte = bz_gte(const bz* a, const bz* b)``

Semantics
  ``bz_gte(a, b) ≡ bz_cmp(a, b) != BL_LT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bz_cmp`_
