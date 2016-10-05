News for kodo-python
====================

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* Major: Update kodo-core, kodo-rlnc, and kodo-fulcrum to version 6, 5, and 5
         respectively.
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
