#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "gfx.h"

/******************************************************************************/

#define MIN(a, b) ({ \
	__typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })
#define MAX(a, b) ({ \
	__typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

/******************************************************************************/

static const unsigned font_w = 8, font_h = 8;
static const unsigned char font_base = 0x20, font_glyphs = 95;
static const unsigned char font[95 * 8] = {
	/* ' '  */ 0, 0, 0, 0, 0, 0, 0, 0,
	/* '!'  */ 24, 24, 24, 24, 0, 24, 24, 0,
	/* '"'  */ 108, 108, 72, 0, 0, 0, 0, 0,
	/* '#'  */ 108, 108, 254, 108, 254, 108, 108, 0,
	/* '$'  */ 16, 124, 208, 124, 22, 124, 16, 0,
	/* '%'  */ 98, 102, 12, 24, 48, 102, 70, 0,
	/* '&'  */ 56, 108, 104, 118, 220, 204, 118, 0,
	/* '''  */ 24, 24, 48, 0, 0, 0, 0, 0,
	/* '('  */ 12, 24, 48, 48, 48, 24, 12, 0,
	/* ')'  */ 48, 24, 12, 12, 12, 24, 48, 0,
	/* '*'  */ 0, 108, 56, 254, 56, 108, 0, 0,
	/* '+'  */ 0, 24, 24, 126, 24, 24, 0, 0,
	/* ','  */ 0, 0, 0, 0, 0, 24, 24, 16,
	/* '-'  */ 0, 0, 0, 126, 0, 0, 0, 0,
	/* '.'  */ 0, 0, 0, 0, 0, 24, 24, 0,
	/* '/'  */ 2, 6, 12, 24, 48, 96, 64, 0,
	/* '0'  */ 60, 102, 110, 118, 102, 102, 60, 0,
	/* '1'  */ 24, 24, 56, 24, 24, 24, 60, 0,
	/* '2'  */ 124, 6, 6, 60, 96, 96, 126, 0,
	/* '3'  */ 124, 6, 6, 28, 6, 6, 124, 0,
	/* '4'  */ 102, 102, 102, 126, 6, 6, 6, 0,
	/* '5'  */ 126, 96, 96, 124, 6, 6, 124, 0,
	/* '6'  */ 62, 96, 96, 124, 102, 102, 60, 0,
	/* '7'  */ 126, 6, 12, 24, 24, 24, 24, 0,
	/* '8'  */ 60, 102, 102, 60, 102, 102, 60, 0,
	/* '9'  */ 60, 102, 102, 62, 6, 6, 124, 0,
	/* ':'  */ 0, 24, 24, 0, 24, 24, 0, 0,
	/* ';'  */ 0, 0, 24, 24, 0, 24, 24, 16,
	/* '<'  */ 12, 24, 48, 96, 48, 24, 12, 0,
	/* '='  */ 0, 0, 126, 0, 126, 0, 0, 0,
	/* '>'  */ 48, 24, 12, 6, 12, 24, 48, 0,
	/* '?'  */ 60, 102, 6, 28, 24, 0, 24, 0,
	/* '@'  */ 60, 102, 110, 110, 110, 96, 62, 0,
	/* 'A'  */ 60, 102, 102, 126, 102, 102, 102, 0,
	/* 'B'  */ 124, 102, 102, 124, 102, 102, 124, 0,
	/* 'C'  */ 60, 102, 96, 96, 96, 102, 60, 0,
	/* 'D'  */ 124, 102, 102, 102, 102, 102, 124, 0,
	/* 'E'  */ 126, 96, 96, 120, 96, 96, 126, 0,
	/* 'F'  */ 126, 96, 96, 120, 96, 96, 96, 0,
	/* 'G'  */ 60, 102, 96, 110, 102, 102, 60, 0,
	/* 'H'  */ 102, 102, 102, 126, 102, 102, 102, 0,
	/* 'I'  */ 126, 24, 24, 24, 24, 24, 126, 0,
	/* 'J'  */ 14, 6, 6, 6, 6, 102, 60, 0,
	/* 'K'  */ 102, 108, 120, 112, 120, 108, 102, 0,
	/* 'L'  */ 96, 96, 96, 96, 96, 96, 126, 0,
	/* 'M'  */ 198, 238, 254, 214, 198, 198, 198, 0,
	/* 'N'  */ 102, 118, 126, 110, 102, 102, 102, 0,
	/* 'O'  */ 60, 102, 102, 102, 102, 102, 60, 0,
	/* 'P'  */ 124, 102, 102, 124, 96, 96, 96, 0,
	/* 'Q'  */ 60, 102, 102, 102, 110, 110, 62, 0,
	/* 'R'  */ 124, 102, 102, 124, 102, 102, 102, 0,
	/* 'S'  */ 62, 96, 96, 60, 6, 6, 124, 0,
	/* 'T'  */ 126, 24, 24, 24, 24, 24, 24, 0,
	/* 'U'  */ 102, 102, 102, 102, 102, 102, 60, 0,
	/* 'V'  */ 102, 102, 102, 102, 60, 60, 24, 0,
	/* 'W'  */ 198, 198, 198, 214, 254, 238, 198, 0,
	/* 'X'  */ 102, 102, 60, 24, 60, 102, 102, 0,
	/* 'Y'  */ 102, 102, 102, 60, 24, 24, 24, 0,
	/* 'Z'  */ 126, 6, 12, 24, 48, 96, 126, 0,
	/* '['  */ 60, 48, 48, 48, 48, 48, 60, 0,
	/* '\\' */ 64, 96, 48, 24, 12, 6, 2, 0,
	/* ']'  */ 60, 12, 12, 12, 12, 12, 60, 0,
	/* '^'  */ 16, 56, 108, 0, 0, 0, 0, 0,
	/* '_'  */ 0, 0, 0, 0, 0, 0, 0, 255,
	/* '`'  */ 24, 24, 8, 0, 0, 0, 0, 0,
	/* 'a'  */ 0, 0, 60, 6, 62, 102, 62, 0,
	/* 'b'  */ 96, 96, 124, 102, 102, 102, 124, 0,
	/* 'c'  */ 0, 0, 60, 102, 96, 102, 60, 0,
	/* 'd'  */ 6, 6, 62, 102, 102, 102, 62, 0,
	/* 'e'  */ 0, 0, 60, 102, 124, 96, 62, 0,
	/* 'f'  */ 14, 24, 24, 60, 24, 24, 60, 0,
	/* 'g'  */ 0, 0, 62, 102, 102, 62, 6, 124,
	/* 'h'  */ 96, 96, 124, 102, 102, 102, 102, 0,
	/* 'i'  */ 24, 0, 24, 24, 24, 24, 24, 0,
	/* 'j'  */ 24, 0, 24, 24, 24, 24, 24, 112,
	/* 'k'  */ 96, 96, 102, 108, 120, 108, 102, 0,
	/* 'l'  */ 48, 48, 48, 48, 48, 48, 28, 0,
	/* 'm'  */ 0, 0, 204, 254, 214, 198, 198, 0,
	/* 'n'  */ 0, 0, 124, 102, 102, 102, 102, 0,
	/* 'o'  */ 0, 0, 60, 102, 102, 102, 60, 0,
	/* 'p'  */ 0, 0, 124, 102, 102, 124, 96, 96,
	/* 'q'  */ 0, 0, 62, 102, 102, 62, 6, 6,
	/* 'r'  */ 0, 0, 54, 56, 48, 48, 48, 0,
	/* 's'  */ 0, 0, 62, 96, 60, 6, 124, 0,
	/* 't'  */ 24, 24, 24, 60, 24, 24, 12, 0,
	/* 'u'  */ 0, 0, 102, 102, 102, 102, 60, 0,
	/* 'v'  */ 0, 0, 102, 102, 60, 60, 24, 0,
	/* 'w'  */ 0, 0, 198, 198, 214, 254, 236, 0,
	/* 'x'  */ 0, 0, 102, 60, 24, 60, 102, 0,
	/* 'y'  */ 0, 0, 102, 102, 102, 62, 6, 124,
	/* 'z'  */ 0, 0, 126, 12, 24, 48, 126, 0,
	/* '{'  */ 28, 48, 48, 96, 48, 48, 28, 0,
	/* '|'  */ 24, 24, 24, 24, 24, 24, 24, 0,
	/* '}'  */ 56, 12, 12, 6, 12, 12, 56, 0,
	/* '~'  */ 50, 76, 0, 0, 0, 0, 0, 0,
};

