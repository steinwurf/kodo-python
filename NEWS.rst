News for kodo-python
====================

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* tbd
* Minor: Added the following functions to provide access to the low-level
  Symbol API of the coder objects:
    - encoder.generate()
    - encoder.write_symbol()
    - encoder.write_uncoded_symbol(index)
    - decoder.read_symbol(symbol_data, coefficients)
    - decoder.read_uncoded_symbol(symbol_data, index)
    - recoder.recoder_generate()
    - recoder.read_symbol(symbol_data, coefficients)
    - recoder.recoder_write_symbol(coefficients)
* Minor: Created or extended the following examples to cover the usage
  of the Symbol API and compare it to the Payload API:
    - encode_decode_using_coefficients
    - pure_recode_payload_api
    - pure_recode_symbol_api
* Major: Use pybind11 instead of boost-python.

13.1.1
------
* Patch: Fixed Python header warnings on certain Linux platforms.

13.1.0
------
* Minor: Added the pure recoder from kodo-rlnc. The coder can be instantiated
  using the kodo.RLNCPureRecoderFactory. The encode_pure_recode_decode_simple
  example shows how to change the recoder_symbols parameter.

13.0.0
------
* Major: Changed all coders to work with shallow storage, so the user needs to
  create a bytearray object where the encoder/decoder can store the symbols.
  Make sure to call set_const_symbols() before using an encoder and
  set_mutable_symbols() before using a decoder!
  Consequently, the copy_from_symbols() function was removed, since the
  decoded symbols are available without copying in the user-specified buffer.
* Major: The write_payload() function returns a Python bytearray and the
  read_payload() function only accepts a bytearray as input. Therefore no
  copying or conversion will happen between the Python and C++ sides.
* Major: The finite field is specified as the first parameter of the factory
  constructor (e.g. kodo.field.binary8).
* Major: Upgrade to kodo-core 20
* Major: Upgrade to kodo-rlnc 13
* Major: Upgrade to kodo-fulcrum 12
* Major: Depend on kodo-perpetual 2
* Minor: Updated the license to STEINWURF EVALUATION LICENSE 1.0.

12.0.0
------
* Major: Upgrade to kodo-core 9
* Major: Upgrade to kodo-rlnc 7
* Major: Upgrade to kodo-fulcrum 7

11.0.1
------
* Patch: The unit tests should only run if this is the top-level project.

11.0.0
------
* Major: Upgrade to waf-tools 4
* Major: Upgrade to boost 3
* Major: Upgrade to kodo-core 8
* Major: Upgrade to kodo-rlnc 6
* Major: Upgrade to kodo-fulcrum 6
* Minor: Added decoder functions: ``copy_from_symbol``, ``is_symbol_missing``,
  ``is_symbol_partially_decoded``, ``is_symbol_uncoded``, ``symbols_missing``,
  ``symbols_partially_decoded``.
* Patch: Updated documentation.

10.0.0
------
* Major: Depend on kodo-core, kodo-rlnc and kodo-fulcrum instead of kodo
  (kodo has been split to multiple repositories).
* Minor: Added the ``enable_codecs`` option to only enable specific codecs
  when configuring kodo-python. This option is useful to build a smaller
  library.

9.0.1
-----
* Patch: Moved the check for the BUILD_PYTHON variable to the build step.

9.0.0
-----
* Major: Upgrade to waf-tools 3
* Minor: Upgrade to boost 2
* Major: Upgrade to kodo 32
* Major: Renamed ``set_symbol`` to ``set_const_symbol``.
* Major: Renamed ``set_symbols`` to ``set_const_symbols``.

8.0.2
-----
* Patch: Only execute the unit tests within the current project.

8.0.1
-----
* Patch: Expose fulcrum stack.

8.0.0
-----
* Minor: Added fulcrum stack.
* Minor: Added visualization of the encoding state.
* Major: Update kodo to version 30.
* Major: renamed ``copy_symbols`` to ``copy_from_symbols``.
* Major: Update kodo to version 29.
* Major: Added dependency for meta.
* Major: Update kodo to version 28.
* Minor: Added ``in_systematic_phase`` member function to systematic encoders.
* Minor: Added a simple benchmark example in ``examples\benchmark.py`` to
  measure the encoding and decoding throughput with the Python API.
* Major: Removed the codec types where the tracing functionality was disabled.
  Tracing is now available in all codecs, but it is not activated by default.
  This change can significantly lower the RAM usage during the compilation.
* Minor: Restructured the library so that different codec types are
  instantiated in separate cpp files. The optimizer might use
  gigabytes of RAM when instantiating a lot of codec stack variants in a
  single cpp file. This problem is mitigated with this separation.
* Major: Upgrade to kodo 26.

7.0.0
-----
* Minor: Added perpetual codes + example.
* Major: Update kodo to version 25.
* Major: Update fifi to version 19.
* Major: Rename ``encode``, ``decode``, and ``recode`` to ``write_payload``,
  ``read_payload``, and ``write_payload``, respectively.

6.0.2
-----
* Patch: Updated README to reflect new licensing requirements.
* Patch: Update waf.

6.0.1
-----
* Patch: Fix version function.

6.0.0
-----
* Minor: Add version attribute to kodo python module.
* Major: Update kodo to version 22.
* Major: Update sak to version 14.
* Major: Update fifi to version 17.
* Minor: Added ``no_code`` algorithm.
* Major: Removed decoder methods: ``is_symbol_uncoded``, ``decode_symbol`` and
  ``decode_symbol_at_index``.

5.0.0
-----
* Minor: Added ``symbols`` and ``symbol_size`` methods to factory.
* Minor: Added graphical Lena example.
* Major: Update kodo to version 20.
* Major: Update sak to version 13.
* Major: Update fifi to version 15.
* Major: Added recycle as a dependency.
* Major: Rename classes to follow the new naming scheme of kodo.
* Major: Use proper python naming style for classes. E.g., the
  ``full_vector_encoder_binary``is now called ``FullVectorEncoderBinary``.
* Minor: Added graphical print_coefficients example.
* Minor: Added a more simple kodo python API, ``pykodo``.
* Minor: Collected example helper logic for graphical exemplification and put
  it in ``kodo_helper`` module.

4.0.0
-----
* Major: Upgrade to Fifi 14.
* Major: Upgrade to Kodo 19.
* Minor: Added ``sparse_full_rlnc_encoder``.
* Minor: Added guide on how to extend the bindings.
* Patch: Fix the docstrings to follow the pep257 style guide.
* Patch: Added keyword argument for the ``is_symbol_pivot`` method.
* Minor: Added UDP unicast example.

3.0.0
-----
* Minor: Added multicast examples.
* Major: Python objects now only provide the functions they support. E.g., a
  non-trace encoder no longer has the trace function. Also the ``has_``
  functions were removed. This includes: ``has_partial_decoding_tracker``,
  ``has_systematic_encoder``, and ``has_trace``.
* Patch: Simplified examples.
* Major: Updated Kodo 18.
* Major: Updated Fifi 13.
* Major: Updated Sak 12.

2.2.0
-----
* Minor: Added documentation for the python functions.
* Minor: Added keyword arguments for the python functions.
* Patch: Removed unused dependencies guage and tables.

2.1.0
-----
* Minor: Set ``kodo-python`` as the name for wscript target, so that it doesn't
  clash with the kodo dependency when both are used as dependencies.

2.0.0
-----
* Major: Updated to Kodo 17.
* Minor: Extended API.
* Minor: Added additional examples.

1.0.0
-----
* Initial release.
