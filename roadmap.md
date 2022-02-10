# Big Literal Roadmap

## v0.2.0

  * before compiling, accumulate headers
  * documentation
    - [ ] core ℕ
    - [ ] core ℤ
  * have gained some use in bsvm
  * float interface (storage only)
  * more in the basic ℕ, ℤ interfaces
    - [ ] abs bz -> bz
    - [ ] min, max

## v0.3.0

  * ℚ interface
  * ltz, gtz, eqz

## v1.0.0

Who knows?

 * Maybe I'll add arithmetic to float.
 * Perhaps a two's-complement bit/byte interface for ℤ

## In Other Projects

Don't try to work with tagged pointers here (except possibly in bz to pack in a sign bit?)
I'll implement compression of small integers into tagged pointers in a bsrts library just before doing bsvm.

[INT36-C](https://wiki.sei.cmu.edu/confluence/display/c/INT36-C.+Converting+a+pointer+to+integer+or+integer+to+pointer) has some good information about standards-compliant was to do tagged pointers. In particular, I can just cast back-and-forth between pointers and `intptr_t`/`uintptr_t`.