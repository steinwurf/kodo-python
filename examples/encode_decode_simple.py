#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import sys

import kodo


def main():
    """Simple example showing how to encode and decode a block of memory."""
    # Set the number of symbols (i.e. the generation size in RLNC
    # terminology) and the size of a symbol in bytes
    field = kodo.field.binary
    symbols = 8
    symbol_size = 160

    # In the following we will make an encoder/decoder factory.
    # The factories are used to build actual encoders/decoders
    encoder_factory = kodo.RLNCEncoderFactory(field, symbols, symbol_size)
    encoder = encoder_factory.build()

    decoder_factory = kodo.RLNCDecoderFactory(field, symbols, symbol_size)
    decoder = decoder_factory.build()

    # Create some data to encode. In this case we make a buffer
    # with the same size as the encoder's block size (the max.
    # amount a single encoder can encode)
    data_in = bytearray(os.urandom(encoder.block_size()))

    # Assign the data buffer to the encoder so that we can
    # produce encoded symbols
    encoder.set_const_symbols(data_in)

    # Define a data buffer where the symbols should be decoded
    data_out = bytearray(decoder.block_size())
    decoder.set_mutable_symbols(data_out)

    packet_number = 0
    while not decoder.is_complete():
        # Generate an encoded packet
        packet = encoder.write_payload()
        print("Packet {} encoded!".format(packet_number))

        # Pass that packet to the decoder
        decoder.read_payload(packet)
        print("Packet {} decoded!".format(packet_number))
        packet_number += 1
        print("rank: {}/{}".format(decoder.rank(), decoder.symbols()))

    print("Coding finished")

    # The decoder is complete, the decoded symbols are now available in
    # the data_out buffer: check if it matches the data_in buffer
    print("Checking results...")
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unable to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
