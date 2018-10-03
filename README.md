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

## Known Problems

It's hard-coded for 16-bpp displays. Which happens to be the defaults for the driver for the 3.5" ILI 9325 LCD serial panel I'm using.

## What Stuff Is

* gfx.c - the graphics library
* fall.c - the analog face clock program. the name is misleadning because it started off as something more elaborate

## Stuff To Ignore

* clock.c - ncurses clock I wrote years ago, needs to be ported to fbgfx
* mkfnt.c - used to make the font that is embedded into fbgfx.
