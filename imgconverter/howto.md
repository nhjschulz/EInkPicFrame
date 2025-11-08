# Image Converter How-To

- [Image Converter How-To](#image-converter-how-to)
  - [Image Manipulation With GIMP](#image-manipulation-with-gimp)
    - [Resizing the Image](#resizing-the-image)
    - [Dithering the Image](#dithering-the-image)
      - [Installing the Color Palette](#installing-the-color-palette)
      - [Floyd-Steinberg Dithering](#floyd-steinberg-dithering)
  - [Image Manipulation with ImageMagick](#image-manipulation-with-imagemagick)
    - [Dithering with ImageMagick](#dithering-with-imagemagick)
  - [Creating Display Raw Data](#creating-display-raw-data)
      - [Script Setup](#script-setup)
      - [Script Execution](#script-execution)

This document explains image data generation for the Waveshare 5.65inch e-Paper Module. The process has 2 major steps:

  1. Resize and dithering of the image using GIMP
  2. Converting the image into the e-Paper memory format

As an alternative to GIMP I also recommend to look at
[ImageMagick](https://imagemagick.org/index.php) as an
alternative. ImageMagick is a command line tool, allowing
to process a large number of images using scripts or
typed commands.
## Image Manipulation With GIMP

This chapter explains the steps to create images with the right size and
color palette. The GIMP version used for this How-To was 2.10.30.

### Resizing the Image

The image must have the exact size of 600x448 pixels. The can be done using
the *Scale Image* tool from menu:

    Image / Scale Image...

Enter the following values in the "Scale Image" Dialog:

![Scale](doc/scale_img_dlg.png)

Press "Scale" to resize the image. You may experiment with various "Interpolation" options in order to figure out what works best.

### Dithering the Image

The next step is to convert the image to the reduced color palette of the e-Paper Module using Floyd-Steinberg dithering.

#### Installing the Color Palette

This step needs to be done only once for a GIMP installation. It adds the e-paper color palette to the list of available palettes.

 1. Open the Palettes Dialog in Gimp from menu

        Window / Dockable Dialogs / Palettes

 2. Right click in the Palettes window and select *Import Palette ...*
    ![Import Palette](doc/import_palette.png)

    Use the palette file from [doc/E-Paper.gpl](doc/E-Paper.gpl).

Now the palette is available in the palettes list:

![Imported Palette](doc/loaded_palette.png)

Note: This palette has 8-Colors defined, but the display only supports  7. But there is indeed an additional one for "cleaning". It means no ink visible and is intended to wipe the pixels between changing images. It  avoids ghosting, but is also a color at the end. The following picture shows the 8 palette colors:

![Colors](doc/colors.png)

#### Floyd-Steinberg Dithering

This Floyed-Steinberg dithering algorithm converts images to look still good using a reduced color set. In GIMP this algorithm gets applied by converting an image to *Indexed Colors Mode*. The pixels then contain an index into a color palette instead of RGB values.

  1. Switch the image to indexed mode by using menu

          Image / Mode / Indexed...

  2. The *Indexed Color Conversion* dialog appears

     ![Indexed Colors Dialog](doc/indexed_mode.png)

  3. Select our *Custom E-paper Palette"

  4. Make sure that "Remove unused and duplicate colors..." is NOT checked. We need to keep the palette indexes as they are.

  5. Select a dithering algorithm. You may try different ones to see what works best in your case.

  6. Export the result as Windows BMP file using menu

          File / Export As...

  Select "Windpws BMP Image" from the "Select File Type" list.

## Image Manipulation with ImageMagick

ImageMagick is like a swiss army knife for image manipulation.
It is a commandline tool with a wealth of options.I don't
recommend to change the image size to 600x448 by script
(also it is perfectly doable with this tool). Each image has
different resize needs (What part of it, what aspect ratio ...).
But dithering is incredibly easy to do on a single or set of files.

### Dithering with ImageMagick

ImageMagick support the -dither option, which is
basically all you need. The only obstacle is that you
need to pass the color palette via an image using the palette
already. This smells a little like a chicken or egg issue. You
can create such an image using the GIMP method described in this
file, or use the [palette colors image](doc/colors.png). It is
already an indexed picture with the right colors.

The call to dither an image is then this:

    magick convert input.jpg -dither FloydSteinberg -remap colors.png output.png

You need to specify the full path to colors.png, unless it is in
the current working directory as well.

The following command will dither **all** files with.jpg extension
into Png files with the same filename:

    magick convert *.jpg -dither FloydSteinberg -remap colors.png -set filename:base "%[basename]" "%[filename:base].png"

## Creating Display Raw Data

The proper sized and dithered image must be converted one more time into the
display memory raw format. This is a simple byte stream of color index values.
Each byte holds two index values, one in the upper 4 bit, the following in the
lower 4 bit.

A python script for converting the images into this raw format is available
[here](./epdconv.py).

#### Script Setup

The script needs a Python 3.x environment with
[Pillow](https://pillow.readthedocs.io/en/stable/index.html). Pillow can be
installed using pip:

    python -m pip install --upgrade pip
    python -m pip install --upgrade Pillow

#### Script Execution

Simply call the Python script with one or more image files as arguments.

    python epdconv.py img000.bmp imgage001.bmp ...

It will create the raw display image files with the same filename and
a ".epd" (E-Paper Display) extension on success.

The script does some sanity checks regarding size and color index mode.
Unfit image will be skipped with a warning.
