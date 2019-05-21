#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2018.
# Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import sys

import kodo


def main():
    """
    Pure recode example using the Payload API.

    This example is very similar to encode_recode_decode_simple.py.
    The only difference is that this example uses a pure recoder instead of
    a decoder acting as a recoder. By "pure", we mean that the recoder will
    not decode the incoming data, it will only re-encode it.
    This example shows how to use an encoder, recoder, and decoder to
    simulate a simple relay network as shown below. For simplicity,
    we have error free links, i.e. no data packets are lost when being
    sent from encoder to recoder to decoder:

            +-----------+      +-----------+      +------------+
            |  encoder  |+---->| recoder   |+---->|  decoder   |
            +-----------+      +-----------+      +------------+

    The pure recoder does not need to decode all symbols, and its "capacity"
    for storing symbols can be limited. These stored coded symbols are
    linear combinations of previously received symbols. When a new symbol is
    received, the pure recoder will combine it with its existing symbols
    using random coefficients.
    The pure recoder can produce a random linear combination of the stored
    coded symbols, which can be processed by a regular decoder.
    """
    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 5
    symbol_size = 160

    # Create an encoder/decoder factory that are used to build the
    # actual encoders/decoders
    encoder_factory = kodo.RLNCEncoderFactory(field, symbols, symbol_size)
    encoder = encoder_factory.build()

    recoder_factory = kodo.RLNCPureRecoderFactory(field, symbols, symbol_size)
    # Set the pure recoder "capacity" to be less than "symbols"
    recoder_factory.set_recoder_symbols(3)
    recoder = recoder_factory.build()

    print("Recoder properties:\n"
          "  Symbols: {}\n"
          "  Recoder symbols: {}".format(
            recoder.symbols(),
            recoder.recoder_symbols()))

    decoder_factory = kodo.RLNCDecoderFactory(field, symbols, symbol_size)
    decoder = decoder_factory.build()

    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_const_symbols(data_in)

    # Define the data_out bytearrays where the symbols should be decoded
    # These bytearrays must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_mutable_symbols(data_out)

    while not decoder.is_complete():

        # Encode a packet into the payload buffer
        packet = encoder.write_payload()
        print("Encoded packet generated and passed to the recoder")

        # Pass that packet to the recoder
        recoder.read_payload(packet)

        # Now produce a new recoded packet from the current decoding buffer
        recoded_packet = recoder.write_payload()

        print("Recoded packet generated and passed to the decoder")

        # Pass the recoded packet to the decoder
        decoder.read_payload(recoded_packet)

        print("Decoder rank: {}/{}\n".format(decoder.rank(), symbols))

    # Check if we properly decoded the data
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
