#!/usr/bin/env python
# encoding: utf-8

# License for Commercial Usage
# Distributed under the "KODO EVALUATION LICENSE 1.3"
# Licensees holding a valid commercial license may use this project in
# accordance with the standard license agreement terms provided with the
# Software (see accompanying file LICENSE.rst or
# https://www.steinwurf.com/license), unless otherwise different terms and
# conditions are agreed in writing between Licensee and Steinwurf ApS in which
# case the license will be regulated by that separate written agreement.
# License for Non-Commercial Usage
# Distributed under the "KODO RESEARCH LICENSE 1.2"
# Licensees holding a valid research license may use this project in accordance
# with the license agreement terms provided with the Software
# See accompanying file LICENSE.rst or https://www.steinwurf.com/license

import os

import kodo


def main():
    """
    Simple example showing how to encode and decode a block of memory
    using perpetual coders
    """

    # Pick the finite field to use for the encoding and decoding.
    field = kodo.FiniteField.binary8

    # Create an encoder and decoder.
    encoder = kodo.perpetual.Encoder(field)
    decoder = kodo.perpetual.Decoder(field)

    # Pick the size of each symbol in bytes
    symbol_bytes = 1400

    # Pick the number of bytes to encode/decode.
    block_bytes = 1000000

    # Pick the "width" of the resulting coded symbols.
    # The higher the width, the greater chances are that each symbol is useful
    # for the decoder, but it will also increase the complexity of both the
    # encoding and decoding.
    width = 100

    # Configure the coders, the encoder and decoder must be configured
    # identically to be compatible.
    encoder.configure(block_bytes, symbol_bytes, width)
    decoder.configure(block_bytes, symbol_bytes, width)

    # Create generator and offset generator.
    generator = kodo.perpetual.generator.RandomUniform(field)
    offset_generator = kodo.perpetual.offset.SequentialSequence()

    # Configure generator and offset generator.
    # The generators must similarly be configured based on the encoder/decoder.
    generator.configure(encoder.symbols, encoder.width)
    offset_generator.configure(encoder.symbols, encoder.width)

    # Allocate some storage for a symbol.
    symbol = bytearray(encoder.symbol_bytes)

    # Allocate some storage for the coefficients.
    coefficients = bytearray(generator.max_coefficients_bytes)

    # Allocate some data to encode. In this case we make a buffer
    # with the same size as the encoder's block size (the max.
    # amount a single encoder can encode)
    # Just for fun - fill data_in with random data
    data_in = bytearray(os.urandom(encoder.block_bytes))

    # Assign the data buffer to the encoder so that we may start
    # to produce encoded symbols from it
    encoder.set_symbols_storage(data_in)

    # Define a data buffer where the symbols should be decoded
    data_out = bytearray(decoder.block_bytes)
    decoder.set_symbols_storage(data_out)

    while not decoder.is_complete():
        # Generate the offset
        offset = offset_generator.offset()

        # Setup the coefficient generator
        generator.set_offset(offset)
        # Generate the coefficients into the symbol buffer
        generator.generate(coefficients)

        # Encode a symbol into the symbol buffer
        encoder.encode_symbol(symbol, coefficients, offset)

        # Note; if this were two septate applications communicating over a
        # network the encoder and decoder would need to exchange some
        # information.
        # Besides the obvious symbol data, the encoder would also need to
        # share the coefficients and offset
        # (the state for the decoder to generate the information on it's own).

        # Pass the symbol, coefficients, and offset to the decoder
        decoder.decode_symbol(symbol, coefficients, offset)

        if decoder.can_complete_decoding():
            decoder.complete_decoding()

    # Check if we properly decoded the data
    if data_in != data_out:
        print("Something went wrong. Decoding was unsuccessful.")

    else:
        print("Decoding was successful. Yay!")


if __name__ == "__main__":
    main()
