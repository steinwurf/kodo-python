#!/usr/bin/env python
# encoding: utf-8

# License for Commercial Usage
# Distributed under the "KODO EVALUATION LICENSE 1.3"
# Licensees holding a valid commercial license may use this project in
# accordance with the standard license agreement terms provided with the
# Software (see accompanying file LICENSE.rst or
# https:#www.steinwurf.com/license), unless otherwise different terms and
# conditions are agreed in writing between Licensee and Steinwurf ApS in which
# case the license will be regulated by that separate written agreement.
# License for Non-Commercial Usage
# Distributed under the "KODO RESEARCH LICENSE 1.2"
# Licensees holding a valid research license may use this project in accordance
# with the license agreement terms provided with the Software
# See accompanying file LICENSE.rst or https:#www.steinwurf.com/license

import os
import sys
import random

import kodo


def main():
    """
    Encode recode decode example.

    In Network Coding applications, one of the key features is the
    ability of intermediate nodes in the network to recode packets
    as they traverse them. In Kodo it is possible to recode packets
    in decoders which provide the produce_payload() function.

    This example shows how to use one encoder and two decoders to
    simulate a simple relay network as shown below:

            +-----------+     +-----------+     +-----------+
            |  encoder  |+--->|  recoder  |+--->|  decoder  |
            +-----------+     +-----------+     +-----------+

    In a practical application recoding can be used in several different
    ways and one must consider several different factors, such as
    reducing linear dependency by coordinating several recoding nodes
    in the network.
    Suggestions for dealing with such issues can be found in current
    research literature (e.g. MORE: A Network Coding Approach to
    Opportunistic Routing).
    """
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.FiniteField.binary8
    symbols = 42
    symbol_bytes = 160

    # Create an encoder and two decoders
    encoder = kodo.block.Encoder(field)
    recoder = kodo.block.Decoder(field)
    decoder = kodo.block.Decoder(field)

    # Create generator
    generator = kodo.block.generator.RandomUniform(field)

    # Configure
    encoder.configure(symbols, symbol_bytes)
    recoder.configure(symbols, symbol_bytes)
    decoder.configure(symbols, symbol_bytes)
    generator.configure(symbols)

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_bytes))
    encoder.set_symbols_storage(data_in)

    # Define the data_out bytearrays where the symbols should be decoded
    # These bytearrays must not go out of scope while the encoder exists!
    data_out1 = bytearray(recoder.block_bytes)
    data_out2 = bytearray(decoder.block_bytes)
    recoder.set_symbols_storage(data_out1)
    decoder.set_symbols_storage(data_out2)

    symbol = bytearray(encoder.symbol_bytes)
    coefficients = bytearray(generator.max_coefficients_bytes)
    recoding_coefficients = bytearray(generator.max_coefficients_bytes)

    loss_probability = 25
    systematic_index = 0

    while not decoder.is_complete():

        if encoder.rank > systematic_index:
            print("systematic symbol", end="")

            index = systematic_index
            systematic_index += 1
            encoder.encode_systematic_symbol(symbol, index)

            # Drop packet based on loss probability
            if random.randint(0, 100) < loss_probability:
                print(" - lost")
            else:
                recoder.decode_systematic_symbol(symbol, index)
                print(f" - decoded, rank now {recoder.rank}")
        elif not recoder.is_complete():

            # The RLNC code is rateless, which means, in theory, that it can
            # generate an infinite number of coded packets.
            print("coded symbol", end="")

            # Generate the coefficients into the symbol buffer
            generator.generate(coefficients)

            # Encode a symbol into the symbol buffer
            encoder.encode_symbol(symbol, coefficients)

            # Drop packet based on loss probability
            if random.randint(0, 100) < loss_probability:
                print(" - lost")
            else:
                # Note, in this example the coefficients are available, but in
                # certain use cases - especially network based it might not be
                # feasible to transmit the coefficients. In this case the seed
                # can be used for generating the coefficients again.
                recoder.decode_symbol(symbol, coefficients)
                print(f" - decoded, rank now {recoder.rank}")

        if recoder.rank != 0:
            print("recoded symbol", end="")
            generator.generate_recode(recoding_coefficients, recoder)
            recoder.recode_symbol(symbol, coefficients, recoding_coefficients)

            if random.randint(0, 100) < loss_probability:
                print(" - lost")
            else:
                # When recoding sending the whole coefficient vector is often
                # needed as the state of the recorder determines the
                # coefficients.
                decoder.decode_symbol(symbol, coefficients)
                print(f" - decoded, rank now {decoder.rank}")

    # Both recoder and decoder should now be complete,
    # check if the output buffers match the data_in buffer
    if data_out1 == data_in and data_out2 == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)


if __name__ == "__main__":
    main()