/******************************************************************************/

static unsigned char *pixels;
static size_t pixels_len, rowbytes;
static unsigned xres, yres;

/******************************************************************************/

static gfx_color_t
rgb565(unsigned char r, unsigned char g, unsigned char b)
{
	return RGB565(r, g, b);
}

static void *
pixel_address(unsigned short x, unsigned short y)
{
	return (uint8_t*)pixels + y * rowbytes + x * 2;
}

static void
putpixel16(unsigned short x, unsigned short y, gfx_color_t c)
{
	gfx_write16(c, pixel_address(x, y));
}

static void
glyph16(unsigned short x, unsigned short y, gfx_color_t fg, gfx_color_t bg, uint8_t ch)
{
	/* expand 2 bits into 2 16-bit pixels */
	static const uint32_t tab[4] = {
		0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff,
       	};
	const uint8_t *c = font + font_h * ((font_w + 7) / 8) * (ch - font_base);
	unsigned w = font_w, h = font_h;
	uint32_t fgx, bgx, eorx;
	uint32_t *out;
	unsigned row, col;
	size_t adj;

	fgx = fg;
	bgx = bg;
	fgx |= (fgx << 16);
	bgx |= (bgx << 16);
	eorx = fgx ^ bgx;

	out = pixel_address(x, y);
	adj = rowbytes / 4 - (w / 2); // TODO: calculate this properly
	for (row = 0; row < h; row++) {
		for (col = 0; col < w; col += 8) {
			gfx_write32((tab[*c >> 6] & eorx) ^ bgx, out++);
			gfx_write32((tab[*c >> 4 & 3] & eorx) ^ bgx, out++);
			gfx_write32((tab[*c >> 2 & 3] & eorx) ^ bgx, out++);
			gfx_write32((tab[*c & 3] & eorx) ^ bgx, out++);
			c++;
		}
		out += adj;
	}
}

