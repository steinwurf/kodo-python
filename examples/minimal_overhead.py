#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http:#www.steinwurf.com/licensing

import os
import sys
import struct
import kodo

# In this example, we will try to show how to use RLNC with minimal
# header overhead. To do this, we have to make some restrictions to the
# code:
#
#     1. We will not support systematic mode. Systematic means that all
#        source symbols are sent once uncoded. Adding support for this
#        would require us to add a bit to the header informing the
#        decoder that the symbol is uncoded. Sending systematic symbols
#        has a nice computational complexity advantage, since uncoded
#        symbols require no decoding.
#
#     2. We will not support recoding. Recoding is an unique feature of
#        RLNC which has significant advantages in some scenarios. Adding
#        recoding means that we have to send what is known as the
#        encoding vector (describing the encoding), which will require
#        at least a single bit per source symbol. In some cases this is
#        acceptable in some cases not. For the simplicity of this
#        example, we will not go further into that here.
#
#     3. All source symbols have the same size in bytes.
#
# We will use a seed-based code in this example, but we will customize
# the standard RLNC codec that supports the systematic mode and uses a
# 4-byte value for the random seed - so it is not really squeezing the header
# to the minimum as we are trying to do here. The basic idea of a seed code
# is to replace the randomly generated coding coefficients (needed for
# recoding) with a seed to a pre-determined PRNG (Pseudo Random Number
# Generator) which can be used by the encoder and decoder to reconstruct
# the coding coefficients.
#
# So let's look at our minimal header layout:
#
#     +--------+----------------------------+
#     | symbol |          symbol            |
#     |   id   |           data             |
#     +--------+----------------------------+
#
# Our header contains two fields:
#
#     1. "symbol id": The field contains information needed by the
#        decoder to understand how the encoder produced the encoded
#        symbols and thus how to decode it. We can change the size of
#        this field depending on how many encoded symbols we expect to
#        produce. Potentially a systematic mode could be implemented by
#        reserving a range of values in this field to represent the
#        original source symbols.
#
#     2. "symbol data": This field contains the encoded data. It will
#        always have the same size in bytes, which corresponds to the
#        chosen symbol size. We can not change the size of this field.
#        In situations where we have variable size source symbols, some
#        optimizations can be performed even here.
#
# So we can tune the size of the "symbol id" field depending on
# two parameters:
#
#     1. The number of source symbols.
#
#     2. The amount of redundancy we want to generate.
#
# From these two numbers we can calculate the total number of symbols
# we want to transmit/store. The "symbol id" field needs to be able to
# represent the encoding for all the generated symbols. Therefore it
# needs to have a size in bits large enough, corresponding to
# log2(t), where t is the total number of symbols to send (source
# symbols + redundancy) and log2 is the binary logarithmic function. This
# might not always produce a nice even number of bits, so in practice
# often we need to round this up to some even number of bytes. Leftover
# bits could be used for other purposes (such as systematic mode).
#
# Let's look at some examples:
#
#     1. Source symbols = 8192
#        Rdundancy = 50%
#        Total symbols = 8192 * 1.5 = 12288
#        Bits needed = log2(12288) = 13.58
#        Bytes needed = 2
#
#     2. Source symbols = 65536
#        Rdundancy = 50%
#        Total symbols = 65536 * 1.5 = 98304
#        Bits needed = log2(12288) = 16.58
#        Bytes needed = 3
#
#     3. Source symbols = 131072
#        Rdundancy = 50%
#        Total symbols = 131072 * 1.5 = 196608
#        Bits needed = log2(12288) = 17.58
#        Bytes needed = 3
#
# A comment regarding the amount of source symbols: It is important to
# be aware that computational complexity grows as the number of symbols
# increases - there are therefore practical implications of increasing the
# number of symbols.
#
# In the example below we will use 2 bytes for the "symbol id". This
# means we have 65536 possible unique seed values we can use. Which
# would be sufficient for various configurations of source symbols and
# redundancy levels (as illustrated in the example above).

