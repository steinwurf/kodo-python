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

import kodo

import math
import os
import time
import random
import sys

try:
    import pygame
    import pygame.locals
    import pygame.gfxdraw
except:
    import sys

    print("Unable to import pygame module, please make sure it is installed.")
    sys.exit()

try:
    import numpy
except:
    import sys

    print("Unable to import numpy module, please make sure it is installed.")
    sys.exit()

while True:
    try:
        import Image

        break
    except ImportError:
        pass

    try:
        from PIL import Image

        break
    except ImportError:
        pass

    print("Unable to import Image/PIL.Image module.")
    sys.exit()


def main():

    if "--dry-run" in sys.argv:
        sys.exit(0)

    # Get directory of this file
    directory = os.path.dirname(os.path.realpath(__file__))

    # The name of the file to use for the test
    filename = "lena.jpg"

    # Open the image convert it to RGB and get the height and width
    image = Image.open(os.path.join(directory, filename)).convert("RGB")

    # Create the canvas
    pygame.init()
    pygame.display.set_caption("Example Canvas")
    canvas = pygame.display.set_mode(image.size, pygame.NOFRAME)

    # Paint it black
    canvas.fill((0, 0, 0))

    # Create the image surface
    surface = pygame.Surface(image.size)

    # Pick a symbol size (image.width * 3 will create a packet for each
    # horizontal line of the image, that is three bytes per pixel (RGB))
    symbol_bytes = image.width * 3

    # Based on the size of the image and the symbol size, calculate the number
    # of symbols needed for containing the image in a single generation.
    symbols = int(math.ceil(image.width * image.height * 3.0 / symbol_bytes))

    field = kodo.FiniteField.binary16

    encoder = kodo.block.Encoder(field)

    encoder.configure(symbols, symbol_bytes)

    decoder = kodo.block.Decoder(field)

    decoder.configure(symbols, symbol_bytes)

    generator = kodo.block.generator.RandomUniform(field)
    generator.configure(encoder.symbols)

    symbol = bytearray(encoder.symbol_bytes)

    coefficients = bytearray(generator.max_coefficients_bytes)

    # Create a bytearray from the image to use in the encoding (only pick the
    # data we have room for).
    data_in = bytearray(image.tobytes()[-encoder.block_bytes :])

    # Set the converted image data
    encoder.set_symbols_storage(data_in)

    # Define the data_out bytearray where the symbols should be decoded
    # This bytearray must not go out of scope while the decoder exists!
    data_out = bytearray(decoder.block_bytes)
    decoder.set_symbols_storage(data_out)

    systematic_index = 0
    loss_probability = 40

    while not decoder.is_complete():
        if encoder.rank > systematic_index:
            index = systematic_index
            systematic_index += 1
            encoder.encode_systematic_symbol(symbol, index)

            if random.randint(0, 100) >= loss_probability:
                decoder.decode_systematic_symbol(symbol, index)
        else:
            generator.generate(coefficients)
            encoder.encode_symbol(symbol, coefficients)

            if random.randint(0, 100) >= loss_probability:
                decoder.decode_symbol(symbol, coefficients)

        # The data_out buffer is continuously updated
        image_array = numpy.frombuffer(data_out, dtype=numpy.uint8)

        # We use PIL.Image to write the image and pygame to read it. To make
        # the two compliant, we must reshape, rotate, and flip the array/image.
        image_array.shape = (image.height, image.width, 3)
        image_array = numpy.flipud(numpy.rot90(image_array, 1))

        # Blit the image data to the surface
        pygame.surfarray.blit_array(surface, image_array)

        # Add the surface to the canvas.
        canvas.blit(surface, (0, 0))
        pygame.display.flip()
        time.sleep(0.02)

    time.sleep(2)

    # Check we properly decoded the data
    if data_out[: len(data_in)] == data_in:
        print("Data decoded correctly")
    else:
        print("Unexpected failure to decode please file a bug report :)")
        sys.exit(1)


if __name__ == "__main__":
    main()
