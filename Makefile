CFLAGS=-Wall -W
all :: clock fall
clock : clock.c
clock : LDLIBS = -lncurses
fall : fall.c gfx.c
