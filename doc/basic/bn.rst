Basic Natural Number API
========================

:API Version: 0.1.0
:Date: 2022-02-07
:Copyright: 2022 Eric Demko
:License: BSD3

Overview
--------

This interface provides the simplest access to numerical operations on big natural numbers, though it is the most memory-hungry (within reason).
The advantage we gain for the cost of memory is that it computations can be written in a very straightforward style, and it is easy to reason about which memory is still in-use.

Mutation
  The primary functions of this library are pure, up to memory allocation.
  The functions in `Destructive Operations`_ are the exception.

Memory Management
  Inputs are taken by-reference, and all outputs are placed in freshly-allocated memory
  obtained from the system ``aligned_alloc``.

  All output pointers are at least 4-byte aligned, which
  on most machines, should allow the two low-order bits to be used for tagging.

Error Handling
  In the case of arithmetic overflow (such as subtraction of a larger number from
  a smaller number), results are undefined, possibly including crashes.

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
    - `Type bn`_
    - `Function bn_umax`_
    - `Function bn_copy`_
    - `Function bn_free`_
  - `Arithmetic Operations`_
    - `Function bn_add`_
    - `Function bn_sub`_
    - `Function bn_inc`_
    - `Function bn_dec`_
    - `Function bn_mul`_
    - `Function bn_div`_
    - `Function bn_mod`_
    - `Struct bn_divmod`_
    - `Function bn_divmod`_
  - `Relational Operations`_
    - `Enum bl_ord`_
    - `Function bn_cmp`_
    - `Function bl_eq`_
    - `Function bl_neq`_
    - `Function bl_lt`_
    - `Function bl_lte`_
    - `Function bl_gt`_
    - `Function bl_gte`_
  - `Bitwise Operations`_
    - `Function bn_and`_
    - `Function bn_or`_
    - `Function bn_xor`_
    - `Function bn_shr`_
    - `Function bn_shl`_
  - `Indexing Operations`_
    - `Function bn_bit`_
    - `Function bn_nBits`_
    - `Function bn_byte`_
    - `Function bn_nBytes`_
  - `Destructive Operations`_
    - `Type bn_buffer`_
    - `Function bn_new`_
    - `Function bn_create`_
    - `Procedure bn_writeBit`_
    - `Procedure bn_writeByte`_
  - `Version Information`_
  - `Limits`_


Construction and Destruction
----------------------------

`\<\<prev <Basic Natural Number API_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Arithmetic Operations_>`_

Type ``bn``
~~~~~~~~~~~

This is an opaque type which holds natural number representations.

Note
  The pointers to this type produced in this library are at least 4-byte aligned.
  On most machines, this should allow the two low-order bits to be used for tagging.

See also
  `Function bn_free`_ to release resources held by values of this type.

Function ``bn_umax``
~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* dst bn_umax(uintmax_t src)``

Semantics
  Create a new `Type bn`_ which represents the same number as ``src``.
  We chose ``uintmax_t``, since it is large enough to fit any unsigned integral C type;
    all other unsigned integral types can be promoted into this type.

  :``src``:
    A finite-size unsigned integer :math:`\mathtt{src} \in \mathbb Z/n\mathbb Z` for some implementation-defined :math:`n`.
  :``dst``:
    A natural number :math:`\mathtt{dst} = \mathtt{src} \in \mathbb N`.

Lifetime & Ownership
  The lifetime of ``dst`` begins, and its ownership resides with the caller.

Function ``bn_copy``
~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* dst = bn_copy(const bn* src)``

Semantics
  Create a new `Type bn`_ which is equal to ``src``.

  :``src``:
    A natural number :math:`\mathtt{src} \in \mathbb N`.
  :``dst``:
    A natural number :math:`\mathtt{dst} = \mathtt{src}`.

Lifetime & Ownership
  - ``src`` is an immutable borrow.
  - The lifetime of ``dst`` begins, and its ownership resides with the caller.

