#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2014.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import socket
import time
import sys

import argparse
import json

import kodo


def main():
    """UDP Server/Client for sending and receiving files."""
    parser = argparse.ArgumentParser(description=main.__doc__)

    parser.add_argument(
        '--settings-port',
        type=int,
        help='Settings port on the server.',
        default=41001)

    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Run without network use, for testing purposes')

    subparsers = parser.add_subparsers(
        dest='role', help='help for subcommand')

    server_parser = subparsers.add_parser(
        'server',
        description="UDP server for sending and receiving files.",
        help='Start a server')

    client_parser = subparsers.add_parser(
        'client',
        description="UDP client for sending and receiving files.",
        help='Start a client')

    client_parser.add_argument(
        '--server-ip',
        type=str,
        help='IP address of the server.',
        default='127.0.0.1')

    client_parser.add_argument(
        '--client-control-port',
        type=int,
        help='control port on the client side, used for signaling.',
        default=41003)

    client_parser.add_argument(
        '--server-control-port',
        type=int,
        help='control port on the server side, used for signaling.',
        default=41005)

    client_parser.add_argument(
        '--data-port',
        type=int,
        help='port used for data transmission.',
        default=41011)

    client_parser.add_argument(
        '--direction',
        help='direction of data transmission',
        choices=[
            'client_to_server',
            'server_to_client',
            'client_to_server_to_client'],
        default='client_to_server_to_client')

    client_parser.add_argument(
        '--symbols',
        type=int,
        help='number of symbols in each generation/block.',
        default=64)

    client_parser.add_argument(
        '--symbol-size',
        type=int,
        help='size of each symbol, in bytes.',
        default=1400)

    client_parser.add_argument(
        '--max-redundancy',
        type=float,
        help='maximum amount of redundancy to be sent, in percent.',
        default=200)

    client_parser.add_argument(
        '--timeout',
        type=float,
        help='timeout used for various sockets, in seconds.',
        default=0.2)

    # We have to use syg.argv for the dry-run parameter, because parse_args()
    # will fail with a "too few arguments" error if this is the only parameter
    if '--dry-run' in sys.argv:
        sys.exit(0)

    args = parser.parse_args()

    if args.role == 'client':
        client(args)
    else:
        server(args)


def server(args):
    """Init server."""
    settings_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    settings_socket.bind(('', args.settings_port))

    # Wait for settings connections
    print("Server running, press Ctrl-C or Ctrl-Break to stop.")
    while True:
        data, address = receive(settings_socket, 1024)
        try:
            settings = json.loads(data)
        except Exception:
            print("Settings message invalid.")
            continue

        settings['role'] = 'server'
        settings['client_ip'] = address[0]

        if settings['direction'] == 'server_to_client':
            send_data(settings, 'server')
        elif settings['direction'] == 'client_to_server':
            receive_data(settings, 'server')
        else:
            print("Invalid direction.")
            continue


def client(args):
    """Init client."""
    if args.symbol_size > 65000:
        print("Resulting packets too big, reduce symbol size")
        return

    settings = vars(args)
    direction = settings.pop('direction')

    # Note: "server>client>server" matches both cases.
    if 'server_to_client' in direction:
        settings['direction'] = 'server_to_client'
        receive_data(settings, 'client')

    if 'client_to_server' in direction:
        settings['direction'] = 'client_to_server'
        send_data(settings, 'client')


