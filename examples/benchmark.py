#! /usr/bin/env python
# encoding: utf-8

import time
import os
import sys
import argparse
import kodo


def run_coding_test(algorithm, field, symbols, symbol_size):
    """Run a timed encoding and decoding benchmark."""
    # First, we measure the combined setup time for the encoder and decoder
    start = time.clock()

    EncoderType = getattr(kodo, algorithm + 'Encoder')
    DecoderType = getattr(kodo, algorithm + 'Decoder')
    field = getattr(kodo.field, field)

    encoder = EncoderType(field, symbols, symbol_size)
    decoder = DecoderType(field, symbols, symbol_size)

    # Stop the setup timer
    stop = time.clock()
    # Calculate interval in microseconds
    setup_time = 1e6 * (stop - start)

    # We measure pure coding, so we always turn off the systematic mode
    if 'set_systematic_off' in dir(encoder):
        encoder.set_systematic_off()

    # Create random data to encode
    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_symbols_storage(data_in)

    data_out = bytearray(decoder.block_size())
    decoder.set_symbols_storage(data_out)

    # The generated payloads will be stored in this list
    payloads = []

    # Generate an ample number of coded symbols (considering binary)
    payload_count = 2 * symbols

    # Start the encoding timer
    start = time.clock()

    # Generate coded symbols with the encoder
    for i in range(payload_count):
        payload = encoder.produce_payload()
        payloads.append(payload)

    # Stop the encoding timer
    stop = time.clock()
    # Calculate interval in microseconds
    encoding_time = 1e6 * (stop - start)

    # Calculate the encoding rate in megabytes / seconds
    encoded_bytes = payload_count * symbol_size
    encoding_rate = encoded_bytes / encoding_time

    # Start the decoding timer
    start = time.clock()

    # Feed the coded symbols to the decoder
    for i in range(payload_count):
        if decoder.is_complete():
            break
        decoder.consume_payload(payloads[i])

    # Stop the decoding timer
    stop = time.clock()
    # Calculate interval in microseconds
    decoding_time = 1e6 * (stop - start)

    # Calculate the decoding rate in megabytes / seconds
    decoded_bytes = symbols * symbol_size
    decoding_rate = decoded_bytes / decoding_time

    if data_out == data_in:
        success = True
    else:
        success = False

    print("Setup time: {} microsec".format(setup_time))
    print("Encoding time: {} microsec".format(encoding_time))
    print("Decoding time: {} microsec".format(decoding_time))

    return (success, encoding_rate, decoding_rate)


def main():
    parser = argparse.ArgumentParser(description=run_coding_test.__doc__)

    algorithms = ['NoCode', 'RLNC', 'Perpetual', 'Fulcrum']
    fields = ['binary', 'binary4', 'binary8', 'binary16']

    parser.add_argument(
        '--algorithm',
        type=str,
        help='The algorithm to use',
        choices=algorithms,
        default='RLNC')

    parser.add_argument(
        '--field',
        type=str,
        help='The finite field to use',
        choices=fields,
        default='binary8')

    parser.add_argument(
        '--symbols',
        type=int,
        help='The number of symbols',
        default=16)

    parser.add_argument(
        '--symbol_size',
        type=int,
        help='The size of each symbol',
        default=1600)

    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Run without the actual benchmark.')

    args = parser.parse_args()

    if args.dry_run:
        sys.exit(0)

    print("Algorithm: {} / Finite field: {}".format(
        args.algorithm, args.field))

    print("Symbols: {} / Symbol size: {}".format(
        args.symbols, args.symbol_size))

    decoding_success, encoding_rate, decoding_rate = run_coding_test(
        args.algorithm,
        args.field,
        args.symbols,
        args.symbol_size)

    print("Encoding rate: {} MB/s".format(encoding_rate))
    print("Decoding rate: {} MB/s".format(decoding_rate))

    if decoding_success:
        print("Data decoded correctly.")
    else:
        print("Decoding failed.")


if __name__ == "__main__":
    main()
