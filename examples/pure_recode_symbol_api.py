#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2018.
# Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import sys

import kodo


def main():
    """
    Pure recode example using the Symbol API.

    This example is very similar to pure_recode_payload_api.py.
    The difference is that this example uses the low-level Symbol API of the
    pure recoder instead of the high-level Payload API.
    Similarly, the pure recoder will not decode the incoming data, it will
    only re-encode it and we use the same relay network as shown below.
    For simplicity, we have error free links, i.e. no data packets are lost
    when being sent from encoder to recoder to decoder:

            +-----------+      +-----------+      +------------+
            |  encoder  |+---->| recoder   |+---->|  decoder   |
            +-----------+      +-----------+      +------------+

    The pure recoder does not need to decode all symbols, and its "capacity"
    for storing symbols can be limited. These stored coded symbols are
    linear combinations of previously received symbols. When a new symbol is
    received, the pure recoder will combine it with its existing symbols
    using random coefficients.
    The pure recoder can produce a random linear combination of the stored
    coded symbols, which can be processed by a regular decoder.
    """
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 5
    symbol_size = 160

    # Create an encoder/decoder factory that are used to build the
    # actual encoders/decoders
    encoder_factory = kodo.RLNCEncoderFactory(field, symbols, symbol_size)
    encoder = encoder_factory.build()

    recoder_factory = kodo.RLNCPureRecoderFactory(field, symbols, symbol_size)
    # Set the pure recoder "capacity" to be less than "symbols"
    recoder_factory.set_recoder_symbols(3)
    recoder = recoder_factory.build()

    print("Recoder properties:\n"
          "  Symbols: {}\n"
          "  Recoder symbols: {}".format(
            recoder.symbols(),
            recoder.recoder_symbols()))

    decoder_factory = kodo.RLNCDecoderFactory(field, symbols, symbol_size)
    decoder = decoder_factory.build()

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_const_symbols(data_in)

    # Define the data_out bytearrays where the symbols should be decoded
    # These bytearrays must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_mutable_symbols(data_out)

    while not decoder.is_complete():
        # Generate some random coefficients for encoding
        coefficients = encoder.generate()
        # We can print the individual coefficients here, because we use the
        # binary8 field where each byte corresponds to a single coefficient.
        # For other fields, we would need fifi-python to do this!
        print("Encoding coefficients:")
        print(" ".join(str(c) for c in coefficients))
        # Write a coded symbol to the symbol buffer
        symbol = encoder.write_symbol(coefficients)

        print("Encoded symbol generated and passed to the recoder")
        # Pass that symbol and the corresponding coefficients to the recoder
        recoder.read_symbol(symbol, coefficients)

        # Generate some random coefficients for recoding
        recoding_coefficients = recoder.recoder_generate()

        print("Recoding coefficients:")
        print(" ".join(str(c) for c in recoding_coefficients))

        # Produce an encoded symbol based on the recoding coefficients
        # Note that the resulting recoded_symbol_coefficients will not be
        # the same as the recoding_coefficients
        recoded_symbol, recoded_symbol_coefficients = \
            recoder.recoder_write_symbol(recoding_coefficients)

        print("Recoded symbol coefficients:")
        print(" ".join(str(c) for c in recoded_symbol_coefficients))

        print("Recoded symbol generated and passed to the decoder")

        # Pass that symbol and the corresponding coefficients to the decoder
        decoder.read_symbol(recoded_symbol, recoded_symbol_coefficients)

        print("Decoder rank: {}/{}\n".format(decoder.rank(), symbols))

    # Check if we properly decoded the data
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
