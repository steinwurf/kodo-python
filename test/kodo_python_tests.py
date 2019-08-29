#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import unittest

import kodo

# The test sets are only enabled for the codecs that were actually compiled
# (some repositories might be unavailable for certain users)
test_sets = []

# Carousel
if hasattr(kodo, 'NoCodeEncoder'):
    test_sets.append(([kodo.field.binary],
                      kodo.NoCodeEncoder,
                      kodo.NoCodeDecoder))

# RLNC
if hasattr(kodo, 'RLNCEncoder'):
    test_sets.append(([kodo.field.binary, kodo.field.binary4,
                      kodo.field.binary8, kodo.field.binary16],
                      kodo.RLNCEncoder,
                      kodo.RLNCDecoder))

# Perpetual
if hasattr(kodo, 'PerpetualEncoder'):
    test_sets.append(([kodo.field.binary, kodo.field.binary4,
                      kodo.field.binary8, kodo.field.binary16],
                      kodo.PerpetualEncoder,
                      kodo.PerpetualDecoder))

# Fulcrum
if hasattr(kodo, 'FulcrumEncoder'):
    test_sets.append(([kodo.field.binary4, kodo.field.binary8,
                      kodo.field.binary16],
                      kodo.FulcrumEncoder,
                      kodo.FulcrumDecoder))


class TestVersion(unittest.TestCase):

    def test_version(self):
        versions = kodo.__version__.split('\n')
        for version in versions:
            # Make sure that a version number is available for all
            # dependencies.
            self.assertNotEqual(
                version.split(':')[1].strip(), '', msg=version.strip())


class TestEncodeDecode(unittest.TestCase):

    def test_all(self):
        for test_set in test_sets:
            for field in test_set[0]:
                self.encode_decode_simple(field, test_set[1], test_set[2])

    def encode_decode_simple(self, field, Encoder, Decoder):

        symbols = 8
        symbol_size = 160

        encoder = Encoder(field, symbols, symbol_size)
        decoder = Decoder(field, symbols, symbol_size)

        data_in = bytearray(os.urandom(encoder.block_size()))
        encoder.set_symbols_storage(data_in)

        data_out = bytearray(decoder.block_size())
        decoder.set_symbols_storage(data_out)

        # Turn off systematic mode to test with coded symbols from the start
        if 'set_systematic_off' in dir(encoder):
            encoder.set_systematic_off()

        while not decoder.is_complete():
            # Generate an encoded packet
            packet = encoder.produce_payload()
            # Decode the encoded packet
            decoder.consume_payload(packet)

        # Check if we properly decoded the data
        self.assertEqual(data_out, data_in)


def main():
    unittest.main()

if __name__ == "__main__":
    main()
