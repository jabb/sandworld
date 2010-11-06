
#include "sandworld.h"


void sw_seed(unsigned long s)
{
	srand(s);
}

int sw_randint(unsigned int l, unsigned int h)
{
	return rand() % (MAX(l, h + 1) - MIN(l, h + 1)) + l;
}

/*
 * Puts a colored character on the screen at x,y
 */
void sw_putch(int x, int y, int ch)
{
	mvaddch(y, x, ch);
}

/*
 * Waits for a command and returns it.
 */
int sw_getcmd(void)
{
	return getch();
}

void sw_setfgbg(color_t fg, color_t bg, attr_t attr)
{
	attron(COLOR_PAIR(bg * SW_COLORS + fg + 1));
	if (attr == SW_ATTR_NONE)
		attroff(A_STANDOUT | A_REVERSE | A_BOLD |
			A_DIM | A_BLINK | A_UNDERLINE);
	else
		attron(attr);
}

void sw_setfg(color_t fg)
{
	sw_setfgbg(fg, SW_BLACK, SW_ATTR_NONE);
}

void sw_putstr(int x, int y, const char *str, ...)
{
	int old_x;
	int old_y;
	va_list args;

	va_start(args, str);
	old_x = getcurx(stdscr);
	old_y = getcury(stdscr);

	move(y, x);
	vwprintw(stdscr, str, args);
	move(old_y, old_x);

	va_end(args);
}

void sw_vputstr(int x, int y, const char *str, va_list args)
{
	int old_x;
	int old_y;

	old_x = getcurx(stdscr);
	old_y = getcury(stdscr);

	move(y, x);
	vwprintw(stdscr, str, args);
	move(old_y, old_x);
}

void sw_clearlineto(int line, int xstart, int xend)
{
	int xi;
	for (xi = xstart; xi < xend; ++xi)
		sw_putch(xi, line, ' ');
}

void sw_box(int x, int y, int w, int h)
{
	mvhline(y, x, ACS_HLINE, w);
	mvhline(y + h - 1, x, ACS_HLINE, w);

	mvvline(y, x, ACS_VLINE, h - 1);
	mvvline(y, x + w, ACS_VLINE, h - 1);

	mvaddch(y, x, ACS_ULCORNER);
	mvaddch(y, x + w, ACS_URCORNER);
	mvaddch(y + h - 1, x, ACS_LLCORNER);
	mvaddch(y + h - 1, x + w, ACS_LRCORNER);
}

void sw_getdelta(int dir, int *dx, int *dy)
{
	if (dx)
		*dx = 0;
	if (dy)
		*dy = 0;

	switch (dir) {
	case SW_CMD_UP: case SW_CMD_UP2:
		if (dy)
			*dy = -1;
		break;
	case SW_CMD_DOWN: case SW_CMD_DOWN2:
		if (dy)
			*dy = 1;
		break;
	case SW_CMD_LEFT: case SW_CMD_LEFT2:
		if (dx)
			*dx = -1;
		break;
	case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
		if (dx)
			*dx = 1;
		break;
	default:
		break;
	}
}

/* Minus two for the borders. */
static char infos[SW_HEIGHT - 2][SW_WIDTH - 2];
static int numinfos = 0;

void sw_addinfo(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	memset(infos[numinfos], 0, SW_WIDTH);
	vsnprintf(infos[numinfos], SW_WIDTH, str, args);
	va_end(args);
	numinfos++;
}

void sw_infobox(int x, int y)
{
	int maxw = 0;
	int i;

	for (i = 0; i < numinfos; ++i)
		if (strlen(infos[i]) > maxw)
			maxw = strlen(infos[i]);

	for (i = 0; i < numinfos; ++i) {
		sw_clearlineto(i + y + 1, x, maxw + x + 2);
		sw_putstr(x + 1, i + y + 1, infos[i]);
	}

	sw_box(x, y, maxw + 2, numinfos + 2);

	sw_getcmd();
}

void sw_clearinfo(void)
{
	numinfos = 0;
}

void sw_displayhelp(void)
{
	sw_setfg(SW_BLUE);
	sw_clearinfo();
	sw_addinfo("Help");
	sw_addinfo("--------------------------------");
	sw_addinfo("ijkl    movement");
	sw_addinfo("<tab>   switch boxes");
	sw_addinfo("<space> interact with something");
	sw_addinfo("q       quit a box or the game");
	sw_addinfo("w       your info");
	sw_addinfo("e       your rucksack");
	sw_addinfo("a       attack something");
	sw_addinfo("s       swap with something");
	sw_addinfo("d       drop something");
	sw_addinfo("f       info on something");
	/*sw_addinfo("z       ");*/
	sw_addinfo("x       use your tool on something");
	sw_addinfo("c       create items");
	sw_addinfo("?       this box");
	sw_infobox(0, 0);
}

