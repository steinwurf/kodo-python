#!/usr/bin/env python
# encoding: utf-8

"""Tests sliding window encoding and decoding"""

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
import random

import unittest
import kodo


class TestSlideEncodeDecode(unittest.TestCase):
    def test_slide_encode_decode_simple(self):

        random_uniform_fields = [
            kodo.FiniteField.binary,
            kodo.FiniteField.binary4,
            kodo.FiniteField.binary8,
            kodo.FiniteField.binary16,
        ]

        for field in random_uniform_fields:
            with self.subTest(field):
                self.slide_encode_decode_simple(field)

    def slide_encode_decode_simple(self, field):

        symbol_bytes = 1400
        window_symbols = 20
        decoder_capacity = 40
        symbols = 6
        loss = 0.5

        encoder = kodo.slide.Encoder(field)
        self.assertEqual(field, encoder.field)

        encoder.configure(symbol_bytes)
        self.assertEqual(symbol_bytes, encoder.symbol_bytes)

        decoder = kodo.slide.Decoder(field)
        self.assertEqual(field, decoder.field)

        decoder.configure(symbol_bytes)
        self.assertEqual(symbol_bytes, decoder.symbol_bytes)

        rate = kodo.slide.RateController(10, 1)

        generator = kodo.slide.generator.RandomUniform(field)
        self.assertEqual(field, generator.field)

        symbol = bytearray(encoder.symbol_bytes)
        coefficients = None  # must be set after set_window on generator
        max_iterations = 1000
        iterations = 0

        decoded = []
        loss_probability = 10

        while iterations < max_iterations:
            iterations += 1
            repair = rate.send_repair()
            if repair:
                encoder.set_window(encoder.stream_lower_bound, encoder.stream_symbols)
                seed = random.randint(0, 100000)
                generator.set_seed(seed)
                coefficients = bytearray(
                    generator.coefficients_bytes(
                        encoder.window_lower_bound, encoder.window_symbols
                    )
                )
                generator.generate(
                    coefficients, encoder.window_lower_bound, encoder.window_symbols
                )
                encoder.encode_symbol(symbol, coefficients)
            else:
                if encoder.window_symbols == window_symbols:
                    encoder.pop_back_symbol()
                index = encoder.push_front_symbol(
                    bytearray(os.urandom(encoder.symbol_bytes))
                )
                encoder.encode_systematic_symbol(symbol, index)

            rate.advance()

            if random.randint(0, 100) < loss_probability:
                continue

            while decoder.stream_upper_bound < encoder.stream_upper_bound:
                if decoder.stream_symbols >= decoder_capacity:
                    decoder.pop_back_symbol()
                decoder.push_front_symbol(bytearray(decoder.symbol_bytes))

            if repair:
                decoder.set_window(encoder.window_lower_bound, encoder.window_symbols)
                generator.set_seed(seed)
                coefficients = bytearray(
                    generator.coefficients_bytes(
                        decoder.window_lower_bound, decoder.window_symbols
                    )
                )
                generator.generate(
                    coefficients, decoder.window_lower_bound, decoder.window_symbols
                )
                decoder.decode_symbol(symbol, coefficients)
            else:
                decoder.decode_systematic_symbol(symbol, index)

            for i in range(decoder.stream_symbols):

                index = i + decoder.stream_lower_bound
                if not decoder.is_symbol_decoded(index):
                    continue
                if index in decoded:
                    continue
                decoded.append(index)
                if decoder.in_stream(index) and encoder.in_stream(index):
                    data_out = decoder.symbol_at(index)
                    data_in = encoder.symbol_at(index)
                    self.assertEqual(data_in, data_out)


if __name__ == "__main__":
    unittest.main()