//
// TODO: make operations that honor a scissor test
//   clip_hline16()
//   clip_putpixel16()
//

static void
hline16(unsigned short x0, unsigned short x1, unsigned short y, gfx_color_t c)
{
	uint32_t *out;
	uint32_t px;
	unsigned short x = x0;

	if (x & 1) {
		putpixel16(x, y, c);
		x++;
	}

	px = c;
	px |= px << 16;
	out = pixel_address(x, y);
	if ((intptr_t)out & 1)
		return;
	while (x < x1) {
		gfx_write32(px, out++);
		x += 2;
	}

	if (!(x1 & 1))
		putpixel16(x1, y, c);
}

static void
circle16(unsigned short x, unsigned short y, unsigned short r, gfx_color_t c)
{
	int i = r, j = 0;
	int err = 1 - i;

	while (i >= j) {
		putpixel16(x + i, y + j, c);
		putpixel16(x - i, y + j, c);
		putpixel16(x + i, y - j, c);
		putpixel16(x - i, y - j, c);
		putpixel16(x + j, y + i, c);
		putpixel16(x - j, y + i, c);
		putpixel16(x + j, y - i, c);
		putpixel16(x - j, y - i, c);
		j++;
		err += (err < 0) ?  2 * j + 1 : 2 * (j - --i + 1);
	}
}