def main():

    # Define the "symbol id" size (storing the PRNG seed), which in this
    # case is 2 bytes.
    id_size = 2

    # Choose the finite field, the number of symbols (i.e. generation size)
    # and the symbol size in bytes
    field = kodo.field.binary8
    symbols = 42
    symbol_size = 160

    # Let's calculate the header overhead we are adding due to the
    # "symbol id". Notice that I use the word header overhead: this is to
    # mean we only concentrate on the overhead caused by the added
    # header, in this case the "symbol id" only. There are other ways to
    # define overhead, e.g. excess generated redundancy or similar, we will
    # not look at those here.
    #
    # We are adding 2 bytes per encoded symbol, so this is the total
    # per-symbol overhead.
    #
    # If we generate 100% redundancy, we would produce in total 84
    # symbols. Those 84 symbols would each have 2 bytes of overhead so
    # in total 168 bytes.
    #
    # If we wanted to store the 84 encoded symbols in memory or on disk
    # they would take up: 84*(160+2) = 13608 bytes (out of which 168
    # bytes would be overhead from the "symbol id"). One minor note here
    # is that in a communication scenario we would typically store and
    # send a single encoded symbol at a time (which would need 160+2 bytes).
    # In both cases the input data also needs to be in memory.
    #
    # From a receiver's perspective only approx. 42 symbols would need to
    # be received in order to be able to decode the original data. So a
    # receiver would need to download 42*(160+2) = 6804 bytes to decode
    # the 42*160 = 6720 bytes of original data. This overhead in that case
    # is 42*2 = 84 bytes.
    #
    # Note that we said approx. 42 symbols is needed to decode. This
    # number depends on the exact type of code used and its parameters,
    # e.g. the size of the finite field. If less than 42 symbols are received,
    # only partial decoding is possible, which means that parts of the
    # file may be decoded, but not all.
    #
    # With RLNC we can generate as much redundancy as we want, in this
    # case we are limited by the 2-byte "symbol id". If instead we used
    # a 4-byte "symbol id", we could have generated 2^32 encoded symbols.
    # In a network, this may make sense, since it allows us to produce a
    # virtually unlimited number of encoded packets.


    # Create an encoder using the specified parameters
    encoder = kodo.RLNCEncoder(field, symbols, symbol_size)


    # Generate some random data to encode. We create a bytearray of the same
    # size as the encoder's block size and assign it to the encoder.
    # This bytearray must not go out of scope while the encoder exists!
    data_in = bytearray(os.urandom(encoder.block_size()))

    # Assign the data buffer to the encoder so that we may start
    # to produce encoded symbols from it
    encoder.set_symbols_storage(data_in)

    # We create twice the symbols so 100% redundancy.
    #
    # The amount of redundancy we generate depends on the amount of
    # erasures (packet loss) we expect to see. For example with a loss
    # rate of 50% i.e. loosing half the packets sent. We need to send at
    # least twice the packets i.e. 100% redundancy. How to tune/adapt
    # these numbers are system dependent and dependens on the desired
    # decoding probability.
    #
    # Specifically for the above example we add 100% redundancy, which
    # means from the 42 symbols we get 84 encoded symbols. If we have an
    # loss rate of 50% it means that on average we get the 42 symbols
    # needed to decode. So half of the time we will decode because we get
    # 42 symbols or more and half of the time we will fail because we get
    # less than the 42 needed symbols. Depending on the chosen finite
    # field size we also get an effect from linear dependency, which means
    # we can expect to decode less than half of the time. We can calculate
    # the redundancy needed to ensure decoding 99% of the time using
    # probability theory.
    # In communication scenarios the amount of redundancy often times
    # feedback will be used to control redundancy levels, how relaxed we
    # can be depends on the cost of asking for more redundancy (which
    # is system dependent).
    # In systems without feedback, often times the best we can do is to
    # simply blindly generate redundancy (for a given period of time).
    # In this case the rate-less feature of RLNC is handy. Rate less
    # simply means that there is no upper limit to the amount of
    # redundancy we can generate.
    #
    # It is often the case that in storage scenarios a fixed redundancy
    # level is used, whereas in communication scenarios an adaptive
    # redundancy level is used (RLNC supports both modes). The adaptive
    # mode is demonstrated e.g. in examples/encode_decode_simple where
    # the encoder keeps producing symbols until the decoder is complete.
    #
    # One subtle thing to notice is that we are using produce_symbol
    # (symbol API) instead of produce_payload (payload API) which is
    # used in many other examples (such as encode_decode_simple).
    #
    # The payload API is a "higher level" API, which allows people to
    # get going quickly. It has its own internal header format, which
    # uses some bytes for signaling various state between an encoder and
    # decoder.
    #
    # The symbol API is "low-level" in the sense that we fully control what
    # is written. As such, a call to produce_symbol(...) will write only
    # the encoded symbol to the buffer passed. We have to write the
    # coefficients and the seed values ourselves as shown below.
    payloads = []
    for seed in range(symbols * 2):
        payload = bytearray()

        # Set the seed value to use
        encoder.set_seed(seed)

        # Write the seed value as two bytes in big endian
        two_bytes_seed = struct.pack('>H', seed)
        # Add the seed as the first part of the payload
        payload.extend(two_bytes_seed)

        # Generate an encoding vector using the current seed value
        coefficients = encoder.generate()

        # Write a symbol according to the generated coefficients
        symbol = encoder.produce_symbol(coefficients)

        # Add the symbol data as the second part of the payload
        payload.extend(symbol)

        # Store the payload
        payloads.append(payload)


    # Now let's create the decoder
    decoder = kodo.RLNCDecoder(field, symbols, symbol_size)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the encoder exists!
    data_out = bytearray(decoder.block_size())
    decoder.set_symbols_storage(data_out)

    for payload in payloads:
        # Extract the seed and symbol data from the payload
        seed = struct.unpack_from('>H', payload, 0)[0]
        symbol_data = payload[2:]

        # Set the seed to use
        decoder.set_seed(seed)

        # Generate the encoding vector using the current seed value
        coefficients = decoder.generate()

        # Read a symbol data according to the generated coefficients
        decoder.consume_symbol(
            symbol_data=symbol_data,
            coefficients=coefficients)

        if decoder.is_complete():
            break

    print("Coding finished")

    # The decoder is complete, the decoded symbols are now available in
    # the data_out buffer: check if it matches the data_in buffer
    print("Checking results...")
    if data_out == data_in:
        print("Data decoded correctly")
    else:
        print("Unable to decode please file a bug report :)")
        sys.exit(1)

if __name__ == "__main__":
    main()
