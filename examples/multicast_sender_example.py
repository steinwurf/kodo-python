#!/usr/bin/env python
# encoding: utf-8

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

import argparse
import kodo
import os
import socket
import struct
import sys
import time
import random

MCAST_GRP = "224.1.1.1"
MCAST_PORT = 5007


def main():
    """
    Multicast example, sender part.
    An example where data is read from a file, encoded, and then send
    via the network.
    """
    parser = argparse.ArgumentParser(description=main.__doc__)

    """The parser takes a path to a file as input."""
    parser.add_argument(
        "--file-path",
        type=str,
        help="Path to the file which should be sent.",
        default=os.path.realpath(__file__),
    )

    """The parser takes the target IP-address as input."""
    parser.add_argument(
        "--ip", type=str, help="The IP address to send to.", default=MCAST_GRP
    )

    """The parser takes the target port as input."""
    parser.add_argument(
        "--port", type=int, help="The port to send to.", default=MCAST_PORT
    )

    """One can tell the parser to run without using the network."""
    parser.add_argument(
        "--dry-run", action="store_true", help="Run without network use."
    )

    args = parser.parse_args()

    # Check the file.
    if not os.path.isfile(args.file_path):
        print("{} is not a valid file.".format(args.file_path))
        sys.exit(1)

    field = kodo.FiniteField.binary8

    file_stats = os.stat(args.file_path)
    block_bytes = file_stats.st_size

    symbol_bytes = 1400
    symbols = block_bytes // symbol_bytes + 1
    width = 10

    if symbols < width:
        width = symbols

    # Create and configure the encoder, coefficient generator and offset generator.
    encoder = kodo.perpetual.Encoder(field)
    encoder.configure(block_bytes, symbol_bytes, width)

    generator = kodo.perpetual.generator.RandomUniform(field)
    generator.configure(symbols, width)

    offset_generator = kodo.perpetual.offset.RandomSequence()
    offset_generator.configure(symbols, width)

    sock = socket.socket(
        family=socket.AF_INET, type=socket.SOCK_DGRAM, proto=socket.IPPROTO_UDP
    )

    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)

    # Read the input data from the file, only the block_bytes first bytes
    # can fit into a single generation. No more data will be sent.
    # If the file is smaller than block_bytes then it will be zero-padded.
    f = open(os.path.expanduser(args.file_path), "rb")
    data_in = bytearray(f.read().ljust(encoder.block_bytes))
    f.close()

    # Assign the data_in buffer to the encoder symbol_storage.
    encoder.set_symbols_storage(data_in)

    if args.dry_run:
        sys.exit(0)

    address = (args.ip, args.port)

    coefficients = bytearray(generator.max_coefficients_bytes)
    symbol = bytearray(encoder.symbol_bytes)

    header_data = bytearray(27)

    print("Processing...")
    while True and not args.dry_run:

        time.sleep(0.2)

        # Generate an encoded packet.
        seed = random.randint(0, 2 ** 64 - 1)
        generator.set_seed(seed)

        offset = offset_generator.offset()
        generator.set_offset(offset)
        generator.generate(coefficients)

        encoder.encode_symbol(symbol, coefficients, offset)

        struct.pack_into(
            "<QQBIIH",
            header_data,
            0,
            seed,
            offset,
            field.value,
            block_bytes,
            symbol_bytes,
            width,
        )

        # Send the encoded packet with seed and offset.
        packet = header_data + symbol
        sock.sendto(packet, address)
        print("Packet sent!")


if __name__ == "__main__":
    main()
