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
import random

import kodo


def main():
    """
    Simple example showing how to encode and decode a block of memory using
    the parity 2d code.
    """

    # Pick the finite field to use for the encoding and decoding.
    field = kodo.FiniteField.binary

    # Create an encoder and decoder.
    encoder = kodo.block.Encoder(field)
    decoder = kodo.block.Decoder(field)

    # Create generator generator.
    generator = kodo.block.generator.Parity2D()

    # Configure generator (rows/columns).
    generator.configure(rows=4, columns=5)

    # The generator's matrix (row and columns) dictates the number of symbols.
    symbols = generator.symbols

    # Pick the size of each symbol in bytes
    symbol_bytes = 1400

    # Configure the coders, the encoder and decoder must be configured
    # identically to be compatible.
    encoder.configure(symbols, symbol_bytes)
    decoder.configure(symbols, symbol_bytes)

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

    # Keep track of the systematic symbols sent
    systematic_index = 0

    # Lose a packet with 10% probability
    loss_probability = 10
    while not decoder.is_complete():

        if generator.can_generate():
            print("coded symbol", end="")

            # Generate the coefficients into the buffer
            index = generator.generate(coefficients)

            # Encode a symbol into the symbol buffer
            encoder.encode_symbol(symbol, coefficients)

            # Lose a packet with based on loss probability
            if random.randint(0, 100) < loss_probability:
                print(" - lost")
            else:
                # In this example the coefficients are available, but in
                # certain use cases - especially network based it might not be
                # feasible to transmit the coefficients. In this case the
                # generator index can be used for generating the coefficients
                # again.
                generator.generate_specific(coefficients, index)
                decoder.decode_symbol(symbol, coefficients)
                print(f" - decoded rank now {decoder.rank}")

        else:
            print("systematic symbol", end="")

            if systematic_index == symbols:
                systematic_index = 0

            # Encode a systematic symbol into the symbol buffer
            encoder.encode_systematic_symbol(symbol, systematic_index)

            # Lose a packet with based on loss probability
            if random.randint(0, 100) < loss_probability:
                print(" - lost")
            else:
                decoder.decode_systematic_symbol(symbol, systematic_index)
            systematic_index += 1

        if generator.can_advance():
            generator.advance()
        else:
            generator.reset()
            systematic_index = 0

    # Check if we properly decoded the data
    if data_in != data_out:
        print("Data was not decoded correctly. Something went wrong")
    else:
        print("Decoding was successful. Yay!")


if __name__ == "__main__":
    main()
