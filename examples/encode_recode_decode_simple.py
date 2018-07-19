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

    In Network Coding applications one of the key features is the
    ability of intermediate nodes in the network to recode packets
    as they traverse them. In Kodo it is possible to recode packets
    in decoders which provide the write_payload() function.

    This example shows how to use one encoder and two decoders to
    simulate a simple relay network as shown below (for simplicity
    we have error free links, i.e. no data packets are lost when being
    sent from encoder to decoder1 and decoder1 to decoder2):

            +-----------+     +-----------+     +-----------+
            |  encoder  |+---.| decoder1  |+---.|  decoder2 |
            +-----------+     | (recoder) |     +-----------+
                              +-----------+
    In a practical application recoding can be using in several different
    ways and one must consider several different factors e.g. such as
    reducing linear dependency by coordinating several recoding nodes
    in the network.
    Suggestions for dealing with such issues can be found in current
    research literature (e.g. MORE: A Network Coding Approach to
    Opportunistic Routing).
    """
    # Set the number of symbols (i.e. the generation size in RLNC
    # terminology) and the size of a symbol in bytes
    symbols = 42
    symbol_size = 160

    # In the following we will make an encoder/decoder factory.
    # The factories are used to build actual encoders/decoders
    encoder_factory = kodo.FullVectorEncoderFactoryBinary(
        max_symbols=symbols,
        max_symbol_size=symbol_size)

    encoder = encoder_factory.build()

    decoder_factory = kodo.FullVectorDecoderFactoryBinary(
        max_symbols=symbols,
        max_symbol_size=symbol_size)

    decoder1 = decoder_factory.build()
    decoder2 = decoder_factory.build()

    # Create some data to encode. In this case we make a buffer
    # with the same size as the encoder's block size (the max.
    # amount a single encoder can encode)
    # Just for fun - fill the input data with random data
    data_in = os.urandom(encoder.block_size())

    # Assign the data buffer to the encoder so that we may start
    # to produce encoded symbols from it
    encoder.set_const_symbols(data_in)

    while not decoder2.is_complete():

        # Encode a packet into the payload buffer
        packet = encoder.write_payload()

        # Pass that packet to decoder1
        decoder1.read_payload(packet)

        # Now produce a new recoded packet from the current
        # decoding buffer, and place it into the payload buffer
        packet = decoder1.write_payload()

        # Pass the recoded packet to decoder2
        decoder2.read_payload(packet)

    # Both decoder1 and decoder2 should now be complete,
    # copy the symbols from the decoders

    data_out1 = decoder1.copy_from_symbols()
    data_out2 = decoder2.copy_from_symbols()

    # Check we properly decoded the data
    if data_out1 == data_in and data_out2 == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
