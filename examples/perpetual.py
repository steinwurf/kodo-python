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
    Perpetual example.

    This Example shows how to use the additional settings and parameters
    supported by the perpetual code.
    """
    if not hasattr(kodo, 'PerpetualEncoderFactory'):
        print("The perpetual codec is not available")
        return

    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 40
    symbol_size = 160

    # Create encoder/decoder factory used to build actual encoders/decoders
    encoder_factory = kodo.PerpetualEncoderFactory(field, symbols, symbol_size)
    encoder = encoder_factory.build()

    decoder_factory = kodo.PerpetualDecoderFactory(field, symbols, symbol_size)
    decoder = decoder_factory.build()

    # The perpetual encoder supports three operation modes;
    #
    # 1) Random pivot mode (default)
    #    The pivot element is drawn at random for each coding symbol
    #
    #    example generated vectors (for width = 2)
    #
    #    0 0 1 X X 0
    #    X X 0 0 0 1
    #    X 0 0 0 1 X
    #    X X 0 0 0 1
    #    0 1 X X 0 0
    #    0 1 X X 0 0
    #
    #    1 X X 0 0 0
    #    0 0 0 1 X X
    #         .
    #         .
    #
    # 2) Pseudo systematic
    #    Pivot elements are generated with indices 0,1,2, ... , n,
    #    after which the generator reverts to the default random pivot
    #
    #    example generated vectors (for width = 2)
    #
    #    1 X X 0 0 0
    #    0 1 X X 0 0
    #    0 0 1 X X 0
    #    0 0 0 1 X X
    #    X 0 0 0 1 X
    #    X X 0 0 0 1
    #
    #    (additional vectors generated using the random mode)
    #
    # 3) Pre-charging
    #    For the first "width" symbols, the pivot index is 0. After that,
    #    the pseudo-systematic mode is used. Finally, pivots are drawn at
    #    random resulting in the indices 0 (width times), 1,2, ... , n
    #
    #    example generated vectors (for width = 2)
    #
    #    1 X X 0 0 0
    #    1 X X 0 0 0
    #    0 1 X X 0 0
    #    0 0 1 X X 0
    #    0 0 0 1 X X
    #
    #    X 0 0 0 1 X
    #    X X 0 0 0 1
    #
    #    (additional vectors generated using the random mode)
    #
    # The operation mode is set in the following. Note that if both
    # pre-charging and pseudo-systematic are enabled, pre-charging takes
    # precedence.

    # Enable the pseudo-systematic operation mode - faster
    encoder.set_pseudo_systematic(True)

    # Enable the pre-charing operation mode - even faster
    # encoder.set_pre_charging(True)

    print("Pseudo-systematic is {}\nPre-charging is {}".format(
        "on" if encoder.pseudo_systematic() else "off",
        "on" if encoder.pre_charging() else "off"))

    # The width of the perpetual code can be set either as a number of symbols
    # using set_width(), or as a ratio of the generation size using
    # set_width_ratio().

    # The default width is set to 10% of the generation size.
    print("The width ratio defaults to: {} (therefore the calculated width is "
          "{})".format(encoder.width_ratio(), encoder.width()))
    encoder.set_width(6)

    print("The width was set to: {} (therefore the calculated width ratio is "
          "{})".format(encoder.width(), encoder.width_ratio()))

    encoder.set_width_ratio(0.2)
    print("The width ratio was set to: {} (therefore the calculated width is "
          "{})".format(encoder.width_ratio(), encoder.width()))

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_const_symbols(data_in)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_mutable_symbols(data_out)

    while not decoder.is_complete():
        # Encode a packet into the payload buffer
        payload = encoder.write_payload()

        # Pass that packet to the decoder
        decoder.read_payload(payload)

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
