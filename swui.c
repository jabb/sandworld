
#include "sandworld.h"
#include "swui.h"

int sw_ui_confirm(const char *str, ...)
{
	va_list args;
	int cmd;

	va_start(args, str);

	sw_setfg(SW_BLACK);
	sw_clearlineto(0, 0, SW_COLS);
	sw_setfg(SW_RED);
	sw_vputstr(0, 0, str, args);
	cmd = sw_getcmd();

	va_end(args);

	return cmd == 'y' || cmd == SW_CMD_ACTION || cmd == SW_CMD_ACTION2;
}

int sw_ui_getnumber(int def, const char *str, ...)
{
	char buf[BUFSIZ];
	char msg[BUFSIZ];
	int pos = 0;
	int i = 0;
	int c = 0;
	va_list args;

	memset(buf, 0, BUFSIZ);

	va_start(args, str);
	vsnprintf(msg, BUFSIZ, str, args);
	va_end(args);

	do {
		if ((c >= '0' && c <= '9') || c == '-') {
			buf[pos] = c;
			pos++;
		}
		else if (c == KEY_BACKSPACE || c == KEY_DC || c == '\b') {
			if (pos >= 1) {
				pos--;
				buf[pos] = '\0';
			}
		}

		sw_setfg(SW_BLACK);
		sw_clearlineto(0, 0, SW_COLS);
		sw_setfg(SW_WHITE);
		sw_putstr(0, 0, "%s (%d): %s", msg, def, buf);
		c = getch();
	} while (c != '\n' && c != 27 && c != 'q');

	buf[pos] = 0;
	sscanf(buf, "%d", &i);

	if (c == 27 || c == 'q' || strlen(buf) == 0)
		return def;
	return i;
}

int sw_ui_getdir(const char *str, ...)
{
	char msg[BUFSIZ] = {0};
	int c = -1;
	va_list args;

	va_start(args, str);
	vsnprintf(msg, BUFSIZ, str, args);
	va_end(args);

	sw_setfg(SW_BLACK);
	sw_clearlineto(0, 0, SW_COLS);
	sw_setfg(SW_WHITE);
	sw_putstr(0, 0, "%s (Up/Down/Left/Right): ", msg);

	c = sw_getcmd();

	if (c == SW_CMD_UP || c == SW_CMD_DOWN ||
		c == SW_CMD_LEFT || c == SW_CMD_RIGHT)
		return c;

	return SW_CMD_NONE;
}

#define MAX_ALERTS 8
static char _alert_strs[MAX_ALERTS][BUFSIZ] = {{0}};
static int _alerts = 0;

void sw_ui_addalert(const char *str, ...)
{
	va_list args;

	if (_alerts >= MAX_ALERTS)
		return;

	va_start(args, str);
	vsnprintf(_alert_strs[_alerts], BUFSIZ, str, args);
	va_end(args);
	_alerts += 1;
}

void sw_ui_draw(void)
{
	int i = 0;

	if (_alerts <= 0)
		return;

	/* Draw first alert without block. */
	sw_setfg(SW_BLACK);
	sw_clearlineto(0, 0, SW_COLS);
	sw_setfg(SW_BLUE);
	sw_putstr(0, 0, "%s", _alert_strs[0]);

	for (i = 1; i < _alerts; ++i) {
		sw_putstr(strlen(_alert_strs[i - 1]), 0, "\t*More*");
		sw_getcmd();
		sw_setfg(SW_BLACK);
		sw_clearlineto(0, 0, SW_COLS);
		sw_setfg(SW_BLUE);
		sw_putstr(0, 0, "%s", _alert_strs[i]);
	}

	_alerts = 0;
}

/* Minus two for the borders. */
static char infos[SW_HEIGHT - 2][SW_WIDTH - 2];
static int numinfos = 0;

void sw_ui_addinfo(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	memset(infos[numinfos], 0, SW_WIDTH);
	vsnprintf(infos[numinfos], SW_WIDTH, str, args);
	va_end(args);
	numinfos++;
}

void sw_ui_infobox(int x, int y)
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

void sw_ui_clearinfo(void)
{
	numinfos = 0;
}

/* Minus two for the borders. */
static char menuheader[SW_WIDTH - 2] = {0};
static char menus[SW_HEIGHT - 2][SW_WIDTH - 2];
static int nummenus = 0;

void sw_ui_addmenuheader(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	memset(menuheader, 0, SW_WIDTH - 2);
	vsnprintf(menuheader, SW_WIDTH - 2, str, args);
	va_end(args);

}

void sw_ui_addmenu(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	memset(menus[nummenus], 0, SW_WIDTH - 2);
	vsnprintf(menus[nummenus], SW_WIDTH - 2, str, args);
	va_end(args);
	nummenus++;

}

int sw_ui_menubox(int x, int y)
{
	const char *sep = "--------------------------------";
	static int sel = 0; /* Static to keep the same selection selected. */
	int maxw = MAX(strlen(sep), strlen(menuheader));
	int i;
	int cmd = SW_CMD_NONE;

	if (sel < 0 || sel >= nummenus)
		sel = 0;

	for (i = 0; i < nummenus; ++i)
		if (strlen(menus[i]) > maxw)
			maxw = strlen(menus[i]);

	do {
		switch (cmd) {
		case SW_CMD_UP: case SW_CMD_UP2:
			sel--;
			if (sel < 0)
				sel = nummenus - 1;
			break;
		case SW_CMD_DOWN: case SW_CMD_DOWN2:
			sel++;
			if (sel >= nummenus)
				sel = 0;
			break;
		case SW_CMD_QUIT:
			sel = -1;
			goto exit;
		case SW_CMD_ACTION: case SW_CMD_ACTION2:
			goto exit;
		default:
			break;
		}


		sw_setfg(SW_WHITE);
		sw_clearlineto(y + 1, x, maxw + x + 2);
		sw_clearlineto(y + 2, x, maxw + x + 2);
		sw_putstr(x + 1, y + 1, menuheader);
		sw_putstr(x + 1, y + 2, sep);

		for (i = 0; i < nummenus; ++i) {
			sw_setfg(SW_WHITE);
			if (i == sel)
				sw_setfg(SW_YELLOW);
			sw_clearlineto(i + y + 1 + 2, x, maxw + x + 2);
			sw_putstr(x + 1, i + y + 1 + 2, menus[i]);
		}

		sw_setfg(SW_WHITE);
		sw_box(x, y, maxw + 2, nummenus + 2 + 2);

		cmd = sw_getcmd();
	} while (1);
exit:
	return sel;
}

void sw_ui_clearmenu(void)
{
	sw_ui_addmenuheader("");
	nummenus = 0;
}

static int (*loopevent) (int cmd) = NULL;
static void (*loopdraw) (void) = NULL;

void sw_ui_loopevent(int (*fp) (int cmd))
{
	loopevent = fp;
}

void sw_ui_loopdraw(void (*fp) (void))
{
	loopdraw = fp;
}

void sw_ui_loop(void)
{
	int cmd = SW_CMD_NONE;

	do {
		if (loopevent)
			if (loopevent(cmd) != 0)
				break;
		if (loopdraw)
			loopdraw();

		cmd = sw_getcmd();
	} while (1);
}