Function ``bn_free``
~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``void bn_free(bn* self)``

Semantics
  Free resources associated with ``self``.

Lifetime & Ownership
  Takes ownership of ``self`` and ends its lifetime.


Arithmetic Operations
---------------------

`\<\<prev <Construction and Destruction_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Relational Operations_>`_

Function ``bn_add``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_add(const bn* a, const bn* b)``

Semantics
  Add the two numbers.

  :math:`\mathtt c = \mathtt a + \mathtt b`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\max(\mathtt a, \mathtt b) + 1))`

Function ``bn_sub``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_sub(const bn* a, const bn* b)``

Semantics
  Subtract the second number from the first.
  If the subtraction would produce a number less than zero, the result is undefined.

  :math:`\mathtt c = \mathtt a - \mathtt b` when :math:`\mathtt b \leq \mathtt a`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\max(\mathtt a, \mathtt b)))`

Function ``bn_inc``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_inc(const bn* a)``

Semantics
  Add one to the number.

  :math:`\mathtt c = \mathtt a + 1`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a + 1))`

Function ``bn_dec``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_dec(const bn* a)``

Semantics
  Subtract one from the number.
  If the subtraction would produce a number less than zero, the result is undefined.

  :math:`\mathtt c = \mathtt a - 1` when :math:`a \neq 0`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a))`

Function ``bn_mul``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_mul(const bn* a, const bn* b)``

Semantics
  Multiply the two numbers.

  :math:`\mathtt c = \mathtt a \times \mathtt b`

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a) \times \log_{256}(\mathtt b))`

Function ``bn_div``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_div(const bn* a, const bn* b)``

Semantics
  Compute the quotient of the two numbers.

  Let :math:`q, r \in \mathbb N` be the unique natural numbers such that :math:`\mathtt a = q * \mathtt b + r`.
  Then :math:`\mathtt c = q`.
  That is, :math:`\mathtt c = \lfloor \mathtt a / \mathtt b \rfloor`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_2(\mathtt a) \times 3\log_{256}(\mathtt b))`

  If both the quotient and remainder are needed, prefer `Function bn_divmod`_.

See also
  - `Function bn_divmod`_
  - `Function bn_mod`_

Function ``bn_mod``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_mod(const bn* a, const bn* b)``

Semantics
  Compute the remainder after division of the two numbers.

  Let :math:`q, r \in \mathbb N` be the unique natural numbers such that :math:`\mathtt a = q * \mathtt b + r`.
  Then :math:`\mathtt c = r`.
  That is, :math:`\mathtt c = \mathtt a - \mathtt b \lfloor \mathtt a / \mathtt b \rfloor`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_2(\mathtt a) \times 3\log_{256}(\mathtt b))`

  If both the quotient and remainder are needed, prefer `Function bn_divmod`_.

See also
  - `Function bn_divmod`_
  - `Function bn_div`_

Struct ``bn_divmod``
~~~~~~~~~~~~~~~~~~~~

Hold both the quotient and remainder for the result of `Function bn_divmod`_.

Members
  - read-only ``bn* div``: the quotient, a.k.a. dividend
  - read-only ``bn* mod``: the remainder, a.k.a. modulus

Lifetime & Ownership
  Both members are immutable borrows.

Note
  There is no type synonym for this struct, as the name ``bn_divmod`` is already taken by `Function bn_divmod`_.

Function ``bn_divmod``
~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``struct bn_divmod r = bn_divmod(const bn* a, const bn* b)``

Semantics
  Compute the quotient and remainder of the two numbers.

  :math:`q, r = \mathtt{r.div}, \mathtt{r.mod}` are the unique natural numbers
  such that :math:`\mathtt a = q * \mathtt b + r`.
  That is, :math:`\mathtt{r.div} = \lfloor \mathtt a / \mathtt b \rfloor`,
  and :math:`\mathtt{r.mod} = \mathtt a - \mathtt b \lfloor \mathtt a / \mathtt b \rfloor`.

Lifetime & Ownership
  - The lifetime of ``r.div`` begins, and its ownership resides with the caller.
  - The lifetime of ``r.mod`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_2(\mathtt a) \times 3\log_{256}(\mathtt b))`

See also
  - `Struct bn_divmod`_
  - `Function bn_div`_
  - `Function bn_mod`_

Relational Operations
---------------------

`\<\<prev <Arithmetic Operations_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Bitwise Operations_>`_

Enum ``bl_ord``
~~~~~~~~~~~~~~~

Report the results of comparison.

Values
  - ``BL_LT`` for less than
  - ``BL_EQ`` for equal to
  - ``BL_GT`` for greater than

See also
  - `Function bn_cmp`_

Function ``bn_cmp``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bl_ord r = bn_cmp(const bn* a, const bn* b)``

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

Function ``bl_eq``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool eq = bn_eq(const bn* a, const bn* b)``

Semantics
  ``bn_eq(a, b) ≡ bn_cmp(a, b) == BL_EQ``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bn_cmp`_