static void
filledcircle16(unsigned short x, unsigned short y, unsigned short r, gfx_color_t c)
{
	int i = r, j = 0;
	int err = 1 - i;
	unsigned short a, b;

	while (i >= j) {
		a = x - i;
		b = x + i;
		hline16(a, b, y - j, c);
		if (j)
			hline16(a, b, y + j, c);
		j++;
		if (err < 0) {
			err += 2 * j + 1;
		} else {
			if (i >= j) {
				a = x - j;
				b = x + j;
				hline16(a, b, y - i, c);
				if (i)
					hline16(a, b, y + i, c);
			}
			err += 2 * (j - --i + 1);
		}
	}
}

/* fast line.
 * TODO: test for correctness.
 * TODO: benchmark.
 * TODO: has a bug if (x0, y0) > (x1, y1)
 */
static void
line16(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, gfx_color_t c)
{
	int maj, minor;
	int ymajor;
	int32_t delta;
	unsigned j;
	unsigned short x, y;

	// HACK to swap (x0, y0) and (x1, y1)
	if (0) {
		unsigned short t;
		t = x0;
		x0 = x1;
		x1 = t;
		t = y0;
		y0 = y1;
		y1 = t;
	}

	maj = (int)x1 - (int)x0;
	minor = (int)y1 - (int)y0;
	x = x0;
	y = y0;

	ymajor = abs(maj) < abs(minor);
	if (ymajor) {
		int tmp = maj;
		maj = minor;
		minor = tmp;
	}

	if (maj)
		delta = (minor << 16) / maj;
	else
		delta = 0;

	if (ymajor) {
		if (maj > 0) {
			maj += y;
			for (j = 0x8000 + ((unsigned)x << 16); y <= maj; y++) {
				putpixel16(j >> 16, y, c);
				j += delta;
			}
		} else {
			maj += y;
			for (j = 0x8000 + ((unsigned)x << 16); y >= maj; y--) {
				putpixel16(j >> 16, y, c /* gfx_rgb(0,0,255) */);
				j -= delta;
			}
		}
	} else {
		if (maj > 0) {
			maj += x;
			for (j = 0x8000 + ((unsigned)y << 16); x <= maj; x++) {
				putpixel16(x, j >> 16, c /* gfx_rgb(0,255,0) */);
				j += delta;
			}
		} else {
			maj += x;
			for (j = 0x8000 + ((unsigned)y << 16); x >= maj; x--) {
				putpixel16(x, j >> 16, c /* gfx_rgb(255,0, 0) */);
				j -= delta;
			}
		}
	}
}

/******************************************************************************/

void (*gfx_draw_glyph)(unsigned short x, unsigned short y, gfx_color_t fg, gfx_color_t bg, uint8_t ch);
void (*gfx_draw_hline)(unsigned short x0, unsigned short x1, unsigned short y, gfx_color_t c);
void (*gfx_draw_circle)(unsigned short x, unsigned short y, unsigned short r, gfx_color_t c);
void (*gfx_fill_circle)(unsigned short x, unsigned short y, unsigned short r, gfx_color_t c);

gfx_color_t (*gfx_rgb)(unsigned char r, unsigned char g, unsigned char b);

int
draw_init(int bpp)
{
	switch (bpp) {
	case 16:
		gfx_draw_glyph = glyph16;
		gfx_rgb = rgb565;
		gfx_draw_hline = hline16;
		gfx_draw_circle = circle16;
		gfx_fill_circle = filledcircle16;
		return 0;
	}

	return -1;
}

/******************************************************************************/

static gfx_color_t pal16[16];

