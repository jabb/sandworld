
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