Function ``bl_neq``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool neq = bn_neq(const bn* a, const bn* b)``

Semantics
  ``bn_neq(a, b) ≡ bn_cmp(a, b) != BL_EQ``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bn_cmp`_

Function ``bl_lt``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool lt = bn_lt(const bn* a, const bn* b)``

Semantics
  ``bn_lt(a, b) ≡ bn_cmp(a, b) == BL_LT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bn_cmp`_

Function ``bl_lte``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool lte = bn_lte(const bn* a, const bn* b)``

Semantics
  ``bn_lte(a, b) ≡ bn_cmp(a, b) != BL_GT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bn_cmp`_

Function ``bl_gt``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool gt = bn_gt(const bn* a, const bn* b)``

Semantics
  ``bn_gt(a, b) ≡ bn_cmp(a, b) == BL_GT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bn_cmp`_

Function ``bl_gte``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool gte = bn_gte(const bn* a, const bn* b)``

Semantics
  ``bn_gte(a, b) ≡ bn_cmp(a, b) != BL_LT``

Ownership and Lifetime
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

See also
  - `Enum bl_ord`_
  - `Function bn_cmp`_


Bitwise Operations
------------------

`\<\<prev <Relational Operations_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Indexing Operations_>`_

Function ``bn_and``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_and(const bn* a, const bn* b)``

Semantics
  Compute the bitwise and of the two numbers.

  :math:`\mathtt c = \mathtt a \mathbin{\&} \mathtt b`

  :``a``:
    A natural number :math:`\mathtt a = \sum a_i 2^i` with coefficients :math:`a_i \in \mathbb B`.
  :``b``:
    A natural number :math:`\mathtt b = \sum b_i 2^i` with coefficients :math:`a_i \in \mathbb B`.
  :``c``:
    The natural number :math:`\mathtt c = \sum (a_i \land b_i) 2^i`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\min(\mathtt a, \mathtt b)))`

Function ``bn_or``
~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_or(const bn* a, const bn* b)``

Semantics
  Compute the bitwise or of the two numbers.

  :math:`\mathtt c = \mathtt a \mathbin{|} \mathtt b`

  :``a``:
    A natural number :math:`\mathtt a = \sum a_i 2^i` with coefficients :math:`a_i \in \mathbb B`.
  :``b``:
    A natural number :math:`\mathtt b = \sum b_i 2^i` with coefficients :math:`a_i \in \mathbb B`.
  :``c``:
    The natural number :math:`\mathtt c = \sum (a_i \lor b_i) 2^i`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\max(\mathtt a, \mathtt b)))`

Function ``bn_xor``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_xor(const bn* a, const bn* b)``

