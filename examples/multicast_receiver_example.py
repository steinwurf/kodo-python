#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import argparse
import kodo
import socket
import struct
import sys
import time

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 5007


def main():
    """
    Multicast example, reciever part.

    An example where data is received, decoded, and finally written to a file.
    """
    parser = argparse.ArgumentParser(description=main.__doc__)
    parser.add_argument(
        '--output-file',
        type=str,
        help='Path to the file which should be received.',
        default='output_file')

    parser.add_argument(
        '--ip',
        type=str,
        help='The IP address to use.',
        default=MCAST_GRP)

    parser.add_argument(
        '--port',
        type=int,
        help='The port to use.',
        default=MCAST_PORT)

    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Run without network use.')

    args = parser.parse_args()

    field = kodo.field.binary
    symbols = 64
    symbol_size = 1400

    decoder_factory = kodo.RLNCDecoderFactory(field, symbols, symbol_size)
    decoder = decoder_factory.build()

    data_out = bytearray(decoder.block_size())
    decoder.set_mutable_symbols(data_out)

    sock = socket.socket(
        family=socket.AF_INET,
        type=socket.SOCK_DGRAM,
        proto=socket.IPPROTO_UDP)

    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    sock.bind(('', args.port))
    mreq = struct.pack("4sl", socket.inet_aton(args.ip), socket.INADDR_ANY)

    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    if args.dry_run:
        sys.exit(0)

    print("Processing...")
    while not decoder.is_complete():
        time.sleep(0.2)
        packet = sock.recv(10240)

        decoder.read_payload(bytearray(packet))
        print("Packet received!")
        print("Decoder rank: {}/{}".format(decoder.rank(), decoder.symbols()))

    # Write the decoded data to the output file
    f = open(args.output_file, 'wb')
    f.write(data_out)
    f.close()

    print("Processing finished.")

if __name__ == "__main__":
    main()
