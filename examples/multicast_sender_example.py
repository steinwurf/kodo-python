#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import argparse
import kodo
import os
import socket
import sys
import time

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 5007


def main():
    """Example of a sender which encodes and sends a file."""
    parser = argparse.ArgumentParser(description=main.__doc__)
    parser.add_argument(
        '--file-path',
        type=str,
        help='Path to the file which should be sent.',
        default=os.path.realpath(__file__))

    parser.add_argument(
        '--ip',
        type=str,
        help='The IP address to send to.',
        default=MCAST_GRP)

    parser.add_argument(
        '--port',
        type=int,
        help='The port to send to.',
        default=MCAST_PORT)

    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Run without network use.')

    args = parser.parse_args()

    # Check file.
    if not os.path.isfile(args.file_path):
        print("{} is not a valid file.".format(args.file_path))
        sys.exit(1)

    field = kodo.field.binary
    symbols = 64
    symbol_size = 1400

    encoder = kodo.RLNCEncoder(field, symbols, symbol_size)

    sock = socket.socket(
        family=socket.AF_INET,
        type=socket.SOCK_DGRAM,
        proto=socket.IPPROTO_UDP)

    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)

    # Read the input data from the file, only the first 64*1400 bytes can
    # fit into a single generation. No more data will be sent.
    # If the file is smaller than 64*1400 bytes, then it will be zero-padded.
    f = open(os.path.expanduser(args.file_path), 'rb')
    data_in = bytearray(f.read().ljust(encoder.block_size()))
    f.close()

    # Assign the data_in buffer to the encoder
    encoder.set_symbols_storage(data_in)

    if args.dry_run:
        sys.exit(0)

    address = (args.ip, args.port)

    print("Processing")
    while True and not args.dry_run:
        time.sleep(0.2)
        # Generate an encoded packet
        packet = encoder.produce_payload()
        # Send the packet
        sock.sendto(packet, address)
        print("Packet sent!")

    print("Processing finished")

if __name__ == "__main__":
    main()
