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
    Encode-decode using coefficients example.

    This example shows how to use the Symbol API with direct coefficient
    access. Using this approach the we have full control over where
    coefficients are stored, however we also have to manage things such as
    systematic symbols ourselves.
    """
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 5
    symbol_size = 160

    # Create an encoder and a decoder
    encoder = kodo.RLNCEncoder(field, symbols, symbol_size)
    decoder = kodo.RLNCDecoder(field, symbols, symbol_size)

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_symbols_storage(data_in)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_symbols_storage(data_out)

    # Define a custom trace function for the decoder which filters the
    # trace message based on their zones
    def callback_function(zone, message):
        if zone in ["decoder_state", "symbol_coefficients_before_consume_symbol"]:
            print("{}:".format(zone))
            print(message)
    # We want to follow the decoding process step-by-step
    decoder.set_log_callback(callback_function)

    # In the first phase, we will transfer some systematic symbols from
    # the encoder to the decoder.
    # Randomly select 2 symbols from the 5 original symbols
    for index in sorted(random.sample(range(symbols), 2)):
        # Get the original symbol from the encoder
        symbol = encoder.produce_systematic_symbol(index)
        # Insert the symbol to the decoder using the raw symbol data,
        # no additional headers or coefficients are needed for this
        print("Adding Systematic Symbol {}:\n".format(index))
        decoder.consume_systematic_symbol(symbol, index)

    # In the second phase, we will generate coded symbols to fill in the gaps
    # and complete the decoding process
    packet_number = 0
    while not decoder.is_complete():
        # Generate some random coefficients for encoding
        coefficients = encoder.generate()
        # We can print the individual coefficients here, because we use the
        # binary8 field where each byte corresponds to a single coefficient.
        # For other fields, we would need fifi-python to do this!
        print("Coding coefficients:")
        print(" ".join(str(c) for c in coefficients))
        # Write a coded symbol to the symbol buffer
        symbol = encoder.produce_symbol(coefficients)

        print("Coded Symbol {} encoded!\n".format(packet_number))

        # Pass that symbol and the corresponding coefficients to the decoder
        print("Processing Coded Symbol {}:\n".format(packet_number))
        decoder.consume_symbol(symbol, coefficients)

        packet_number += 1
        print("Decoder rank: {}/{}\n".format(decoder.rank(), symbols))

    print("Coding finished")

    # The decoder is complete, the decoded symbols are now available in
    # the data_out buffer: check if it matches the data_in buffer
    print("Checking results...")
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unable to decode, please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
