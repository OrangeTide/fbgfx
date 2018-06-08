/* PUBLIC DOMAIN - Jon Mayo - August 3, 2015 */
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include <unistd.h>

static const char digits[][9] = {
	['0'] = {1,1,1,0,1,1,1,0,0},
	['1'] = {0,0,1,0,0,1,0,0,0},
	['2'] = {1,0,1,1,1,0,1,0,0},
	['3'] = {1,0,1,1,0,1,1,0,0},
	['4'] = {0,1,1,1,0,1,0,0,0},
	['5'] = {1,1,0,1,0,1,1,0,0},
	['6'] = {1,1,0,1,1,1,1,0,0},
	['7'] = {1,0,1,0,0,1,0,0,0},
	['8'] = {1,1,1,1,1,1,1,0,0},
	['9'] = {1,1,1,1,0,1,0,0,0},
	[':'] = {0,0,0,0,0,0,0,1,1},
	['A'] = {0,0,0,0,0,0,0,1,0},
	['a'] = {0,0,0,0,0,0,0,1,0},
	['P'] = {0,0,0,0,0,0,0,0,1},
	['p'] = {0,0,0,0,0,0,0,0,1},
	[' '] = {0,0,0,0,0,0,0,0,0},
};

#define DWIDTH 5
#define DHEIGHT 5

void drawchar(int xpos, int ypos, unsigned char c)
{
	int x, y;

	const char *cur = digits[c < (sizeof(digits) / sizeof(*digits)) ? c : 0];
	/* elem0 */
	move(ypos, xpos + 1);
	for (x = 0; x < DWIDTH; x++)
		addch(cur[0] ? '#' : ' ');
	/* elem1 and elem2 and elem7 */
	for (y = 0; y < DHEIGHT; y++) {
		move(ypos + y + 1, xpos);
		addch(cur[1] ? '#' : ' ');
		for (x = 0; x < DWIDTH; x++) {
			if (y / 2 == DHEIGHT / 4 && x == DWIDTH / 2)
				addch(cur[7] ? '#' : ' ');
			else
				addch(' ');
		}
		addch(cur[2] ? '#' : ' ');
	}
	/* elem3 */
	move(ypos + 1 + DHEIGHT, xpos + 1);
	for (x = 0; x < DWIDTH; x++)
		addch(cur[3] ? '#' : ' ');
	/* elem4 and elem5 and elem8 */
	for (y = 0; y < DHEIGHT; y++) {
		move(ypos + y + 1 + DHEIGHT + 1, xpos);
		addch(cur[4] ? '#' : ' ');
		for (x = 0; x < DWIDTH; x++) {
			if (y / 2 == DHEIGHT / 4 && x == DWIDTH / 2)
				addch(cur[8] ? '#' : ' ');
			else
				addch(' ');
		}
		addch(cur[5] ? '#' : ' ');
	}
	/* elem6 */
	move(ypos + 1 + DHEIGHT + 1 + DHEIGHT, xpos + 1);
	for (x = 0; x < DWIDTH; x++)
		addch(cur[6] ? '#' : ' ');
}

static void drawstr(int xpos, int ypos, const char *s)
{
	while (*s) {
		drawchar(xpos, ypos, *s);
		xpos += 3 + DWIDTH;
		s++;
	}
}

static long user_hz;
static long laststat_user, laststat_nice, laststat_sys, laststat_idle, laststat_iowait;

static int readstat(long *user, long *nice, long *system, long *idle, long *iowait)
{
	FILE *f;
	const char *fn = "/proc/stat";
	f = fopen(fn, "r");
	if (!f) {
		perror(fn);
		return -1;
	}
	if (fscanf(f, "cpu %ld %ld %ld %ld %ld", user, nice, system, idle, iowait) != 5)
		return -1;
	fclose(f);
	return 0;
}

static int initload(void)
{
	user_hz = sysconf(_SC_CLK_TCK);
	if (readstat(&laststat_user, &laststat_nice, &laststat_sys, &laststat_idle, &laststat_iowait))
		return -1;
	return 0;
}

static int readloadavg(float *avg1, float *avg5, float *avg15)
{
	const char *fn = "/proc/loadavg";
	FILE *f = fopen(fn, "r");
	if (!f) {
		perror(fn);
		return -1;
	}
	if (fscanf(f, "%f %f %f", avg1, avg5, avg15) != 3)
		return -1;
	fclose(f);
	return 0;
}

static void drawload(int xpos, int ypos, int width, int height)
{
	if (user_hz < 0)
		return;
	float a1, a5, a15;
	if (readloadavg(&a1, &a5, &a15))
		return;
	long u, n, s, i, w;
	if (readstat(&u, &n, &s, &i, &w))
		return;
	u -= laststat_user;
	n -= laststat_nice;
	s -= laststat_sys;
	i -= laststat_idle;
	w -= laststat_iowait;
	move(ypos, xpos);
	// TODO: graph
	printw("userhz=%ld user=%ld nice=%ld sys=%ld idle=%ld iowait=%ld loadavg=%g/%g/%g", user_hz, u, n, s, i, w, a1, a5, a15);
}

int main()
{
	initload();
	initscr();
	cbreak();
	halfdelay(50); /* 5 seconds */

	time_t t = time(0);
	srand(t);

	do {
		erase();
		t = time(0);
		char buf[32];
		strftime(buf, sizeof(buf), "%I:%M%p", localtime(&t));
		if (buf[0] == '0') /* replace leading zero of hours "01" to "09" with " " */
			buf[0] = ' ';
		int width = strlen(buf) * (3 + DWIDTH);
		int height = 2 * DHEIGHT + 3;
		if (LINES <= height || COLS <= width) {
			/* fall back to normal text on small displays */
			move(1, 1);
			addstr(buf);
		} else {
			int ypos = rand() % (LINES - height);
			int xpos = rand() % (COLS - width);
			// DISABLED: drawload(1, 1, 0, 0);
			drawstr(xpos, ypos, buf);
		}
		refresh();
	} while (getch() == ERR);

	endwin();
	return 0;
}
