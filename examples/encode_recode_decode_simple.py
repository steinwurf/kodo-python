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
    """
    Encode recode decode example.

    In Network Coding applications, one of the key features is the
    ability of intermediate nodes in the network to recode packets
    as they traverse them. In Kodo it is possible to recode packets
    in decoders which provide the produce_payload() function.

    This example shows how to use one encoder and two decoders to
    simulate a simple relay network as shown below (for simplicity
    we have error free links, i.e. no data packets are lost when being
    sent from encoder to decoder1 and decoder1 to decoder2):

            +-----------+     +-----------+     +-----------+
            |  encoder  |+---.| decoder1  |+---.|  decoder2 |
            +-----------+     | (recoder) |     +-----------+
                              +-----------+
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
    field = kodo.field.binary
    symbols = 42
    symbol_size = 160

    # Create an encoder and two decoders
    encoder = kodo.RLNCEncoder(field, symbols, symbol_size)
    decoder1 = kodo.RLNCDecoder(field, symbols, symbol_size)
    decoder2 = kodo.RLNCDecoder(field, symbols, symbol_size)

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_symbols_storage(data_in)

    # Define the data_out bytearrays where the symbols should be decoded
    # These bytearrays must not go out of scope while the encoder exists!
    data_out1 = bytearray(decoder1.block_size())
    data_out2 = bytearray(decoder1.block_size())
    decoder1.set_symbols_storage(data_out1)
    decoder2.set_symbols_storage(data_out2)

    while not decoder2.is_complete():

        # Encode a packet into the payload buffer
        packet = encoder.produce_payload()

        # Pass that packet to decoder1
        decoder1.consume_payload(packet)

        # Now produce a new recoded packet from the current
        # decoding buffer, and place it into the payload buffer
        packet = decoder1.produce_payload()

        # Pass the recoded packet to decoder2
        decoder2.consume_payload(packet)

    # Both decoder1 and decoder2 should now be complete,
    # check if the output buffers match the data_in buffer
    if data_out1 == data_in and data_out2 == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