Semantics
  Compute the bitwise exclusive or of the two numbers.

  :math:`\mathtt c = \mathtt a \oplus \mathtt b`

  :``a``:
    A natural number :math:`\mathtt a = \sum a_i 2^i` with coefficients :math:`a_i \in \mathbb B`.
  :``b``:
    A natural number :math:`\mathtt b = \sum b_i 2^i` with coefficients :math:`a_i \in \mathbb B`.
  :``c``:
    The natural number :math:`\mathtt c = \sum ((a_i \lor b_i) \land \lnot(a_i \land b_i)) 2^i`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.
  - ``b`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\max(\mathtt a, \mathtt b)))`

Function ``bn_shr``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_shr(bn* a, size_t amt)``

Semantics
  Shift the number right by ``amt`` bits.
  I.e. rightwards is towards the least-significant bit.

  :math:`\mathtt c = \lfloor \mathtt a / 2^\mathtt{amt} \rfloor`

  :``a``:
    A natural number :math:`\mathtt a = \sum a_i 2^i`
    with coefficients :math:`a_i \in \mathbb B`.
  :``amt``:
    A natural number :math:`\mathtt{amt} \in \mathbb N`.
  :``c``:
    The natural number :math:`\mathtt c = \sum a_{i+\mathtt{amt}} 2^i`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a))`

Function ``bn_shl``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* c = bn_shl(bn* a, size_t amt)``

Semantics
  Shift the number left by ``amt`` bits.
  I.e. leftwards is towards the most-significant bit.

  :math:`\mathtt c = \mathtt a \times 2^\mathtt{amt}`

  :``a``:
    A natural number :math:`\mathtt a = \sum a_i 2^i`
    with coefficients :math:`a_i \in \mathbb B`.
  :``amt``:
    A natural number :math:`\mathtt{amt} \in \mathbb N`.
  :``c``:
    The natural number :math:`\mathtt c = \sum a_i 2^{i+\mathtt{amt}}`.

Lifetime & Ownership
  - The lifetime of ``c`` begins, and its ownership resides with the caller.
  - ``a`` is an immutable borrow.

Performance
  :math:`O(\log_{256}(\mathtt a))`


Indexing Operations
-------------------

`\<\<prev <Bitwise Operations_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Destructive Operations_>`_


Function ``bn_bit``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bool isSet = bn_bit(const bn* src, size_t i)``

Semantics
  Return whether bit ``i`` of the ``src`` is set.
  Bits are zero-indexed and little-endian.

  :``src``:
    A natural number :math:`\mathtt{src} = \sum a_i 2^i \in \mathbb N` with coefficients :math:`a_i \in \mathbb B`.
  :``i``:
    An index in :math:`\mathtt{i} \in \mathbb N`.

    The maximum query-able index is `limited <Limits_>`_ by the size of the machine.
  :``isSet``:
    A boolean :math:`\mathtt{isSet} = a_\mathtt{i}`.

Ownership and Lifetime
  ``src`` is an immutable borrow.

See also
  - `Function bn_byte`_

Function ``bn_nBits``
~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``size_t n = bn_nBits(const bn* src)``

Semantics
  Return the index of the highest set bit in ``src``.
  Bits are zero-indexed and little-endian.

  :``src``:
    A natural number :math:`\mathtt{src} = \sum a_i 2^i \in \mathbb N` with coefficients :math:`a_i \in \mathbb B`.
  :``n``:
    An index :math:`\mathtt{n} \in \mathbb N`
    such that :math:`a_\mathtt{n} = 1` and :math:`\forall i > \mathtt{n}.\;a_i = 0`.

    The maximum resulting index is `limited <Limits_>`_ by the size of the machine.

Ownership and Lifetime
  ``src`` is an immutable borrow.

See also
  - `Function bn_nBytes`_

Function ``bn_byte``
~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``uint8_t b = bn_byte(const bn* src, size_t i)``

Semantics
  Return the ``i``th base-256 digit of ``src``.
  Bytes are zero-indexed and little-endian.

  :``src``:
    A natural number :math:`\mathtt{src} = \sum a_i 256^i \in \mathbb N`
    with coefficients :math:`a_i \in \mathbb Z / 256\mathbb Z`.
  :``i``:
    An index in :math:`\mathtt{i} \in \mathbb N`.

    The maximum query-able index is `limited <Limits_>`_ by the size of the machine.
  :``b``:
    A coefficient :math:`0 \leq \mathtt{b} = a_\mathtt{i} \leq 255`.

Ownership and Lifetime
  ``src`` is an immutable borrow.

See also
  - `Function bn_bit`_

Function ``bn_nBytes``
~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``size_t n = bn_nBytes(const bn* src)``

Semantics
  Return the index of the highest non-zero byte in ``src``.
  Bytes are zero-indexed and little-endian.

  :``src``:
    A natural number :math:`\mathtt{src} = \sum a_i 256^i \in \mathbb N`
    with coefficients :math:`a_i \in \mathbb Z / 256\mathbb Z`.
  :``n``:
    An index :math:`\mathtt{n} \in \mathbb N`
    such that :math:`a_\mathtt{n} > 0` and :math:`\forall i > \mathtt{n}.\;a_i = 0`.

    The maximum resulting index is `limited <Limits_>`_ by the size of the machine.

Ownership and Lifetime
  ``src`` is an immutable borrow.

See also
  - `Function bn_nBits`_

Destructive Operations
----------------------

`\<\<prev <Indexing Operations_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Version Information_>`_

This part of the interface is not recommended for general use.
It does, however, provide an escape hatch which can be used to reasonably-efficiently produce natural numbers using algorithms not listed in the rest of the interface.
Its only real performance advantages is that the memory can be re-used for intermediate results.

Type ``bn_buffer``
~~~~~~~~~~~~~~~~~~

This is an opaque type which holds mutable natural number representations.
The maximum size of natural number it can hold is set when it is created, and cannot later be altered.

Pointers of this type are created by the system ``aligned_alloc``, and should be passed either to `Function bn_create`_ or ``free`` when no longer required.
Beware: ``realloc`` provides no alignment guarantees.

Note
  The pointers to this type produced in this library are at least 4-byte aligned.
  On most machines, this should allow the two low-order bits to be used for tagging.

See also
  `Function bn_create`_ to convert values of this type into ``bn`` values.

Function ``bn_new``
~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn_buffer* dst = bn_new(size_t nBytes, bn* src)``

Semantics
  Create a new buffer which can hold natural numbers up to but not including :math:`256^\mathtt{nBytes}`.

  :``src``:
    A nullable pointer to an initial number.
    If non-null, the initial value of :math:`\mathtt{dst} \equiv \mathtt{src}\:(\text{mod }256^\mathtt{nBytes})`.
    If null, the initial value of :math:`\mathtt{dst} = 0`.

Lifetime & Ownership
  - The lifetime of ``dst`` begins, and its ownership resides with the caller.
  - ``src`` is an immutable borrow.

Note
  The maximum number of bytes in the buffer is `limited <Limits_>`_ by the system.

See also
  - `Type bn_buffer`_
  - `Function bn_create`_

Function ``bn_create``
~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn* n = bn_create(bn_buffer* src, size_t maxBytes)``

Semantics
  Freeze the contents of the buffer to create a natural number.

  :maxBytes:
    The maximum number of bytes that might contribute to the final number.
    Pass ``SIZE_MAX`` to never truncate (up to the system `limits <Limits_>`_).
  :n:
    :math:`\mathtt n \equiv \mathtt{src}\:(\text{mod }256^{\mathtt{maxBytes}})`

Note
  The maximum number of bytes in the result is `limited <Limits_>`_ by the system.

Procedure ``bn_writeBit``
~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn_writeBit(bn_buffer* dst, size_t i, bool bit)``

Semantics
  Set or clear bit ``i`` of the ``dst`` buffer.
  Bits are zero-indexed and little-endian.

  :``i``:
    An index in :math:`\mathtt{i} \in \mathbb N`.

    The maximum write-able index is limited by the size of the buffer when it was created.
    Writes to indices above :math:`8\times\mathtt{maxBytes}` are ignored.
  :``bit``:
    A boolean :math:`\mathtt{bit} \in \{0, 1\}`.
  :``dst``:
    Let the initial contents of the buffer be :math:`\mathtt{dst}_0 = \sum a_i 2^i`.
    Then the final contents of the buffer
    :math:`\mathtt{dst}' = \mathtt{dst}_0 + (-a_\mathtt{i} + \mathtt{bit}) 2^\mathtt{i}`.

