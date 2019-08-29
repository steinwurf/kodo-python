#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2011-2013.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import random
import sys

import kodo


def main():
    """An example for using the trace functionality."""
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 5
    symbol_size = 16

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

    # Setup tracing

    # Enable the stdout trace function of the encoder
    encoder.set_log_stdout()
    encoder.set_zone_prefix("encoder")

    # Define a custom trace function for the decoder which filters the
    # trace message based on their zones
    def callback_function(zone, message):
        if zone in ["decoder_state", "symbol_coefficients_before_consume_symbol"]:
            print("{}:".format(zone))
            print(message)

    decoder.set_log_callback(callback_function)

    while not decoder.is_complete():

        # Encode a packet into the payload buffer
        packet = encoder.produce_payload()

        # Here we "simulate" a packet loss of approximately 50%
        # by dropping half of the encoded packets.
        # When running this example you will notice that the initial
        # symbols are received systematically (i.e. decoded). After
        # sending all symbols once decoded, the encoder will switch
        # to full coding, in which case you will see the full encoding
        # vectors being sent and received.
        if random.choice([True, False]):
            print("Packet dropped.\n")
            continue

        # Pass that packet to the decoder
        decoder.consume_payload(packet)

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
