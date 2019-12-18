CFLAGS=-Wall -W
all :: fall mkfnt
clean :: ; $(RM) fall mkfnt
fall : fall.c gfx.c
fall : LDLIBS = -lm
mkfnt : mkfnt.c
.PHONY : gitzip gittar all clean install
gitzip : ; git archive --format=zip -o ../gfx-$(shell date +%Y%m%d).zip --prefix=gfx/ HEAD
gittar : ; git archive --format=tar.gz -o ../gfx-$(shell date +%Y%m%d).tar.gz --prefix=gfx/ HEAD
install : fall fb1clock.service
	install -t /usr/local/bin fall
	install -m 644 -t /etc/systemd/system fb1clock.service