Lifetime & Ownership
  - ``dst`` is a mutable borrow.

See also
  - `Procedure bn_writeByte`_

Procedure ``bn_writeByte``
~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
  ``bn_writeByte(bn_buffer* dst, size_t i, uint8_t byte)``

Semantics
  Overwrite byte ``i`` of the ``dst`` buffer.
  Bytes are zero-indexed and little-endian.

  :``i``:
    An index in :math:`\mathtt{i} \in \mathbb N`.

    The maximum write-able index is limited by the size of the buffer when it was created.
    Writes to indices above :math:`\mathtt{maxBytes}` are ignored.
  :``byte``:
    A byte :math:`\mathtt{byte} \in mathbb Z / 256\mathbb Z`.
  :``dst``:
    Let the initial contents of the buffer be :math:`\mathtt{dst}_0 = \sum a_i 256^i`.
    Then the final contents of the buffer
    :math:`\mathtt{dst}' = \mathtt{dst}_0 + (-a_\mathtt{i} + \mathtt{byte}) 256^\mathtt{i}`.

Lifetime & Ownership
  - ``dst`` is a mutable borrow.

See also
  - `Procedure bn_writeBit`_


Version Information
-------------------

`\<\<prev <Destructive Operations_>`_
`^up^ <Basic Natural Number API_>`_
`next\>\> <Limits_>`_