void
pal_init(void)
{
	pal16[0] = gfx_rgb(0,0,0);
	pal16[1] = gfx_rgb(0,0,170);
	pal16[2] = gfx_rgb(0,170,0);
	pal16[3] = gfx_rgb(0,170,170);
	pal16[4] = gfx_rgb(170,0,0);
	pal16[5] = gfx_rgb(170,0,170);
	pal16[6] = gfx_rgb(170,85,0);
	pal16[7] = gfx_rgb(170,170,170);
	pal16[8] = gfx_rgb(85,85,85);
	pal16[9] = gfx_rgb(85,85,255);
	pal16[10] = gfx_rgb(85,255,85);
	pal16[11] = gfx_rgb(85,255,255);
	pal16[12] = gfx_rgb(255,85,85);
	pal16[13] = gfx_rgb(255,85,255);
	pal16[14] = gfx_rgb(255,255,85);
	pal16[15] = gfx_rgb(255,255,255);
}

static void
draw_string(unsigned short x, unsigned short y, gfx_color_t fg, gfx_color_t bg, const char *s)
{
	unsigned short i;

	
	for (i = x; *s; i += font_w) {
		if (i > xres) {
			y += font_h;
			i = x;	
		}
		gfx_draw_glyph(i, y, fg, bg, *s++);
	}
}

static void
random_text(void)
{
	gfx_color_t fg, bg;
	unsigned short x, y;

	/* fill the screen with random letters of random color */
	for (y = 0; y <= yres - font_h; y += 8) {
		for (x = 0; x <= xres - font_w; x += 8) {
			unsigned char ch = (rand() % font_glyphs) + font_base;
			fg = pal16[rand() % 16];
			bg = pal16[rand() % 16];

			gfx_draw_glyph(x, y, fg, bg, ch);
		}
	}
}

void
iso_maze(void)
{
	gfx_color_t fg, bg;
	unsigned short x, y;

	fg = pal16[0];
	bg = pal16[7];
	for (y = 0; y <= yres - font_h; y += 8) {
		for (x = 0; x <= xres - font_w; x += 8) {

			gfx_draw_glyph(x, y, fg, bg, rand() % 2 ? '\\' : '/');
		}
	}
}

/* demo of several draw commands */
void
demo16(void)
{
	gfx_color_t fg, bg;
	unsigned i;
	unsigned short r;
	unsigned short x, y;

	iso_maze();

	/* draw some lines on top of a black circle */
	filledcircle16(xres / 2, yres / 2, MAX(xres, yres) / 5, pal16[0]);
	for (i = 1; i < 15; i++) {
		line16(xres / 3, yres / 3,
			xres - xres / 3 - i * 3, yres - yres / 3, pal16[i]);
		line16(xres / 3, yres / 3,
			xres - xres / 3, yres - yres / 3 - i * 2, pal16[i]);
	}

	/* draw a fan in the corner */
	r = MIN(yres, xres) / 3;
	for (i = 1; i < r; i += 10) {
		line16(xres - r, yres - r, xres - i, yres - 1, pal16[15]);
		line16(xres - r, yres - r, xres - 1, yres - i, pal16[15]);
	}

	/* draw a red triangle thing */
	r = xres / 4;
	for (i = 0; i < r; i++) {
		gfx_draw_hline(r + i, xres - 1 - r - i, 40 + i, pal16[12]);
	}

	/* draw a 2x2 block of black on white letters */
	bg = pal16[15];
	fg = pal16[0];
	x = 233; // rand() % (xres - font_w * 2);
	y = 57; // rand() % (yres - font_h * 2);
	glyph16(x, y, fg, bg, 'A');
	glyph16(x + font_w, y, fg, bg, 'B');
	glyph16(x, y + font_h, fg, bg, 'C');
	glyph16(x + font_w, y + font_h, fg, bg, 'D');

	/* draw a blue circle in the upper left corner */
	r = MIN(xres, yres) / 5;
	filledcircle16(r, r, r, pal16[9]);
	circle16(xres / 2, yres / 2, r, pal16[14]);
}

