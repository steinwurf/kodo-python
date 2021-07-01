#!/usr/bin/env python
# encoding: utf-8

"""Tests perpetual encoding and decoding"""

# License for Commercial Usage
# Distributed under the "KODO EVALUATION LICENSE 1.3"
# Licensees holding a valid commercial license may use this project in
# accordance with the standard license agreement terms provided with the
# Software (see accompanying file LICENSE.rst or
# https://www.steinwurf.com/license), unless otherwise different terms and
# conditions are agreed in writing between Licensee and Steinwurf ApS in which
# case the license will be regulated by that separate written agreement.
# License for Non-Commercial Usage
# Distributed under the "KODO RESEARCH LICENSE 1.2"
# Licensees holding a valid research license may use this project in accordance
# with the license agreement terms provided with the Software
# See accompanying file LICENSE.rst or https://www.steinwurf.com/license

import os
import unittest
import kodo


class TestPerpetualEncodeDecode(unittest.TestCase):
    def test_encode_decode_offset_random_sequence(self):
        fields = [
            kodo.FiniteField.binary,
            kodo.FiniteField.binary4,
            kodo.FiniteField.binary8,
            kodo.FiniteField.binary16,
        ]
        for field in fields:
            with self.subTest(field):
                self.encode_decode_offset_random_sequence(field)

    def encode_decode_offset_random_sequence(self, field):

        symbol_bytes = 1400

        block_bytes = 1000000

        width = 100

        encoder = kodo.perpetual.Encoder(field)
        decoder = kodo.perpetual.Decoder(field)

        encoder.configure(block_bytes, symbol_bytes, width)
        decoder.configure(block_bytes, symbol_bytes, width)
        self.assertEqual(encoder.width, width)
        self.assertEqual(encoder.symbols, block_bytes // symbol_bytes + 1)
        self.assertEqual(encoder.symbol_bytes, symbol_bytes)
        self.assertEqual(encoder.block_bytes, block_bytes)

        self.assertEqual(decoder.width, width)
        self.assertEqual(decoder.symbols, block_bytes // symbol_bytes + 1)
        self.assertEqual(decoder.symbol_bytes, symbol_bytes)
        self.assertEqual(decoder.block_bytes, block_bytes)

        generator = kodo.perpetual.generator.RandomUniform(field)
        offset_generator = kodo.perpetual.offset.RandomSequence()

        generator.configure(encoder.symbols, encoder.width)
        offset_generator.configure(encoder.symbols, encoder.width)

        symbol = bytearray(encoder.symbol_bytes)
        coefficients = bytearray(generator.max_coefficients_bytes)

        data_in = bytearray(os.urandom(encoder.block_bytes))
        encoder.set_symbols_storage(data_in)

        data_out = bytearray(decoder.block_bytes)
        decoder.set_symbols_storage(data_out)

        while not decoder.is_complete():
            offset = offset_generator.offset()

            generator.set_offset(offset)
            generator.generate(coefficients)

            encoder.encode_symbol(symbol, coefficients, offset)

            decoder.decode_symbol(symbol, coefficients, offset)

            if decoder.can_complete_decoding():
                decoder.complete_decoding()

        self.assertEqual(data_in, data_out)

    def test_encode_decode_offset_sequential_sequence(self):
        fields = [
            kodo.FiniteField.binary,
            kodo.FiniteField.binary4,
            kodo.FiniteField.binary8,
            kodo.FiniteField.binary16,
        ]
        for field in fields:
            with self.subTest(field):
                self.encode_decode_offset_sequential_sequence(field)

    def encode_decode_offset_sequential_sequence(self, field):

        symbol_bytes = 1400

        block_bytes = 1000000

        width = 100

        encoder = kodo.perpetual.Encoder(field)
        decoder = kodo.perpetual.Decoder(field)

        encoder.configure(block_bytes, symbol_bytes, width)
        decoder.configure(block_bytes, symbol_bytes, width)
        self.assertEqual(encoder.width, width)
        self.assertEqual(encoder.symbols, block_bytes // symbol_bytes + 1)
        self.assertEqual(encoder.symbol_bytes, symbol_bytes)
        self.assertEqual(encoder.block_bytes, block_bytes)

        self.assertEqual(decoder.width, width)
        self.assertEqual(decoder.symbols, block_bytes // symbol_bytes + 1)
        self.assertEqual(decoder.symbol_bytes, symbol_bytes)
        self.assertEqual(decoder.block_bytes, block_bytes)

        generator = kodo.perpetual.generator.RandomUniform(field)
        offset_generator = kodo.perpetual.offset.SequentialSequence()

        generator.configure(encoder.symbols, encoder.width)
        offset_generator.configure(encoder.symbols, encoder.width)

        symbol = bytearray(encoder.symbol_bytes)
        coefficients = bytearray(generator.max_coefficients_bytes)

        data_in = bytearray(os.urandom(encoder.block_bytes))
        encoder.set_symbols_storage(data_in)

        data_out = bytearray(decoder.block_bytes)
        decoder.set_symbols_storage(data_out)

        while not decoder.is_complete():
            offset = offset_generator.offset()

            generator.set_offset(offset)
            generator.generate(coefficients)

            encoder.encode_symbol(symbol, coefficients, offset)

            decoder.decode_symbol(symbol, coefficients, offset)

            if decoder.can_complete_decoding():
                decoder.complete_decoding()

        self.assertEqual(data_in, data_out)

    def test_encode_decode_offset_random_uniform(self):
        fields = [
            kodo.FiniteField.binary,
            kodo.FiniteField.binary4,
            kodo.FiniteField.binary8,
            kodo.FiniteField.binary16,
        ]
        for field in fields:
            with self.subTest(field):
                self.encode_decode_offset_random_uniform(field)

    def encode_decode_offset_random_uniform(self, field):

        symbol_bytes = 1400

        block_bytes = 1000000

        width = 100

        encoder = kodo.perpetual.Encoder(field)
        decoder = kodo.perpetual.Decoder(field)

        encoder.configure(block_bytes, symbol_bytes, width)
        decoder.configure(block_bytes, symbol_bytes, width)
        self.assertEqual(encoder.width, width)
        self.assertEqual(encoder.symbols, block_bytes // symbol_bytes + 1)
        self.assertEqual(encoder.symbol_bytes, symbol_bytes)
        self.assertEqual(encoder.block_bytes, block_bytes)

        self.assertEqual(decoder.width, width)
        self.assertEqual(decoder.symbols, block_bytes // symbol_bytes + 1)
        self.assertEqual(decoder.symbol_bytes, symbol_bytes)
        self.assertEqual(decoder.block_bytes, block_bytes)

        generator = kodo.perpetual.generator.RandomUniform(field)
        offset_generator = kodo.perpetual.offset.RandomUniform()

        generator.configure(encoder.symbols, encoder.width)
        offset_generator.configure(encoder.symbols, encoder.width)

        symbol = bytearray(encoder.symbol_bytes)
        coefficients = bytearray(generator.max_coefficients_bytes)

        data_in = bytearray(os.urandom(encoder.block_bytes))
        encoder.set_symbols_storage(data_in)

        data_out = bytearray(decoder.block_bytes)
        decoder.set_symbols_storage(data_out)

        while not decoder.is_complete():
            offset = offset_generator.offset()

            generator.set_offset(offset)
            generator.generate(coefficients)

            encoder.encode_symbol(symbol, coefficients, offset)

            decoder.decode_symbol(symbol, coefficients, offset)

            if decoder.can_complete_decoding():
                decoder.complete_decoding()

        self.assertEqual(data_in, data_out)


if __name__ == "__main__":
    unittest.main()