We conform to `Semantic Versioning`_.
The API documented here is for version ``0.1.0``.

.. _`Semantic Versioning`: https://semver.org/

Version Macros
~~~~~~~~~~~~~~~~~~~

  - ``BIGLIT_BASIC_MAJOR``: 0
  - ``BIGLIT_BASIC_MINOR``: 1
  - ``BIGLIT_BASIC_PATCH``: 0


Limits
------

`\<\<prev <Version Information_>`_
`^up^ <Basic Natural Number API_>`_

When working with bit-indices, we use ``size_t``.
On "reasonable" modern machines, this limit restricts us to working with numbers that take up to one-eight the addressable memory space.
Frankly, if you are working with numbers this large, you have probably already succumbed to a DoS attack.
Remember, this library is for working with source-code literals, and it's hard to believe that (in the foreseeable future) a source file large enough to store such a number would fit on a single disk.

Likewise, when working with byte-indices, we also use ``size_t``.
On "reasonable" modern machines, this limit restricts us to working with numbers that take up to the entire addressable memory space.
This is already a less restrictive limit than the bit-index limit, and so the same resource utilization analysis applies here.

While the source code is reasonably portable, we do make some assumptions about the target architecture and the operation of your compiler.
We attempt to sanity check this using static asserts.

  - The target has an eight-bit byte: i.e. ``CHAR_BIT == 8``.
  - The target uses exactly one byte to store a ``uint8_t``: i.e. ``sizeof(uint8_t) == 1``.
  - FIXME move elsewhere, where it counts: The target performs arithmetic right shift on ``int16_t`` in two's-complement representation:
    e.g. ``(int16_t)-1 >> 1 == -1``.
