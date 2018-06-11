CFLAGS=-Wall -W
all :: clock fall mkfnt
clock : clock.c
clock : LDLIBS = -lncurses
fall : fall.c gfx.c
fall : LDLIBS = -lm
mkfnt : mkfnt.c
.PHONY : gitzip gittar
gitzip : ; git archive --format=zip -o ../gfx-$(shell date +%Y%m%d).zip --prefix=gfx/ HEAD
gittar : ; git archive --format=tar.gz -o ../gfx-$(shell date +%Y%m%d).tar.gz --prefix=gfx/ HEAD