/* an animated clock */
void
clock16(void)
{
	float theta;
	time_t now;
	struct tm tm;
	int ox, oy, cx, cy;
	int r;
	int i;
	char date[40];

	ox = xres / 2;
	oy = yres / 2;

	r = MIN(xres, yres) * 2 / 3 / 2;

	while (1) {
		time(&now);
		tm = *localtime(&now);

		gfx_clear(pixels, pixels_len);

		strftime(date, sizeof(date), "%Y.%m.%d", &tm);
		draw_string(0, 0, pal16[8], pal16[15], date);

		/* draw jewels */
		for (i = 0; i < 12; i++) {
			theta = 2 * M_PI * (i / 12.) - M_PI / 2;
			cx = round(r * cos(theta));
			cy = round(r * sin(theta));
			filledcircle16(ox + cx, oy + cy, r / 18,
					i == 0 ? pal16[14] : pal16[3]);
		}

		/* second hand */
		theta = 2 * M_PI * (tm.tm_sec / 60.) - M_PI / 2;
		cx = round(r * cos(theta));
		cy = round(r * sin(theta));
		line16(ox, oy, ox + cx, oy + cy, pal16[15]);

		/* minute hand */
		// TODO: draw a triangle
		theta = 2 * M_PI * (tm.tm_min / 60.) - M_PI / 2;
		cx = round(r * cos(theta));
		cy = round(r * sin(theta));
		line16(ox, oy, ox + cx, oy + cy, pal16[10]);

		/* hour hand */
		// TODO: draw a triangle
		theta = 2 * M_PI * ((tm.tm_hour % 12) / 12.) - M_PI / 2;
		cx = round(r / 2 * cos(theta));
		cy = round(r / 2 * sin(theta));
		line16(ox, oy, ox + cx, oy + cy, pal16[13]);

		sleep(1);
	}
}

/* draws a simple star pattern to test the line drawing function */
void
test16(void)
{
	unsigned short ox, oy;
	unsigned short r;

	ox = xres / 2;
	oy = yres / 2;
	r = MIN(xres, yres) / 4;

	line16(ox, oy, ox - r, oy - r, pal16[15]); // top-left
	line16(ox, oy, ox + r, oy - r, pal16[12]); // top-right
	line16(ox, oy, ox - r, oy + r, pal16[10]); // bottom-left
	line16(ox, oy, ox + r, oy + r, pal16[9]); // bottom-right

	line16(ox, oy, ox - r / 2, oy - r, pal16[14]);
	line16(ox, oy, ox + r / 2, oy - r, pal16[14]);
	line16(ox, oy, ox, oy - r, pal16[13]);

	line16(ox, oy, ox - r / 2, oy + r, pal16[14]);
	line16(ox, oy, ox + r / 2, oy + r, pal16[14]);
	line16(ox, oy, ox, oy + r, pal16[13]);

	line16(ox, oy, ox - r, oy - r / 2, pal16[2]);
	line16(ox, oy, ox - r, oy + r / 2, pal16[4]);
	line16(ox, oy, ox - r, oy, pal16[1]);

	line16(ox, oy, ox + r, oy + r / 2, pal16[2]);
	line16(ox, oy, ox + r, oy - r / 2, pal16[4]);
	line16(ox, oy, ox + r, oy, pal16[1]);
}

/******************************************************************************/

int
main(int argc, char **argv)
{
	srand(time(0));

	if (gfx_open(&xres, &yres, &rowbytes))
		return 1;

	if (gfx_setbpp(16, &rowbytes))
		return 1;

	draw_init(16);

	pixels = gfx_buffer(&pixels_len);
	if (!pixels)
		return 1;

	gfx_clear(pixels, pixels_len);

	pal_init();

	if (argc == 1)
		printf("usage: %s [d|c|t]\n", argv[0]);
	else if (argv[1][0] == 'd')
		demo16();
	else if (argv[1][0] == 'c')
		clock16();
	else if (argv[1][0] == 't')
		test16();
	else
		goto failure;

	gfx_close();

	return 0;
failure:
	gfx_close();
	return 1;
}
