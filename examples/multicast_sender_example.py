import argparse
import kodo
import os
import socket
import sys
import time

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 5007


def main():
    """
    Example of a sender which encodes and sends a file.
    """

    parser = argparse.ArgumentParser(description=main.__doc__)
    parser.add_argument(
        '--file-path',
        type=str,
        help='Path to the file which should be send.',
        default=os.path.realpath(__file__))

    parser.add_argument(
        '--ip',
        type=str,
        help='The ip to send to.',
        default=MCAST_GRP)

    parser.add_argument(
        '--port',
        type=int,
        help='The port to send to.',
        default=MCAST_PORT)

    parser.add_argument(
        '--loops',
        type=int,
        help='The number of loops. Set to a negative number to loop forever.',
        default=10)

    args = parser.parse_args()

    # Check file.
    if not os.path.isfile(args.file_path):
        print("{} is not a valid file.".format(args.file_path))
        sys.exit(1)

    # Set the number of symbols (i.e. the generation size in RLNC
    # terminology) and the size of a symbol in bytes
    symbols = 64
    symbol_size = 1400

    # In the following we will make an encoder factory.
    # The factories are used to build actual encoder
    encoder_factory = kodo.full_rlnc_encoder_factory_binary(symbols,
                                                            symbol_size)
    encoder = encoder_factory.build()

    sock = socket.socket(
        family=socket.AF_INET,
        type=socket.SOCK_DGRAM,
        proto=socket.IPPROTO_UDP)

    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)

    # Get the data to encode.
    f = open(os.path.expanduser(args.file_path), 'r')
    data_in = bytes(bytearray(f.read(), 'utf-8'))
    f.close()

    # Assign the data buffer to the encoder so that we can
    # produce encoded symbols
    encoder.set_symbols(data_in)

    address = (args.ip, args.port)

    print("Processing")
    counter = 0
    while args.loops < 0 or counter < args.loops:
        time.sleep(0.2)
        counter += 1
        # Generate an encoded packet
        sys.stdout.write("\tEncoding packet...")
        packet = encoder.encode()
        sys.stdout.write(" done!\n")

        # Send the packet.
        sock.sendto(packet, address)

    print("Finished")

if __name__ == "__main__":
    main()