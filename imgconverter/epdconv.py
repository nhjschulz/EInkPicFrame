"""
epdconv - Convert images into E-Paper raw format


This module reads images and converts them into the raw format expected
by the Waveshare 5.64 inch 7 color E-Paper display. The tool expects
the image to be already proper sized and dithered for the few colors
supported by the display. Unfit images are skipped.

The output format is really simple. It is a bytestrem where each nibble
represent a color index of 0..7 into the display fixed color palette.

The following sequence would plot 8 pixels with color 0..7:

   0x01, 0x23, 0x45, 0x67

pixel(0,0) is the upper left, pixel(599, 447) the lower right one. 

The converter treats every argument on the command line as an image 
file name. It iterates over the files and writes the converted  result
into a file with the same basename, but extension "epd" - for e paper display.

Example:

        $ python epdconv.py  img000.bmp img001.bmp

This will create img00.epd and img001.epd on success.
"""

import os
from PIL import Image

# required image size
WIDTH=600
HEIGHT=448
COLORS=8

def convert(filename, im : Image):
    "convert image into epd  format and write it into a epd file"

    try:
        outname = os.path.splitext(filename)[0] + '.epd'
        imgfile = open(outname, "wb")

        data = bytearray()

        for y in range(0, HEIGHT):
            for x in range(0, WIDTH, 2):
                ph = im.getpixel((x, y))
                pl = im.getpixel((x + 1, y))
                if ((COLORS <= ph) or (COLORS <= pl)) :
                    raise Exception("Color index out of range")

                val = ((ph << 4) | pl)
                data.append(val)

        imgfile.write(data) 
        imgfile.close()
        print('Created epd image {}'.format(outname))

    except Exception as e: 
        print('{} : {}'.format(outname, e))


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) <= 1:
        print('usage {}: image_file [image_file]...'.format(sys.argv[0]))
        exit(1)

    for infile in sys.argv[1:]:
        try:
            with Image.open(infile) as im:
                if ((WIDTH != im.size[0]) and (HEIGHT != im.size[1])):
                    print('{}: wrong image size {}'.format(infile, im.size))
                    continue
                
                if (not isinstance(im.getpixel((0,0)), int)):
                    print('{}: wrong color format, need palette indexed'.format(infile))
                    continue

                convert(infile, im)

        except OSError:
            pass


