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

        encoder = kodo.RLNCEncoder(field, symbols, symbol_size)
        decoder = kodo.RLNCDecoder(field, symbols, symbol_size)

        data_in = bytearray(os.urandom(encoder.block_size()))
        encoder.set_symbols_storage(data_in)

        data_out = bytearray(decoder.block_size())
        decoder.set_symbols_storage(data_out)

        def decoder_callback(zone, msg):
            decoder_viewer.log_callback(zone, msg)
        decoder.set_log_callback(decoder_callback)

        def encoder_callback(zone, msg):
            encoder_viewer.log_callback(zone, msg)
        encoder_viewer.set_symbols(encoder.symbols())
        encoder.set_log_callback(encoder_callback)

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
                continue

            # Pass that packet to the decoder
            decoder.consume_payload(packet)

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
