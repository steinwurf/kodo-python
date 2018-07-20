#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

from __future__ import print_function
from __future__ import division

import kodo
import kodo_helpers

import os
import random
import sys
import time


def main():

    # Setup canvas and viewer
    size = 512
    canvas = kodo_helpers.CanvasScreenEngine(size * 2, size)

    encoder_viewer = kodo_helpers.EncodeStateViewer(
        size=size,
        canvas=canvas)

    decoder_viewer = kodo_helpers.DecodeStateViewer(
        size=size,
        canvas=canvas,
        canvas_position=(size, 0))

    canvas.start()
    try:
        field = kodo.field.binary8
        symbols = 64
        symbol_size = 16

        encoder_factory = kodo.RLNCEncoderFactory(field, symbols, symbol_size)
        encoder = encoder_factory.build()

        decoder_factory = kodo.RLNCDecoderFactory(field, symbols, symbol_size)
        decoder = decoder_factory.build()

        data_in = bytearray(os.urandom(encoder.block_size()))
        encoder.set_const_symbols(data_in)

        data_out = bytearray(decoder.block_size())
        decoder.set_mutable_symbols(data_out)

        def decoder_callback(zone, msg):
            decoder_viewer.trace_callback(zone, msg)
        decoder.set_trace_callback(decoder_callback)

        def encoder_callback(zone, msg):
            encoder_viewer.trace_callback(zone, msg)
        encoder_viewer.set_symbols(encoder.symbols())
        encoder.set_trace_callback(encoder_callback)

        while not decoder.is_complete():
            # Encode a packet into the payload buffer
            packet = encoder.write_payload()

            # Here we "simulate" a packet loss of approximately 50%
            # by dropping half of the encoded packets.
            # When running this example you will notice that the initial
            # symbols are received systematically (i.e. uncoded). After
            # sending all symbols once uncoded, the encoder will switch
            # to full coding, in which case you will see the full encoding
            # vectors being sent and received.
            if random.choice([True, False]):
                continue

            # Pass that packet to the decoder
            decoder.read_payload(packet)

        time.sleep(1)
    finally:
        # What ever happens, make sure we stop the viewer.
        canvas.stop()

    # Check we properly decoded the data
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
