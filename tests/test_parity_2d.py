#!/usr/bin/env python
# encoding: utf-8

"""Tests parity 2d encoding and decoding"""

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

import kodo
import random
import os


class Encoder(object):
    def __init__(self):

        self.encoder = kodo.block.Encoder(kodo.FiniteField.binary)
        self.generator = kodo.block.generator.Parity2D()
        self.generator.configure(rows=5, columns=5)
        self.symbols = self.generator.symbols
        self.symbol_bytes = 10
        self.encoder.configure(self.symbols, self.symbol_bytes)
        self.coded_symbol = bytearray(self.encoder.symbol_bytes)
        self.coefficients = bytearray(self.generator.max_coefficients_bytes)
        self.produce = None
        self.block_index = 0

    def consume(self, symbol):

        assert self.encoder.rank < self.encoder.symbols

        index = self.encoder.rank

        self.encoder.set_symbol_storage(symbol_storage=symbol, index=index)
        self.encoder.encode_systematic_symbol(
            symbol_storage=self.coded_symbol, index=index
        )

        self.produce(
            is_systematic=True,
            index=index,
            symbol=self.coded_symbol,
            block_index=self.block_index,
        )

        if self.encoder.rank == self.encoder.symbols:

            while self.generator.can_advance():

                if self.generator.can_generate():
                    repair_index = self.generator.generate(self.coefficients)
                    self.encoder.encode_symbol(self.coded_symbol, self.coefficients)

                    self.produce(
                        is_systematic=False,
                        index=repair_index,
                        symbol=self.coded_symbol,
                        block_index=self.block_index,
                    )

                self.generator.advance()

            self.encoder.reset()
            self.generator.reset()
            self.block_index += 1


class Decoder(object):
    def __init__(self):

        self.decoder = kodo.block.Decoder(kodo.FiniteField.binary)
        self.generator = kodo.block.generator.Parity2D()
        self.generator.configure(rows=5, columns=5)
        self.symbols = self.generator.symbols
        self.symbol_bytes = 10
        self.decoder.configure(self.symbols, self.symbol_bytes)
        self.symbols_storage = bytearray(self.decoder.block_bytes)
        self.decoder.set_symbols_storage(self.symbols_storage)
        self.coefficients = bytearray(self.generator.max_coefficients_bytes)
        self.produce = None
        self.block_index = 0
        self.release_index = 0

    def consume(self, is_systematic, index, symbol, block_index):

        if block_index != self.block_index:
            self.decoder.reset()
            self.generator.reset()
            self.decoder.set_symbols_storage(self.symbols_storage)
            self.block_index = block_index
            self.release_index = 0

        if is_systematic:
            self.decoder.decode_systematic_symbol(symbol=symbol, index=index)

        else:
            self.generator.generate_specific(self.coefficients, index)
            self.decoder.decode_symbol(symbol=symbol, coefficients=self.coefficients)

        while self.release_index < self.decoder.symbols:

            if not self.decoder.is_symbol_decoded(self.release_index):
                return

            release = self.decoder.symbol_at(self.release_index)

            self.produce(symbol=release)

            self.release_index += 1


def main():

    encoder = Encoder()
    decoder = Decoder()

    symbols_in = []
    symbols_out = []

    for _ in range(50):
        symbols_in.append(bytearray(os.urandom(10)))

    def channel(is_systematic, index, symbol, block_index):

        if random.random() * 100 < 5:
            return

        decoder.consume(
            is_systematic=is_systematic,
            index=index,
            symbol=symbol,
            block_index=block_index,
        )

    def consume_decoded(symbol):
        symbols_out.append(symbol)

    encoder.produce = channel
    decoder.produce = consume_decoded

    for symbol in symbols_in:
        encoder.consume(symbol=symbol)

    if len(symbols_in) != len(symbols_out):
        # Probably we did not decode
        return

    for i, (input, output) in enumerate(zip(symbols_in, symbols_out)):
        if input != output:
            print(f"{i}: {input} != {output}")
            assert 0

    assert symbols_in == symbols_out


if __name__ == "__main__":
    main()
