#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include "gfx.h"

/******************************************************************************/

static int fb_fd = -1;
static const char *fb_dev;
static struct fb_var_screeninfo fb_vinfo;
static struct fb_fix_screeninfo fb_finfo;
static void *fb_pixels;
static int fb_frontbuffer_id;
static unsigned short fb_saved_red[256], fb_saved_green[256], fb_saved_blue[256];
static int fb_saved_palette;
static int fb_screensize;
static int fb_doublebuffer;

/******************************************************************************/

#define fb_xres (fb_vinfo.xres)
#define fb_yres (fb_vinfo.yres)
#define fb_rowbytes (fb_finfo.line_length)

/******************************************************************************/

int gfx_setpal(unsigned i, unsigned n, unsigned short *r, unsigned short *g,
		unsigned short *b);

/******************************************************************************/

static int
gfx_save_pal(void)
{
	struct fb_cmap pal;

	if (fb_fd == -1)
		return -1;

	pal.start = 0;
	pal.len = 256;
	pal.red = fb_saved_red;
	pal.green = fb_saved_green;
	pal.blue = fb_saved_blue;
	pal.transp = 0;

	fb_saved_palette = 0;
	if (ioctl(fb_fd, FBIOGETCMAP, &pal)) {
		perror(fb_dev);
		return -1;
	}
	fb_saved_palette = 1;

	return 0;
}

static int
gfx_show_primary(void)
{
	fb_frontbuffer_id = 0;
	if (fb_vinfo.yoffset != 0) {
		fb_vinfo.yoffset = 0;
		if (ioctl(fb_fd, FBIOPAN_DISPLAY, &fb_vinfo))
			return -1;
	}

	return 0;
}

static void
gfx_unmap_buffer(void)
{
	if (fb_pixels) {
		if (munmap(fb_pixels, fb_finfo.smem_len))
			perror(fb_dev);
		fb_pixels = NULL;
	}
}

static void *
gfx_map_buffer(void)
{
	if (fb_fd == -1)
		return NULL;

	fb_pixels = mmap(NULL, fb_finfo.smem_len, PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if (fb_pixels == (void*)(MAP_FAILED)) {
		perror(fb_dev);
		fb_pixels = NULL;
		return NULL;
	}

	fb_screensize = fb_rowbytes * fb_yres;
	// TODO: round up by ypanstep

	if (fb_screensize * 2u <= fb_finfo.smem_len)
		fb_doublebuffer = 1;
	else
		fb_doublebuffer = 0;

	return fb_pixels;
}

static void
gfx_dump_info(void)
{
	dprintf(STDERR_FILENO, "%s:xres=%d yres=%d\n", fb_dev, fb_xres, fb_yres);
	dprintf(STDERR_FILENO, "%s:bpp=%d\n", fb_dev, fb_vinfo.bits_per_pixel);
}

void
gfx_close(void)
{
	if (fb_fd == -1)
		return;

	if (fb_saved_palette)
		gfx_setpal(0, 256, fb_saved_red, fb_saved_green, fb_saved_blue);

	gfx_unmap_buffer();

	close(fb_fd);
	fb_fd = -1;

	memset(&fb_vinfo, 0, sizeof(fb_vinfo));
	memset(&fb_finfo, 0, sizeof(fb_finfo));
}

int
gfx_open(unsigned *xres, unsigned *yres, size_t *rowbytes)
{
	fb_dev = getenv("FBDEV") ? : "/dev/fb0";

	if (fb_fd != -1)
		return -1;

	fb_fd = open(fb_dev, O_RDWR);
	if (fb_fd == -1)
		goto failure;

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_vinfo))
		goto failure;

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_finfo))
		goto failure;

	if (gfx_show_primary())
		goto failure;

	if (xres)
		*xres = fb_xres;

	if (yres)
		*yres = fb_yres;

	if (rowbytes)
		*rowbytes = fb_rowbytes ? : fb_xres * 4; // HACK!

	gfx_dump_info();

	return 0;
failure:
	perror(fb_dev);
	if (fb_fd != -1)
		close(fb_fd);
	fb_fd = -1;
	return -1;
}

int
gfx_setbpp(unsigned bpp, size_t *rowbytes)
{
	struct fb_var_screeninfo vinfo;

	if (fb_fd == -1)
		return -1;

	vinfo = fb_vinfo;

	vinfo.bits_per_pixel = bpp;
	if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo))
		goto failure;

	if (vinfo.bits_per_pixel != bpp) {
		dprintf(STDERR_FILENO, "%s:unable to set %d bpp\n", fb_dev, bpp);
		return -1;
	}

	/* HACK: maybe refresh line_lenght? (I don't know really) */
	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_finfo))
		goto failure;

	if (rowbytes)
		*rowbytes = fb_rowbytes ? : fb_xres * 4; // HACK!

	gfx_dump_info();

	return 0;
failure:
	perror(fb_dev);
	return -1;
}

void *
gfx_buffer(size_t *len)
{
	if (!fb_pixels)
		gfx_map_buffer();

	if (!fb_pixels)
		return NULL;

	if (len)
		*len = fb_screensize;

	if (!fb_doublebuffer || !fb_frontbuffer_id)
		return fb_pixels;

	return fb_pixels + fb_screensize;
}

int
gfx_setpal(unsigned i, unsigned n, unsigned short *r, unsigned short *g, unsigned short *b)
{
	struct fb_cmap pal;

	if (fb_fd == -1)
		return -1;

	if (!fb_saved_palette) {
		if (gfx_save_pal())
			return -1;
	}

	pal.start = i;
	pal.len = n;
	pal.red = r;
	pal.green = g;
	pal.blue = b;
	pal.transp = 0;

	if (ioctl(fb_fd, FBIOPUTCMAP, &pal)) {
		perror(fb_dev);
		return -1;
	}

	return 0;
}

int
gfx_swapbuffers(void)
{
	if (!fb_doublebuffer)
		return 0;

	fb_vinfo.yoffset = fb_frontbuffer_id ? fb_screensize : 0;

	if (ioctl(fb_fd, FBIOPAN_DISPLAY, &fb_vinfo)) {
		perror(fb_dev);
		return -1;
	}
	fb_frontbuffer_id = !fb_frontbuffer_id;

	return 0;
}
