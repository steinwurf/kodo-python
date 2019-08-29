#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2016.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import sys
import copy

import kodo


def main():
    """Example showing the result of enabling the symbol status updater."""
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary
    symbols = 50
    symbol_size = 160

    # Create an encoder
    encoder = kodo.RLNCEncoder(field, symbols, symbol_size)

    # Create two decoders, one which has the status updater turned on, and one
    # which has it off.
    decoder1 = kodo.RLNCDecoder(field, symbols, symbol_size)
    decoder2 = kodo.RLNCDecoder(field, symbols, symbol_size)

    decoder2.set_status_updater_on()

    print("decoder 1 status updater: {}".format(
        decoder1.is_status_updater_enabled()))
    print("decoder 2 status updater: {}".format(
        decoder2.is_status_updater_enabled()))

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_symbols_storage(data_in)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out1 = bytearray(decoder1.block_size())
    data_out2 = bytearray(decoder1.block_size())
    decoder1.set_symbols_storage(data_out1)
    decoder2.set_symbols_storage(data_out2)

    # Skip the systematic phase as the effect of the symbol status decoder is
    # only visible when reading coded packets.
    encoder.set_systematic_off()

    print("Processing")
    while not decoder1.is_complete():
        # Generate an encoded packet
        payload = encoder.produce_payload()
        payload_copy = copy.copy(payload)

        # Pass that packet to the decoder
        decoder1.consume_payload(payload)
        decoder2.consume_payload(payload_copy)
        print("decoder 1: {}".format(decoder1.symbols_decoded()))
        print("decoder 2: {}".format(decoder2.symbols_decoded()))
        print("-----------------")

    print("Processing finished")

    # Check if both decoders properly decoded the original data
    print("Checking results")
    if data_out1 == data_in and data_out2 == data_in:
        print("Data decoded correctly")
    else:
        print("Unable to decode please file a bug report :)")
        sys.exit(1)


if __name__ == "__main__":
    main()
