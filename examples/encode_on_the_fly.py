#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import random
import sys

import kodo


def main():
    """
    Encode on the fly example.

    This example shows how to use a storage aware encoder which will
    allow you to encode from a block before all symbols have been
    specified. This can be useful in cases where the symbols that
    should be encoded are produced on-the-fly.
    """
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary
    symbols = 10
    symbol_size = 160

    # Create an encoder and a decoder
    encoder = kodo.RLNCEncoder(field, symbols, symbol_size)
    decoder = kodo.RLNCDecoder(field, symbols, symbol_size)

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size
    data_in = bytearray(os.urandom(encoder.block_size()))

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_symbols_storage(data_out)

    # Let's split the data into symbols and feed the encoder one symbol at a
    # time
    symbol_storage = [
        data_in[i:i + symbol_size] for i in range(0, len(data_in), symbol_size)
    ]

    while not decoder.is_complete():

        # Randomly choose to insert a symbol
        if random.choice([True, False]) and encoder.rank() < symbols:
            # For an encoder the rank specifies the number of symbols
            # it has available for encoding
            rank = encoder.rank()
            encoder.set_symbol_storage(symbol_storage[rank], rank)
            print("Symbol {} added to the encoder".format(rank))

        # Encode a packet into the payload buffer
        packet = encoder.produce_payload()
        print("Packet encoded")

        # Send the data to the decoders, here we just for fun
        # simulate that we are loosing 50% of the packets
        if random.choice([True, False]):
            print("Packet dropped on channel")
            continue

        # Packet got through - pass that packet to the decoder
        decoder.consume_payload(packet)
        print("Decoder received packet")
        print("Encoder rank = {}".format(encoder.rank()))
        print("Decoder rank = {}".format(decoder.rank()))
        decoded_symbol_indces = []
        for i in range(decoder.symbols()):
            if decoder.is_symbol_decoded(i):
                decoded_symbol_indces.append(str(i))

        print("Decoder decoded = {} ({}) symbols".format(
            decoder.symbols_decoded(),
            " ".join(decoded_symbol_indces)))
        print("Decoder partially decoded = {}".format(
            decoder.symbols_partially_decoded()))

    print("Coding finished")

    # The decoder is complete, the decoded symbols are now available in
    # the data_out buffer: check if it matches the data_in buffer
    print("Checking results...")
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
