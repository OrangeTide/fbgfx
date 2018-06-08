CFLAGS=-Wall -W
all :: clock fall mkfnt
clock : clock.c
clock : LDLIBS = -lncurses
fall : fall.c gfx.c
mkfnt : mkfnt.c
