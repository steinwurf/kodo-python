#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2016.
# Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import sys
import copy

import kodo


def main():
    """Example showing the result of enabling the symbol status updater."""
    # Set the number of symbols (i.e. the generation size in RLNC
    # terminology) and the size of a symbol in bytes
    symbols = 50
    symbol_size = 160

    # In the following we will make an encoder/decoder factory.
    # The factories are used to build actual encoders/decoders
    # To show the effect of the symbol status updater we need to use a lower
    # sized field - the lower the better.
    encoder_factory = kodo.FullVectorEncoderFactoryBinary(symbols, symbol_size)
    encoder = encoder_factory.build()

    decoder_factory = kodo.FullVectorDecoderFactoryBinary(symbols, symbol_size)
    # Create two decoders, one which has the status updater turned on, and one
    # which has it off.
    decoder1 = decoder_factory.build()
    decoder2 = decoder_factory.build()

    decoder2.set_status_updater_on()

    print("decoder 1 status updater: {}".format(
        decoder1.is_status_updater_enabled()))
    print("decoder 2 status updater: {}".format(
        decoder2.is_status_updater_enabled()))

    # Create some data to encode. In this case we make a buffer
    # with the same size as the encoder's block size (the max.
    # amount a single encoder can encode)
    # Just for fun - fill the input data with random data
    data_in = os.urandom(encoder.block_size())

    # Assign the data buffer to the encoder so that we can
    # produce encoded symbols
    encoder.set_const_symbols(data_in)

    # Skip the systematic phase as the effect of the symbol status decoder is
    # only visible when reading coded packets.
    encoder.set_systematic_off()

    print("Processing")
    while not decoder1.is_complete():
        # Generate an encoded packet
        payload = encoder.write_payload()

        payload_copy = copy.copy(payload)
        # Pass that packet to the decoder
        decoder1.read_payload(payload)
        decoder2.read_payload(payload_copy)
        print("decoder 1: {}".format(decoder1.symbols_uncoded()))
        print("decoder 2: {}".format(decoder2.symbols_uncoded()))
        print("-----------------")

    print("Processing finished")

    # The decoder is complete, now copy the symbols from the decoder
    data_out1 = decoder1.copy_from_symbols()
    data_out2 = decoder2.copy_from_symbols()

    # Check if we properly decoded the data
    print("Checking results")
    if data_out1 == data_in and data_out2 == data_in:
        print("Data decoded correctly")
    else:
        print("Unable to decode please file a bug report :)")
        sys.exit(1)


if __name__ == "__main__":
    main()
