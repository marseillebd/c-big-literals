Big Literals C Library
======================

This library is meant to provide facilities for compiler which may need to manipulate numbers that do not fit into a machine word, or otherwise have unusual representation formats.
The primary design goals are correctness of implementation, generality of representations and algorithms, and ease of integration into existing or planned software projects.
It is expected that the algorithms in this library will be used on reasonably small numbers[#]_, and will only be used for numerical computations only rarely[#]_.

.. [#] such as what a human might be able to correctly transcribe into program source code
.. [#] Most computations should be in service to just reading and writing formatted number literals, hence the name.

These goals emphasize favor simplicity far above performance.
There are plentiful mature, high-performance bignum libraries available for use in cryptography and number theory.
This library is just about making sure compilers don't alter the meaning of my code by accidentally truncating my literals.

The library is implemented in C not for speed, but because C is highly portable both to different machines, and to other languages through an FFI[#]_.

.. [#] Even Javascript can call C nowadays, thanks to WASM.

Interface Documentation
-----------------------

Building
  Clone the git repository from <https://github.com/edemko/biglit>.
  Inside the directory is a ``./BUILD.sh`` script.
  By default it uses a recent gcc to craete libraries and pandoc to create documentation.

  If your system differs from the default parameters of they script, try editong those parameters
  (they are just some variables assigned near the top of the script).

Usage
  TODO header files

  TODO a single static library (or you can create one youself if you only need some of the functionality)

  Once the build script complete, a number of object files will be left in ``build``.
  You will need at least the ``build/core`` object files for the number systems relevant to you,
  as well as the ``build/basic`` object files (for the relevant number systems) if you use that interface.

  Just statically link these object files with your code.


Basic Interfaces
  The basic interfaces are the easiest way to interact with this library.

  - `Basic Natural Number API <basic-bn.html>`_
  - `Basic Integer API <basic-bz.html>`_

Core Interfaces
  The core interfaces allow access to the minimal functionality and can be used to build additional interfaces.
  They perform little-to-no saftey checks, or memory management,
  which is not very suitable for library implementors, not library users.
  
  - Core Natural Number API