#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2011-2013.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http:#www.steinwurf.com/licensing

import os
import random
import sys

import kodo


def main():
    """
    Switch systematic off example.

    This example shows how to enable or disable systematic coding for
    coding stacks that support it.
    Systematic coding is used to reduce the amount of work done by an
    encoder and a decoder. This is achieved by initially sending all
    symbols which has not previously been sent uncoded. Kodo allows this
    feature to be optionally turn of or off.
    """
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary
    symbols = 10
    symbol_size = 160

    # Create an encoder/decoder factory that are used to build the
    # actual encoders/decoders
    encoder_factory = kodo.RLNCEncoderFactory(field, symbols, symbol_size)
    encoder = encoder_factory.build()

    decoder_factory = kodo.RLNCDecoderFactory(field, symbols, symbol_size)
    decoder = decoder_factory.build()

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_const_symbols(data_in)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_mutable_symbols(data_out)

    print("Starting encoding / decoding...")

    while not decoder.is_complete():

        # If the chosen codec stack supports systematic coding
        if 'is_systematic_on' in dir(encoder):

            # Toggle systematic mode with 50% probability
            if random.choice([True, False]):

                if encoder.is_systematic_on():
                    print("Turning systematic OFF")
                    encoder.set_systematic_off()
                else:
                    print("Turning systematic ON")
                    encoder.set_systematic_on()

        # Encode a packet into the payload buffer
        packet = encoder.write_payload()

        if random.choice([True, False]):
            print("Packet dropped.")
            continue

        # Pass that packet to the decoder
        decoder.read_payload(packet)

        print("Rank of decoder {}".format(decoder.rank()))

        # Symbols that were received in the systematic phase correspond
        # to the original source symbols and are therefore marked as
        # decoded
        print("Symbols decoded {}".format(decoder.symbols_uncoded()))

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
