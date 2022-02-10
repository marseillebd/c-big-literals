Basic API Versioning
====================

`^up^ <Big Literals C Library_>`_
`next\>\> <Basic API Limits_>`_

We conform to `Semantic Versioning`_.
The API documented here is for version ``0.1.0``.

.. _`Semantic Versioning`: https://semver.org/

Version Macros
~~~~~~~~~~~~~~~~~~~

  - ``BIGLIT_BASIC_MAJOR``: 0
  - ``BIGLIT_BASIC_MINOR``: 1
  - ``BIGLIT_BASIC_PATCH``: 0


Basic API Limits
================

`\<\<prev <Basic API Versioning_>`_
`^up^ <Big Literals C Library_>`_

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
