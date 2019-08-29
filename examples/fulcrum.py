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
    Fulcrum example.

    This Example shows how to use the additional settings and parameters
    supported by the fulcrum code.
    """
    if not hasattr(kodo, 'FulcrumEncoder'):
        print("The fulcrum codec is not available")
        return

    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 24
    symbol_size = 160

    # Create a fulcrum encoder and decoder
    encoder = kodo.FulcrumEncoder(field, symbols, symbol_size)
    decoder = kodo.FulcrumDecoder(field, symbols, symbol_size)

    # The expansion denotes the number of additional symbols created by
    # the outer code
    print(
        "The created coders now have the following settings.\n"
        "Encoder:\n"
        "\tSymbols: {}\n"
        "\tExpansion: {}\n"
        "\tInner symbols: {}\n"
        "Decoder:\n"
        "\tSymbols: {}\n"
        "\tExpansion: {}\n"
        "\tInner symbols: {}".format(
            encoder.symbols(),
            encoder.expansion(),
            encoder.inner_symbols(),
            decoder.symbols(),
            decoder.expansion(),
            decoder.inner_symbols()))

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_symbols_storage(data_in)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_symbols_storage(data_out)

    while not decoder.is_complete():
        # Encode a packet into the payload buffer
        payload = encoder.produce_payload()

        # Pass that packet to the decoder
        decoder.consume_payload(payload)

    # The decoder is complete, the decoded symbols are now available in
    # the data_out buffer: check if it matches the data_in buffer
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
