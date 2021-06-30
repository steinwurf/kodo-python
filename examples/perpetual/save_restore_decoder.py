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
import sys
import random

import kodo


def decoder_state_filename(files: int):
    return "decoder.state." + str(files)


def decoder_state_files():
    files = 0
    while os.path.exists(decoder_state_filename(files + 1)):
        files += 1
    return files


def main():
    """
    Simple save/restore example showing how to save and restore the
    decoders state.
    The decoder processes a single symbol and then saves its state
    until completion it has processed enough symbols to complete.
    """

    if "--dry-run" in sys.argv:
        sys.exit(0)

    # Pick the finite field to use for the encoding and decoding.
    field = kodo.FiniteField.binary8

    # Create an encoder.
    encoder = kodo.perpetual.Encoder(field)

    # Pick the number of bytes to encode/decode.
    block_bytes = 1000000

    # Pick the size of each symbol in bytes
    symbol_bytes = 1400

    # Pick the "width" of the resulting coded symbols.
    # The higher the width, the greater chanes are that each symbol is useful
    # for the decoder, but it will also increase the complexity of both the
    # encoding and decoding.
    width = 100

    # Configure the encoder.
    encoder.configure(block_bytes, symbol_bytes, width)

    # Create generator and offset generator.
    generator = kodo.perpetual.generator.RandomUniform(field)
    offset_generator = kodo.perpetual.offset.RandomUniform()

    generator.configure(encoder.symbols, width)
    offset_generator.configure(encoder.symbols, width)

    seed = random.randrange(60000)
    generator.set_seed(seed)
    offset_generator.set_seed(seed)

    # Allocate some storage for a symbol.
    symbol = bytearray(encoder.symbol_bytes)

    # Allocate some storage for the coefficients.
    coefficients = bytearray(generator.max_coefficients_bytes)

    # If it doesn't exist already, allocate some data to encode.
    # In this case we make a buffer with the same size as the
    # encoder's block size(the max. amount a single encoder can
    # encode)
    if os.path.exists("data.in"):
        with open("data.in", "rb") as data_in_file:
            data_in = bytearray(data_in_file.read())
    else:
        # Just for fun - fill data_in with random data
        data_in = bytearray(os.urandom(encoder.block_bytes))
        with open("data.in", "wb+") as data_in_file:
            data_in_file.write(data_in)

    # Assign the data buffer to the encoder so that we may start
    # to produce encoded symbols from it
    encoder.set_symbols_storage(data_in)

    # Define the decoder.
    decoder = None

    # Check if we should restore a previous state
    state_files = decoder_state_files()
    if state_files != 0:
        with open(decoder_state_filename(state_files), "rb") as decoder_state_file:
            decoder = kodo.perpetual.Decoder.restore_state(
                bytearray(decoder_state_file.read())
            )
    else:
        # No previous state to restore, create and configure decoder
        decoder = kodo.perpetual.Decoder(field)
        decoder.configure(block_bytes, symbol_bytes, width)
    assert decoder != None

    # Restore output file
    data_out = bytearray(decoder.block_bytes)
    if os.path.exists("data.out"):
        f = open("data.out", "rb")
        data_out = bytearray(f.read())
        f.close()

    decoder.set_symbols_storage(data_out)

    # Record the current rank
    old_rank = decoder.rank

    # Generate the offset
    offset = offset_generator.offset()

    # Setup the coefficient generator
    generator.set_offset(offset)

    # Generate the coefficients into the coefficients buffer
    generator.generate(coefficients)

    # Encode a symbol into the symbol buffer
    encoder.encode_symbol(symbol, coefficients, offset)

    # Pass the symbol, coefficients, and offset to the decoder
    decoder.decode_symbol(symbol, coefficients, offset)

    if decoder.can_complete_decoding():
        decoder.complete_decoding()

    if decoder.is_complete():
        if data_in == data_out:
            print("Data decoded correctly")
        else:
            print("Unexpected failure to decode,\nplease file a bug report :)")
    else:
        print(f"Rank: {decoder.rank}/{decoder.symbols}", end="")

        if decoder.rank == old_rank:
            print(" packet linear dependent!")
            return 0
        else:
            print("!")

        # Save the state
        state_data = bytearray(decoder.state_bytes())
        decoder.save_state(state_data)
        with open(decoder_state_filename(state_files + 1), "xb") as decoder_state_file:
            decoder_state_file.write(state_data)

        with open("data.out", "wb") as data_out_file:
            data_out_file.write(data_out)


if __name__ == "__main__":
    main()