def send_data(settings, role):
    """Send data to the other node."""
    # Setup kodo encoder
    encoder = kodo.RLNCEncoder(
        field=kodo.field.binary,
        symbols=settings['symbols'],
        symbol_size=settings['symbol_size'])

    data_in = bytearray(os.urandom(encoder.block_size()))
    encoder.set_symbols_storage(data_in)

    send_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    control_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    control_socket.settimeout(0.00000000000000000001)

    if role == 'client':
        address = (settings['server_ip'], settings['data_port'])
        send_settings(settings)
        control_socket.bind(('', settings['client_control_port']))
    else:  # server
        address = (settings['client_ip'], settings['data_port'])
        server_address = (
            settings['server_ip'],
            settings['client_control_port'])
        control_socket.bind(('', settings['server_control_port']))
        send(send_socket, "settings OK, sending", server_address)

    sent = 0
    start = time.clock()
    end = None
    while sent < settings['symbols'] * settings['max_redundancy'] / 100:
        packet = encoder.produce_payload()
        send(send_socket, packet, address)
        sent += 1

        try:
            control_socket.recv(1024)
            if end is None:
                end = time.clock()
            break
        except socket.timeout:
            continue

    # if no ack was received we sent all packets
    if end is None:
        end = time.clock()

    control_socket.close()

    size = encoder.block_size() * (float(sent) / settings['symbols'])
    microseconds = 1e6 * (end - start)
    print("Sent {0} packets, {1} kB in {2:.0f} microseconds at "
          "{3:.2f} Mbit/s.".format(sent, size / 1000, microseconds,
                                   size * 8 / microseconds))


def receive_data(settings, role):
    """Receive data from the other node."""
    # Setup kodo decoder
    decoder = kodo.RLNCDecoder(
        field = kodo.field.binary,
        symbols=settings['symbols'],
        symbol_size=settings['symbol_size'])

    data_out = bytearray(decoder.block_size())
    decoder.set_symbols_storage(data_out)

    send_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Set receiving sockets
    data_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    data_socket.settimeout(settings['timeout'])
    data_socket.bind(('', settings['data_port']))

    if role == 'client':
        address = (settings['server_ip'], settings['server_control_port'])
        send_settings(settings)
    else:  # server
        address = (settings['client_ip'], settings['client_control_port'])
        send(send_socket, "settings OK, receiving", address)

    # Decode coded packets
    received = 0
    start = time.clock()
    end = None
    while 1:
        try:
            packet = data_socket.recv(settings['symbol_size'] + 100)

            if not decoder.is_complete():
                decoder.consume_payload(bytearray(packet))
                received += 1

            if decoder.is_complete():
                if end is None:
                    end = time.clock()  # stopping time once
                send(send_socket, "Stop sending", address)

        except socket.timeout:
            break  # no more data arriving

    # in case we did not complete
    if end is None:
        end = time.clock()

    data_socket.close()

    if not decoder.is_complete():
        print("Decoding failed")

    size = decoder.block_size() * (float(received) / settings['symbols'])
    microseconds = 1e6 * (end - start)
    print("Received {0} packets, {1} kB in {2:.0f} microseconds at "
          "{3:.2f} Mbit/s.".format(received, size / 1000, microseconds,
                                   decoder.block_size() * 8 / microseconds))


def send_settings(settings):
    """
    Send settings to server.

    This function blocks until confirmation received that settings
    was correctly received.
    """
    control_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    control_socket.settimeout(settings['timeout'])
    control_socket.bind(('', settings['client_control_port']))

    send_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    send_address = (settings['server_ip'], settings['settings_port'])

    message = json.dumps(settings)
    ack = None
    address = ''
    while ack is None:
        # Send settings
        send(send_socket, message, send_address)
        # Waiting for respons
        try:
            ack, address = receive(control_socket, 1024)  # Server ack
        except socket.timeout:
            print("Timeout - server not responding to settings.")

    control_socket.close()


def send(socket, message, address):
    """
    Send message to address using the provide socket.

    Works for both python2 and python3

    :param socket: The socket to use.
    :param message: The message to send.
    :param address: The address to send to.
    """
    # Convert str message to bytes on Python 3+
    if sys.version_info[0] > 2 and isinstance(message, str):
        message = bytes(message, 'utf-8')

    socket.sendto(message, address)


def receive(socket, number_of_bytes):
    """
    Receive an amount of bytes.

    Works for both python2 and python3

    :param socket: The socket to use.
    :param number_of_bytes: The number of bytes to receive.
    """
    data, address = socket.recvfrom(number_of_bytes)
    if sys.version_info[0] > 2 and isinstance(data, bytes):
        return str(data, 'utf-8'), address

    return data, address


if __name__ == "__main__":
    main()
