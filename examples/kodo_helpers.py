#! /usr/bin/env python
# encoding: utf-8

# Copyright Steinwurf ApS 2015.
# Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
# See accompanying file LICENSE.rst or
# http://www.steinwurf.com/licensing

import os
import threading

try:
    import pygame
    import pygame.locals
    import pygame.gfxdraw
except:
    import sys
    print("Unable to import pygame module, please make sure it is installed.")
    sys.exit()

import numpy


class CanvasScreenEngine(object):
    """Canvas engine for displaying images to the screen."""

    def __init__(self, width, height):
        """Create CanvasScreenEngine."""
        self.screen = None
        self.running = False
        # Add a little padding
        self.size = (width + 1, height + 1)

        self.lock = threading.Lock()
        self.thread = threading.Thread(name='canvas', target=self.__start)

    def start(self):
        """Start a thread which runs the viewer logic."""
        self.thread.start()
        # Busy wait for the engine to start.
        while not self.running:
            pass

    def __start(self):
        """Start pygame and create a game loop."""
        with self.lock:
            self.running = True
            pygame.init()
            pygame.display.set_caption('Example Canvas')
            self.screen = pygame.display.set_mode(self.size, pygame.NOFRAME)
            # Paint it black
            self.screen.fill((0, 0, 0))

        while(self.running):
            with self.lock:
                pygame.display.flip()
        pygame.quit()

    def stop(self):
        """Stop the game loop and joins the thread."""
        self.running = False
        self.thread.join()

    def add_surface(self, surface, position):
        """Thread-safe way of adding a surface to the screen."""
        with self.lock:
            self.screen.blit(surface, position)


class CanvasFileEngine(object):
    """
    Canvas engine for writing images to files.

    This is especially useful if you want to create a video or view specific
    parts of the coding.

    If you want to create a video you can use the following command (tested on
    ubuntu):

      avconv -r 60 -i %07d.png -b 65536k -qscale 5 out.mp4

    """

    def __init__(self, width, height, directory):
        """Create CanvasFileEngine."""
        self.size = (width, height)
        self.files = 0
        self.directory = directory
        self.screen = pygame.Surface(self.size)
        self.dirty = False

        self.lock = threading.Lock()
        self.thread = threading.Thread(name='canvas', target=self.__start)

        if not os.path.exists(self.directory):
            os.makedirs(self.directory)
            print("Created directory: {}".format(
                os.path.abspath(self.directory)))

    def start(self):
        """Start a thread which runs the viewer logic."""
        self.thread.start()
        # Busy wait for the engine to start.
        while not self.running:
            pass

    def __start(self):
        """Start pygame and create a game loop."""
        with self.lock:
            self.running = True

        while(self.running):
            if not self.dirty:
                continue
            with self.lock:
                self.store_file()
                self.dirty = False

    def store_file(self):
        """Save canvas to file named after an incremeted integer."""
        filename = "{:07d}.png".format(self.files)
        self.files += 1
        pygame.image.save(
            self.screen, os.path.join(self.directory, filename))

    def stop(self):
        """Stop the game loop and joins the thread."""
        self.running = False
        self.thread.join()

    def add_surface(self, surface, position):
        """Thread-safe way of adding a surface to the screen."""
        with self.lock:
            self.screen.blit(surface, position)
            self.dirty = True


class StateViewer(object):
    """Class for displaying the coding coefficients."""

    def __init__(self, size, canvas, canvas_position):
        """Create StateViewer."""
        super(StateViewer, self).__init__()
        self.size = size
        self.canvas = canvas
        self.canvas_position = canvas_position

    def show_decode_state(self, code_state):
        """
        Use the coding state to print a graphical representation.

        :param code_state: A list of lists containing the symbol coefficients
        """
        surface = pygame.Surface((self.size + 1, self.size + 1))
        surface.fill((0,) * 3)
        diameter = self.size / len(code_state)
        radius = diameter / 2
        y = radius
        for symbol in code_state:
            x = radius
            for data in symbol:
                if data != 0:
                    color = (255 - (data % 255),) * 3
                    pygame.gfxdraw.circle(surface, x, y, radius, color)
                x += diameter
            y += diameter

        self.canvas.add_surface(
            surface,
            self.canvas_position)


class DecodeStateViewer(StateViewer):
    """Class for displaying the decoding coefficients."""

    def __init__(self, size, canvas, canvas_position=(0, 0)):
        """Create DecodeStateViewer."""
        super(DecodeStateViewer, self).__init__(
            size, canvas, canvas_position)

    def log_callback(self, zone, message):
        """Callback to be used with the decoder trace API."""
        # We are only interested in the decoder state.
        if zone != "decoder_state":
            return

        decode_state = []
        for line in message.split('\n'):
            if not line:
                continue
            line = line.split()

            # Add the decoding state
            decode_state.append([int(i) for i in line[2:]])

        self.show_decode_state(decode_state)


class EncodeStateViewer(StateViewer):
    """Class for displaying the encoding coefficients."""

    def __init__(self, size, canvas, canvas_position=(0, 0),
                 wrap_around=False):
        """Create EncodeStateViewer."""
        super(EncodeStateViewer, self).__init__(
            size, canvas, canvas_position)
        # if true, wrap_around, else push up.
        self.wrap_around = wrap_around
        self.__symbols = None

    def set_symbols(self, symbols):
        """
        Set the number of symbols.

        The number of symbols must be set before using the log_callback.

        :param symbols: the number of symbols.
        """
        self.__symbols = symbols
        self.state = [[] for i in range(self.__symbols)]
        self.index = 0

    def log_callback(self, zone, message):
        """Callback to be used with the encoder trace API."""
        assert self.__symbols is not None, "Symbols not set, use set_symbols"

        if zone == "symbol_index_after_produce_systematic_symbol":
            index = int(message.split(' ')[-1])
            symbol = [0 for i in range(self.__symbols)]
            symbol[index] = 1

        elif zone == "symbol_coefficients_after_produce_symbol":
            symbol = message[3:].split(' ')[:-1]
            symbol = map(int, symbol)
        else:
            return

        if self.index < self.__symbols:
            self.state[self.index] = symbol
        else:
            self.state = self.state[1:] + [symbol]
        self.index += 1

        if self.wrap_around:
            self.index = self.index % self.__symbols
            self.state[self.index - 1] = symbol

        self.show_decode_state(self.state)


class ImageViewer(object):
    """A class containing the logic for displaying an image during decoding."""

    def __init__(self, width, height, canvas, canvas_position=(0, 0)):
        """
        Construct ImageViewer object.

        :param width: the width of the picture to be displayed
        :param height: the height of the picture to be displayed
        """
        super(ImageViewer, self).__init__()
        self.size = (width, height)
        self.data_size = width * height * 3
        self.canvas = canvas
        self.canvas_position = canvas_position

    def set_image(self, image_string):
        """
        Display the provided string as an image.

        The string should be a string representation of a numpy 3d array,
        and it should be equal to or larger than width * height * 3.
        """
        image_array = numpy.fromstring(
            image_string[:self.data_size], dtype=numpy.uint8)

        # We use PIL.Image to write the image and pygame to read it. To make
        # the two compliant, we must reshape, rotate, and flip the array/image.
        image_array.shape = (self.size[1], self.size[0], 3)
        image_array = numpy.flipud(numpy.rot90(image_array, 1))

        # Create a surface
        image_surface = pygame.Surface(self.size)

        # Blit the image data to the surface
        pygame.surfarray.blit_array(image_surface, image_array)

        # Add the surface to the canvas.
        self.canvas.add_surface(
            image_surface,
            self.canvas_position)
