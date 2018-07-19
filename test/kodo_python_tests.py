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
if hasattr(kodo, 'NoCodeEncoderFactory'):
    test_sets.append(([kodo.field.binary],
                      kodo.NoCodeEncoderFactory,
                      kodo.NoCodeDecoderFactory))

# RLNC
if hasattr(kodo, 'RLNCEncoderFactory'):
    test_sets.append(([kodo.field.binary, kodo.field.binary4,
                      kodo.field.binary8, kodo.field.binary16],
                      kodo.RLNCEncoderFactory,
                      kodo.RLNCDecoderFactory))

# Perpetual
if hasattr(kodo, 'PerpetualEncoderFactory'):
    test_sets.append(([kodo.field.binary, kodo.field.binary4,
                      kodo.field.binary8, kodo.field.binary16],
                      kodo.PerpetualEncoderFactory,
                      kodo.PerpetualDecoderFactory))

# Fulcrum
if hasattr(kodo, 'FulcrumEncoderFactory'):
    test_sets.append(([kodo.field.binary4, kodo.field.binary8,
                      kodo.field.binary16],
                      kodo.FulcrumEncoderFactory,
                      kodo.FulcrumDecoderFactory))


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

    def encode_decode_simple(self, field, EncoderFactory, DecoderFactory):

        symbols = 8
        symbol_size = 160

        encoder_factory = EncoderFactory(field, symbols, symbol_size)
        encoder = encoder_factory.build()

        decoder_factory = DecoderFactory(field, symbols, symbol_size)
        decoder = decoder_factory.build()

        data_in = bytearray(os.urandom(encoder.block_size()))
        encoder.set_const_symbols(data_in)

        data_out = bytearray(decoder.block_size())
        decoder.set_mutable_symbols(data_out)

        # Turn off systematic mode to test with coded symbols from the start
        if 'set_systematic_off' in dir(encoder):
            encoder.set_systematic_off()

        while not decoder.is_complete():
            # Generate an encoded packet
            packet = encoder.write_payload()
            # Decode the encoded packet
            decoder.read_payload(packet)

        # Check if we properly decoded the data
        self.assertEqual(data_out, data_in)


def main():
    unittest.main()

if __name__ == "__main__":
    main()
