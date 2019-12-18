# fbgfx & clock program

A framebuffer graphics library and analog clock demo ready to run on Raspberry Pi.

## Building

  make

## Running

To run the clock:
    ./fall c

or to run the clock on a secondary display:

    FBDEV=/dev/fb1 ./fall

To get help:

    ./fall

To run a demo:

    ./fall d

To run the tests:

    ./fall t

## Installing

If your system uses /dev/fb1 as a secondary display, and you wish to show a clock running:

    sudo make install
    sudo systemctl start fb1clock

If the clock is working satisfactorily, then enable for future reboots:

    sudo systemctl enable fb1clock

Else edit fb1clock.service and repeat the installation steps.

## Known Problems

The library and program are hard-coded for 16-bpp displays. This is the power-on default for the 3.5" ILI 9325 LCD serial panel I'm using on fb1.

## Components

* gfx.c - the graphics library
* fall.c - the analog face clock program. the name is misleadning because it started off as something more elaborate
* mkfnt.c - used to make the font that is embedded into fbgfx.
