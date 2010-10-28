/* Copyright (c) 2010, Michael Patraw
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of Michael Patraw may not be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Michael Patraw ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Michael Patraw BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Sandworld - A sandbox game with multiple worlds.
 *
 * TODO:
 * 	[*]	Generic overlay for displaying and selecting items with
 * 		various actions.
 * 	[ ]	More items!
 * 	[ ]	Creation system.
 * 	[ ]	Tools.
 * 	[ ]	Make abyss drop actual items.
 *
 */
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SW_WORLD_WIDTH	80
#define SW_WORLD_HEIGHT	24

#define SW_BLACK	COLOR_BLACK
#define SW_RED		COLOR_RED
#define SW_GREEN	COLOR_GREEN
#define SW_BLUE		COLOR_BLUE
#define SW_YELLOW	COLOR_YELLOW
#define SW_MAGENTA	COLOR_MAGENTA
#define SW_CYAN		COLOR_CYAN
#define SW_WHITE	COLOR_WHITE
#define SW_COLORS	8

#define SW_ROWS	LINES
#define SW_COLS	COLS

#define SW_ERR_GENERAL		(-1)	/* General error */
#define SW_ERR_NONCOMP		(-2)	/* Non-compliancy */
#define SW_ERR_NOMEM		(-3)	/* No memory */
#define SW_ERR_OVERFLOW		(-4)	/* Buffer overflow */
#define SW_ERR_UNDERFLOW	(-5)	/* Buffer underflow */
#define SW_ERR_RANGE		(-6)	/* Range error */

#define SW_CMD_NONE		(-1)
#define SW_CMD_UP		KEY_UP
#define SW_CMD_DOWN		KEY_DOWN
#define SW_CMD_LEFT		KEY_LEFT
#define SW_CMD_RIGHT		KEY_RIGHT
#define SW_CMD_UP2		'k'
#define SW_CMD_DOWN2		'j'
#define SW_CMD_LEFT2		'h'
#define SW_CMD_RIGHT2		'l'
#define SW_CMD_SWITCH		'\t'
#define SW_CMD_ACTION		' '
#define SW_CMD_QUIT		'q'
/*#define SW_CMD_RES1		'w'*/
#define SW_CMD_RUCKSACK		'e'
#define SW_CMD_ATTACK		'a'
#define SW_CMD_SWAP		's'
#define SW_CMD_DROP		'd'
/*#define SW_CMD_SKILL		'z'*/
#define SW_CMD_TOOL		'x'
/*#define SW_CMD_CREATE		'c'*/

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

typedef short color_t;
typedef chtype attr_t;

#define SW_ATTR_NONE		0
#define SW_ATTR_BRIGHT		A_BOLD

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
inline void sw_putch(int x, int y, int ch)
{
	mvaddch(y, x, ch);
}

/*
 * Waits for a command and returns it.
 */
inline int sw_getcmd(void)
{
	return getch();
}

inline void sw_setfgbg(color_t fg, color_t bg, attr_t attr)
{
	attron(COLOR_PAIR(bg * SW_COLORS + fg + 1));
	if (attr == SW_ATTR_NONE)
		attroff(A_STANDOUT | A_REVERSE | A_BOLD |
			A_DIM | A_BLINK | A_UNDERLINE);
	else
		attron(attr);
}

inline void sw_setfg(color_t fg)
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

int sw_menu(const char *items[], int numitems)
{
	int cmd = 0;
	int i;
	int selected = 0;

	do {
		if (cmd == SW_CMD_UP) {
			selected--;
			if (selected < 0)
				selected = numitems - 1;
		} else if (cmd == SW_CMD_DOWN) {
			selected++;
			if (selected >= numitems)
				selected = 0;
		} else if (cmd == ' ') {
			return selected;
		} else if ((int)cmd >= '0') {
			if ((int)cmd - (int)'0' < numitems)
				selected = (int)cmd - (int)'0';
		}

		for (i = 0; i < numitems; ++i) {
			sw_clearlineto(i, 0, SW_COLS/2);
			if (i == selected) {
				sw_setfgbg(SW_YELLOW, SW_BLUE, SW_ATTR_NONE);
				sw_putstr(0, i, items[i]);
			} else {
				sw_setfgbg(SW_WHITE, SW_BLUE, SW_ATTR_NONE);
				sw_putstr(0, i, items[i]);
			}
		}

		cmd = sw_getcmd();
	} while (1);

	return -1;
}

#define SW_ITEM_NONE		0
#define SW_ITEM_WEAPON		(1 << 0)
#define SW_ITEM_TOOL		(1 << 1)
#define SW_ITEM_MATERIAL	(1 << 2)

struct sw_item {
	unsigned long flags;
	int id;
	const char *name;
	int amount;
};

/* Perhaps later store these into a file? */
static struct sw_item sw_item_table[] = {
	/* TYPE			ID	NAME			AMOUNT */
	{  SW_ITEM_NONE, 	0,	"Nothing",		1},
	{  SW_ITEM_MATERIAL, 	1,	"Dirt",			1},
	{  SW_ITEM_MATERIAL, 	2,	"Wood",			1},
	{  SW_ITEM_WEAPON, 	3,	"Pulverizer",		1},
};

enum {
	/*SW_ITEM_NONE,*/
	SW_ITEM_DIRT=1,
	SW_ITEM_WOOD,
	SW_ITEM_PULVERIZER
};

#define SW_RUCKSACK_SIZE 16
#define SW_INHAND_POS 0
#define SW_ONSELF_POS 1

struct sw_rucksack {
	struct sw_item items[SW_RUCKSACK_SIZE];
};

struct sw_obj;

enum sw_tile_type {
	SW_TILE_BLANK,
	SW_TILE_GRASS
};

struct sw_tile {
	enum sw_tile_type type;
	attr_t attr;
	color_t fg;
	color_t bg;
	int display;
	struct sw_obj *object;
};

struct sw_world;

enum sw_obj_ev {
	SW_OBJ_EV_UPDATE,
	SW_OBJ_EV_MOVE,
	SW_OBJ_EV_INTERACT,
	SW_OBJ_EV_ATTACK,
	SW_OBJ_EV_TOOL
};

enum sw_obj_type {
	SW_OBJ_NONE,	/* This can not be interacted with at all. */
	SW_OBJ_PLAYER,	/* The player! */
	SW_OBJ_ABYSS,	/* This can be dug into. */
	SW_OBJ_ITEMS,	/* Simple item batch. */
	SW_OBJ_TREE,
	SW_OBJ_BOULDER
};

struct sw_obj {
	enum sw_obj_type type;
	attr_t attr;
	color_t fg;
	color_t bg;
	int display;
	int x;
	int y;
	int life;
	struct sw_tile *tile;
	struct sw_rucksack rucksack;
	/* This function returns 0 when a given object event is successful */
	int (*handle_event) (struct sw_world *,
		struct sw_obj *, struct sw_obj *, enum sw_obj_ev);
};

struct sw_world {
	int home_x;
	int home_y;
	struct sw_tile tiles[SW_WORLD_WIDTH][SW_WORLD_HEIGHT];
	struct sw_world_link {
		int x;
		int y;
		int dest_x;
		int dest_y;
		struct sw_world *world;
	} *linkto, *linkfrom;
};

int sw_start(void)
{
	int f;
	int b;
	int ret = 0;

	if (!initscr()) {
		ret = SW_ERR_GENERAL;
		goto failure;
	}

	if (noecho() == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (cbreak() == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (curs_set(0) == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (keypad(stdscr, 1) == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (start_color() == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	for (b = 0; b < SW_COLORS; ++b)
		for (f = 0; f < SW_COLORS; ++f)
			if (init_pair(b * SW_COLORS + f + 1, f, b) == ERR) {
				ret = SW_ERR_GENERAL;
				goto failure;
			}

	sw_seed(time(NULL));

	goto success;
failure:
	endwin();
success:
	return ret;
}

void sw_stop(void)
{
	endwin();
}

int sw_ui_confirm(const char *str, ...);
int sw_ui_getnumber(int def, const char *str, ...);
int sw_ui_getdir(const char *str, ...);
void sw_ui_addalert(const char *str, ...);
void sw_ui_draw(void);

struct sw_item sw_item_make(unsigned long flags);
int sw_item_areequal(struct sw_item i1, struct sw_item i2);
int sw_item_isnone(struct sw_item i);
int sw_item_is(struct sw_item i, unsigned long flags);

struct sw_item *sw_rucksack_getitemp(struct sw_rucksack *rs, int pos);
/* Shortcut (damn lazy programmers) */
#define SW_ITEMP(rs, pos) (sw_rucksack_getitemp(rs, pos))
int sw_rucksack_empty(struct sw_rucksack *rs);
int sw_rucksack_freeslots(struct sw_rucksack *rs);
int sw_rucksack_takenslots(struct sw_rucksack *rs);
void sw_rucksack_swap(struct sw_rucksack *rs, int pos1, int pos2);
int sw_rucksack_additem(struct sw_rucksack *rs, struct sw_item item);
int sw_rucksack_trans(struct sw_rucksack *rs, struct sw_rucksack *rs2, int pos);
int sw_rucksack_split(struct sw_rucksack *rs, int pos);
int sw_rucksack_addrucksack(struct sw_rucksack *rs, struct sw_rucksack *rs2);
struct sw_item sw_rucksack_removeitem(struct sw_rucksack *rs, int pos);
int sw_rucksack_hasweapon(struct sw_rucksack *rs);
int sw_rucksack_hastool(struct sw_rucksack *rs);
void sw_rucksack_draw(struct sw_rucksack *rs, int x, int y);
void sw_rucksack_show(struct sw_rucksack *rs);
void sw_rucksack_compare(struct sw_rucksack *rs, struct sw_rucksack *rs2);

struct sw_obj *sw_obj_alloc(void);
void sw_obj_free(struct sw_obj *o);
struct sw_obj *sw_obj_gentype(enum sw_obj_type type);
int sw_obj_attack(struct sw_obj *def, struct sw_obj *att);

struct sw_tile sw_tile_make(enum sw_tile_type type);
void sw_tile_walkon(struct sw_tile tile, struct sw_obj *obj);
void sw_tile_draw(struct sw_tile tile, int x, int y);

struct sw_world *sw_world_alloc(void);
void sw_world_free(struct sw_world *world);
struct sw_tile *sw_world_gettilep(struct sw_world *world, int x, int y);
/* Shortcut (damn lazy programmers) */
#define SW_TILEP(world, x, y) (sw_world_gettilep(world, x, y))
void sw_world_draw(struct sw_world *world, int x, int y);
int sw_world_placeobj(struct sw_world *world, struct sw_obj *o, int x, int y);
int sw_world_placeobjhome(struct sw_world *world, struct sw_obj *o);
struct sw_obj *sw_world_removeobj(struct sw_world *world, int x, int y);
void sw_world_freeobj(struct sw_world *world, int x, int y);
void sw_world_freeallobj(struct sw_world *world);
int sw_world_inbounds(struct sw_world *world, int x, int y);
/* These functions trigger object events */
void sw_world_updateobjs(struct sw_world *world);
void sw_world_moveobjby(struct sw_world *world, int x, int y, int dx, int dy);
void sw_world_moveobjto(struct sw_world *world, int x, int y, int nx, int ny);
void sw_world_interactobj(struct sw_world *world, int x, int y, int nx, int ny);
void sw_world_attackobj(struct sw_world *world, int x, int y, int nx, int ny);

struct sw_world *sw_world_genstart(void);

/******************************************************************************\
 * ENTRY
\******************************************************************************/

int main(int argc, char *argv[])
{
	int dx;
	int dy;
	int tmpcmd = 0;
	int tmpnum = 0;
	int cmd = 0;
	struct sw_item tmpitem;
	struct sw_obj *tmpobj = NULL;
	struct sw_world *world = sw_world_genstart();
	struct sw_obj *player = sw_obj_gentype(SW_OBJ_PLAYER);

	sw_start();

	sw_world_placeobjhome(world, player);
	sw_rucksack_additem(&player->rucksack, sw_item_make(SW_ITEM_DIRT));
	sw_rucksack_additem(&player->rucksack, sw_item_make(SW_ITEM_DIRT));

	do {
		switch (cmd) {
		case SW_CMD_UP: case SW_CMD_UP2:
			/* Fallthrough. */
		case SW_CMD_DOWN: case SW_CMD_DOWN2:
			/* Fallthrough. */
		case SW_CMD_LEFT: case SW_CMD_LEFT2:
			/* Fallthrough. */
		case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
			sw_getdelta(cmd, &dx, &dy);
			sw_world_moveobjby(world, player->x, player->y, dx, dy);
			break;
		case SW_CMD_DROP:
			tmpnum = sw_ui_getnumber( -1, "Drop which item?");
			if (tmpnum >= 0 && tmpnum < SW_RUCKSACK_SIZE) {
				tmpitem = sw_rucksack_removeitem(
					&player->rucksack, tmpnum);
				tmpcmd = sw_ui_getdir("Drop where?");
				if (tmpcmd != SW_CMD_NONE) {
					sw_getdelta(tmpcmd, &dx, &dy);
					tmpobj = sw_obj_gentype(SW_OBJ_ITEMS);
					sw_world_placeobj(world, tmpobj,
						player->x + dx, player->y + dy);

					sw_rucksack_additem(&tmpobj->rucksack,
						tmpitem);
					if (!sw_rucksack_takenslots(&tmpobj->rucksack))
						sw_world_freeobj(world,
						player->x + dx, player->y + dy);
				}
			}
			else {
				sw_ui_addalert("No such item.");
			}
			break;
		case SW_CMD_ACTION:
			tmpcmd = sw_ui_getdir("Interact with?");
			if (tmpcmd != SW_CMD_NONE) {
				sw_getdelta(tmpcmd, &dx, &dy);
				sw_world_interactobj(world,
					player->x, player->y,
					player->x + dx, player->y + dy);
			}
			break;
		case SW_CMD_ATTACK:
			tmpcmd = sw_ui_getdir("Attack what?");
			if (tmpcmd != SW_CMD_NONE) {
				sw_getdelta(tmpcmd, &dx, &dy);
				sw_world_attackobj(world,
					player->x, player->y,
					player->x + dx, player->y + dy);
			}
			break;
		case SW_CMD_RUCKSACK:
			sw_rucksack_show(&player->rucksack);
			break;
		case SW_CMD_QUIT:
			if (sw_ui_confirm("Are you sure? (y/n)"))
				goto exit;
			break;
		default:
			break;
		}

		sw_world_updateobjs(world);
		sw_world_draw(world, 0, 0);
		sw_ui_draw();
		cmd = sw_getcmd();
	} while (1);

exit:
	sw_stop();

	sw_world_freeallobj(world);
	sw_world_free(world);
	return 0;
}

/******************************************************************************\
 * UI
\******************************************************************************/
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

	return cmd == 'y' || cmd == SW_CMD_ACTION;
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

		sw_setfg(SW_BLACK);
		sw_clearlineto(0, 0, SW_COLS);
		sw_setfgbg(SW_WHITE, SW_BLACK, SW_ATTR_NONE);
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

/******************************************************************************\
 * ITEM
\******************************************************************************/
struct sw_item sw_item_make(unsigned long id)
{
	return sw_item_table[id];
}

int sw_item_areequal(struct sw_item i1, struct sw_item i2)
{
	/* This will be improved as time goes on. */
	return i1.id == i2.id;
}

int sw_item_isnone(struct sw_item i)
{
	return i.flags == SW_ITEM_NONE;
}

int sw_item_is(struct sw_item i, unsigned long flags)
{
	/* Check if any of the high order bits (type) are set. */
	return i.flags & flags;
}

/******************************************************************************\
 * RUCKSACK
\******************************************************************************/
struct sw_item *sw_rucksack_getitemp(struct sw_rucksack *rs, int pos)
{
	return &rs->items[pos];
}

int sw_rucksack_empty(struct sw_rucksack *rs)
{
	int i;
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i)
		*SW_ITEMP(rs, i) = sw_item_make(SW_ITEM_NONE);
	return 0;
}

int sw_rucksack_freeslots(struct sw_rucksack *rs)
{
	int i;
	int count = 0;
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i)
		if (sw_item_isnone(*SW_ITEMP(rs, i)))
			count ++;
	return count;
}

int sw_rucksack_takenslots(struct sw_rucksack *rs)
{
	return SW_RUCKSACK_SIZE - sw_rucksack_freeslots(rs);
}

void sw_rucksack_swap(struct sw_rucksack *rs, int pos1, int pos2)
{
	struct sw_item tmp = *SW_ITEMP(rs, pos2);
	*SW_ITEMP(rs, pos1) = *SW_ITEMP(rs, pos2);
	*SW_ITEMP(rs, pos2) = tmp;
}

int sw_rucksack_additem(struct sw_rucksack *rs, struct sw_item item)
{
	int i;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (sw_item_isnone(*SW_ITEMP(rs, i))) {
			*SW_ITEMP(rs, i) = item;
			return 0;
		}
		/* Stack. TODO: Fix items that are the same only in type. */
		else if (sw_item_areequal(*SW_ITEMP(rs, i), item)) {
			SW_ITEMP(rs, i)->amount += item.amount;
			return 0;
		}
	}

	return -1; /* Couldn't find a place for the item. */
}


int sw_rucksack_trans(struct sw_rucksack *rs, struct sw_rucksack *rs2, int pos)
{
	struct sw_item tmp;

	tmp = *SW_ITEMP(rs2, pos);

	if (sw_rucksack_additem(rs, tmp) == 0) {
		sw_rucksack_removeitem(rs2, pos);
		return -1;
	}

	return -1;
}

int sw_rucksack_split(struct sw_rucksack *rs, int pos)
{
	int i;

	/* First pass, check for a second stack of items. */
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (i != pos &&
			sw_item_areequal(*SW_ITEMP(rs, i),
				*SW_ITEMP(rs, pos))) {
			SW_ITEMP(rs, pos)->amount -= 1;
			SW_ITEMP(rs, i)->amount += 1;
			if (SW_ITEMP(rs, pos)->amount <= 0)
				sw_rucksack_removeitem(rs, pos);
			return 0;
		}
	}

	/* Second pass, just add the item to an empty space. */
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (!sw_item_areequal(*SW_ITEMP(rs, i), *SW_ITEMP(rs, pos))) {
			if (sw_item_isnone(*SW_ITEMP(rs, i))) {
				*SW_ITEMP(rs, i) = rs->items[pos];
				SW_ITEMP(rs, i)->amount = 1;
				SW_ITEMP(rs, pos)->amount -= 1;
				if (SW_ITEMP(rs, pos)->amount <= 0)
					sw_rucksack_removeitem(rs, pos);
				return 0;
			}
		}
	}

	return -1;
}

int sw_rucksack_addrucksack(struct sw_rucksack *rs, struct sw_rucksack *rs2)
{
	int i;

	if (sw_rucksack_freeslots(rs) < sw_rucksack_takenslots(rs2))
		return -1;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (!sw_item_isnone(*SW_ITEMP(rs, i))) {
			sw_rucksack_additem(rs, *SW_ITEMP(rs2, i));
			*SW_ITEMP(rs2, i) = sw_item_make(SW_ITEM_NONE);
		}
	}

	return 0;
}

struct sw_item sw_rucksack_removeitem(struct sw_rucksack *rs, int pos)
{
	struct sw_item item;
	item = *SW_ITEMP(rs, pos);
	*SW_ITEMP(rs, pos) = sw_item_make(SW_ITEM_NONE);
	return item;
}


int sw_rucksack_iswielding(struct sw_rucksack *rs)
{
	/* TODO: Clean up (remove `items` direct access) */
	return sw_item_is(rs->items[SW_INHAND_POS], SW_ITEM_WEAPON) ||
		sw_item_is(rs->items[SW_INHAND_POS], SW_ITEM_TOOL);
}

int sw_rucksack_iswearing(struct sw_rucksack *rs)
{
	return 0;
}

void sw_rucksack_draw(struct sw_rucksack *rs, int x, int y)
{
	int i;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		sw_setfg(SW_BLACK);
		sw_clearlineto(i + 1, x, x + SW_COLS/2 - 1);
		sw_setfg(SW_WHITE);
		/* The '\t' is just arbitrary padding */
		if (!sw_item_isnone(*SW_ITEMP(rs, i))) {
			sw_putstr(x + 1, i + y + 1, "\t%s (%d)",
				SW_ITEMP(rs, i)->name,
				SW_ITEMP(rs, i)->amount);
		} else {
			sw_putstr(x + 1, i + y + 1, "\t----");
		}
	}
	sw_box(x, y, SW_COLS/2 - 1, SW_RUCKSACK_SIZE + 2);
}

void sw_rucksack_show(struct sw_rucksack *rs)
{
	int tmp = 0;
	int selected = 0;
	int cmd = 0;
	int i;

	do {
		switch (cmd) {
			case SW_CMD_UP: case SW_CMD_UP2:
				selected--;
				if (selected < 0)
					selected = SW_RUCKSACK_SIZE - 1;
				break;
			case SW_CMD_DOWN: case SW_CMD_DOWN2:
				selected++;
				if (selected >= SW_RUCKSACK_SIZE)
					selected = 0;
				break;
			case SW_CMD_ACTION:
				goto exit;
				break;
			case SW_CMD_SWAP:
				tmp = sw_ui_getnumber(-1, "Swap with?");
				if (tmp >= 0 && tmp < SW_RUCKSACK_SIZE) {
					sw_rucksack_swap(rs, selected, tmp);
				}
				break;
			case SW_CMD_LEFT: case SW_CMD_LEFT2:
			case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
				sw_rucksack_split(rs, selected);
				break;
			case SW_CMD_QUIT:
				return;
			default:
				break;
		}

		sw_rucksack_draw(rs, 0, 0);
		for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
			if (i == selected) {
				sw_setfg(SW_YELLOW);
				sw_putstr(1, i + 1, "%d", i);
			} else {
				sw_setfg(SW_WHITE);
				sw_putstr(1, i + 1, "%d", i);
			}
		}

		cmd = sw_getcmd();
	} while (1);
exit:
	return;
}

void sw_rucksack_compare(struct sw_rucksack *rs, struct sw_rucksack *rs2)
{
	int tmp;
	int which = 1;
	int sel = 0;
	int cmd = 0;
	int i;

	do {
		switch (cmd) {
			case SW_CMD_UP: case SW_CMD_UP2:
				sel--;
				if (sel < 0)
					sel = SW_RUCKSACK_SIZE - 1;
				break;
			case SW_CMD_DOWN: case SW_CMD_DOWN2:
				sel++;
				if (sel >= SW_RUCKSACK_SIZE)
					sel = 0;
				break;
			case SW_CMD_SWITCH:
				which = which == 0 ? 1 : 0;
				break;
			case SW_CMD_ACTION:
				if (sw_rucksack_freeslots(rs) >=
					sw_rucksack_takenslots(rs2)) {
					sw_rucksack_addrucksack(rs, rs2);
					goto exit;
				}
				break;
			case SW_CMD_SWAP:
				tmp = sw_ui_getnumber(-1, "Swap with?");
				if (tmp >= 0 && tmp < SW_RUCKSACK_SIZE) {
					if (which == 0)
						sw_rucksack_swap(rs, sel, tmp);
					else
						sw_rucksack_swap(rs2, sel, tmp);
				}
				break;
			case SW_CMD_LEFT: case SW_CMD_LEFT2:
				if (which == 0) {
					sw_rucksack_split(rs, sel);
				} else {
					sw_rucksack_trans(rs, rs2, sel);
				}
				break;
			case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
				if (which == 0) {
					sw_rucksack_trans(rs2, rs, sel);
				} else {
					sw_rucksack_split(rs2, sel);
				}
				break;
			case SW_CMD_QUIT:
				return;
			default:
				break;
		}

		sw_rucksack_draw(rs, 0, 0);
		sw_rucksack_draw(rs2, SW_COLS/2, 0);
		for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
			/* First rucksack. */
			if (i == sel && which == 0) {
				sw_setfg(SW_YELLOW);
				sw_putstr(1, i + 1, "%d", i);
			} else {
				sw_setfg(SW_WHITE);
				sw_putstr(1, i + 1, "%d", i);
			}

			/* Second rucksack. */
			if (i == sel && which == 1) {
				sw_setfg(SW_YELLOW);
				sw_putstr(SW_COLS/2 + 2, i + 1, "%d", i);
			} else {
				sw_setfg(SW_WHITE);
				sw_putstr(SW_COLS/2 + 2, i + 1, "%d", i);
			}
		}

		cmd = sw_getcmd();
	} while (1);
exit:
	return;
}

/******************************************************************************\
 * OBJ
\******************************************************************************/
static int blank_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	return -1; /* Always returns -1 by default (failure) */
}

struct sw_obj *sw_obj_alloc(void)
{
	struct sw_obj *o = malloc(sizeof(struct sw_obj));
	if (!o)
		return NULL;
	o->type = SW_OBJ_NONE;
	o->attr = SW_ATTR_NONE;
	o->fg = SW_BLACK;
	o->bg = SW_BLACK;
	o->display = ' ';
	o->x = -1;
	o->y = -1;
	o->life = 1;
	o->tile = NULL;
	o->handle_event = blank_handleevent;
	sw_rucksack_empty(&o->rucksack);
	return o;
}

void sw_obj_free(struct sw_obj *o)
{
	free(o);
}

int sw_obj_attack(struct sw_obj *def, struct sw_obj *att)
{
	/* Possible formula for damage:
	 * D = rand(0, a + d/2) - d/2
	 * where D = damage, a = attack, d = defense
	 */
	def->life -= 1;
	return 1;
}

static int items_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("Space->Direction to pick up items.");
		return -1; /* Return -1 saying, "you can't move here yet" */
	/* Interacting with an item attempts pickup. */
	case SW_OBJ_EV_INTERACT:
		sw_rucksack_compare(&from->rucksack, &self->rucksack);
		if (sw_rucksack_takenslots(&self->rucksack) == 0)
			sw_world_freeobj(world, self->x, self->y);
		return 0; /* Object interaction always succeeeds? */
	/* Attacking an item destroys it. */
	case SW_OBJ_EV_ATTACK:
		sw_obj_attack(self, from);
		sw_ui_addalert("You stomp on the items.");
		if (self->life <= 0) {
			sw_ui_addalert("They are smushed.");
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
		}
		return 0;
	default:
		return -1;
	}
	return -1;
}

static int abyss_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("You cannot move there!");
		return -1; /* Return -1 saying, "you can't move here yet" */
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("You need to use a tool on abyss.");
		return 0; /* Object interaction always succeeeds? */
	case SW_OBJ_EV_TOOL:
		x = self->x;
		y = self->y;
		sw_world_freeobj(world, x, y);
		sw_world_placeobj(world, sw_obj_gentype(SW_OBJ_ITEMS), x, y);
		return 0; /* Dig succeeded. */
	case SW_OBJ_EV_ATTACK:
		sw_ui_addalert("Abyss is tough stuff, but you're tougher.");
		sw_obj_attack(self, from);
		if (self->life <= 0) {
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
		}
	default:
		return -1;
	}

	return -1;
}

static int tree_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	struct sw_obj *tmpobj = NULL;
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("You cannot move there!");
		return -1; /* Return -1 saying, "you can't move here yet" */
	case SW_OBJ_EV_ATTACK:
		sw_obj_attack(self, from);
		sw_ui_addalert("You deal a feirce blow to the tree!");
		if (self->life <= 0) {
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
			tmpobj = sw_obj_gentype(SW_OBJ_ITEMS);
			sw_rucksack_additem(&tmpobj->rucksack,
				sw_item_make(SW_ITEM_WOOD));
			sw_world_placeobj(world, tmpobj, x, y);
		}
		else {
			sw_ui_addalert("It remains standing.");
		}
		return 0;
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Nice tree. Kinda mangly.");
		return 0; /* Object interaction always succeeeds? */
	default:
		return -1;
	}
	return -1;
}

static int boulder_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	struct sw_obj *tmpobj = NULL;
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("You cannot move there!");
		return -1; /* Return -1 saying, "you can't move here yet" */
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Mighty big boulder there.");
		return 0; /* Object interaction always succeeeds? */
	case SW_OBJ_EV_ATTACK:
		sw_ui_addalert("You injure yourself on the boulder,"
			"but it cracks.");
		sw_obj_attack(self, from);
		if (self->life <= 0) {
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
			tmpobj = sw_obj_gentype(SW_OBJ_ITEMS);
			sw_rucksack_additem(&tmpobj->rucksack,
				sw_item_make(SW_ITEM_DIRT));
			sw_world_placeobj(world, tmpobj, x, y);
		}
		return 0;
	default:
		return -1;
	}
	return -1;
}

struct sw_obj *sw_obj_gentype(enum sw_obj_type type)
{
	struct sw_obj *o = sw_obj_alloc();

	if (!o)
		return NULL;

	switch (type) {
	case SW_OBJ_ABYSS:
		o->type = SW_OBJ_ABYSS;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = ';';
		o->handle_event = abyss_handleevent;
		break;
	case SW_OBJ_PLAYER:
		o->type = SW_OBJ_PLAYER;
		o->attr = SW_ATTR_NONE;
		o->fg = SW_RED;
		o->display = '@';
		break;
	case SW_OBJ_ITEMS:
		o->type = SW_OBJ_ITEMS;
		o->fg = SW_YELLOW;
		o->display = ',';
		o->handle_event = items_handleevent;
		break;
	case SW_OBJ_TREE:
		o->type = SW_OBJ_TREE;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_GREEN;
		o->display = 'T';
		o->handle_event = tree_handleevent;
		break;
	case SW_OBJ_BOULDER:
		o->type = SW_OBJ_BOULDER;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = 'o';
		o->handle_event = boulder_handleevent;
		break;
	default:
		break;
	}

	return o;
}

/******************************************************************************\
 * TILES
\******************************************************************************/
struct sw_tile sw_tile_make(enum sw_tile_type type)
{
	struct sw_tile tile;
	tile.type = type;
	tile.object = NULL;

	tile.attr = SW_ATTR_NONE;
	tile.bg = SW_BLACK;

	switch (type) {
	case SW_TILE_BLANK:
		tile.fg = SW_BLACK;
		tile.display = ' ';
		break;
	case SW_TILE_GRASS:
		tile.fg = SW_GREEN;
		tile.display = '.';
		break;
	}

	return tile;
}


void sw_tile_walkon(struct sw_tile tile, struct sw_obj *obj)
{
	/* Annoying: */
	/* sw_info(0, "You are on grass"); */
}

void sw_tile_draw(struct sw_tile tile, int x, int y)
{
	sw_setfgbg(tile.fg, tile.bg, tile.attr);
	sw_putch(x, y, tile.display);
	if (tile.object) {
		sw_setfgbg(tile.object->fg, tile.object->bg, tile.object->attr);
		sw_putch(x, y, tile.object->display);
	}
}

/******************************************************************************\
 * WORLD
\******************************************************************************/
struct sw_world *sw_world_alloc(void)
{
	int x;
	int y;
	struct sw_world *world = malloc(sizeof(struct sw_world));

	if (!world)
		return NULL;

	for (x = 0; x < SW_WORLD_WIDTH; ++x)
		for (y = 0; y < SW_WORLD_HEIGHT; ++y)
			*SW_TILEP(world, x, y) = sw_tile_make(SW_TILE_BLANK);

	world->home_x = 0;
	world->home_y = 0;
	world->linkto = NULL;
	world->linkfrom = NULL;

	return world;
}

void sw_world_free(struct sw_world *world)
{
	free(world);
}

struct sw_tile *sw_world_gettilep(struct sw_world *world, int x, int y)
{
	return &world->tiles[x][y];
}

void sw_world_draw(struct sw_world *world, int x, int y)
{
	int xi;
	int yi;
	for (xi = 0; xi < SW_WORLD_WIDTH; ++xi) {
		for (yi = 0; yi < SW_WORLD_HEIGHT; ++yi) {
			if (world->linkto && xi == world->linkto->x &&
				xi == world->linkto->x) {
				sw_setfg(SW_RED);
				sw_putch(xi, yi, '>');
			}
			else if (world->linkfrom && xi == world->linkfrom->x &&
				xi == world->linkfrom->x) {
				sw_setfg(SW_RED);
				sw_putch(xi, yi, '<');
			}
			else {
				sw_tile_draw(world->tiles[xi][yi], xi+x, yi+y);
			}
		}
	}
}

int sw_world_placeobj(struct sw_world *world, struct sw_obj *o, int x, int y)
{
	if (SW_TILEP(world, x, y)->object)
		return -1;
	o->x = x;
	o->y = y;
	o->tile = SW_TILEP(world, x, y);
	SW_TILEP(world, x, y)->object = o;
	return 0;
}

int sw_world_placeobjhome(struct sw_world *world, struct sw_obj *o)
{
	if (world->tiles[world->home_x][world->home_y].object)
		return -1;
	o->x = world->home_x;
	o->y = world->home_y;
	o->tile = &world->tiles[o->x][o->y];
	world->tiles[o->x][o->y].object = o;
	return 0;
}

struct sw_obj *sw_world_removeobj(struct sw_world *world, int x, int y)
{
	struct sw_obj *o = SW_TILEP(world, x, y)->object;
	if (!o)
		return NULL;
	o->x = -1;
	o->y = -1;
	o->tile = NULL;
	SW_TILEP(world, x, y)->object = NULL;
	return o;
}

void sw_world_freeobj(struct sw_world *world, int x, int y)
{
	if (SW_TILEP(world, x, y)->object)
		sw_obj_free(SW_TILEP(world, x, y)->object);
	SW_TILEP(world, x, y)->object = NULL;
}

void sw_world_freeallobj(struct sw_world *world)
{
	int x;
	int y;

	for (x = 0; x < SW_WORLD_WIDTH; ++x) {
		for (y = 0; y < SW_WORLD_HEIGHT; ++y) {
			sw_world_freeobj(world, x, y);
		}
	}
}

int sw_world_inbounds(struct sw_world *world, int x, int y)
{
	if (x < 0 || x >= SW_WORLD_WIDTH || y < 0 || y >= SW_WORLD_HEIGHT)
		return 0;
	return 1;
}

void sw_world_updateobjs(struct sw_world *world)
{
	int x;
	int y;
	struct sw_obj *o = NULL;

	for (x = 0; x < SW_WORLD_WIDTH; ++x) {
		for (y = 0; y < SW_WORLD_HEIGHT; ++y) {
			o = SW_TILEP(world, x, y)->object;
			if (o) {
				o->handle_event(world, o, NULL,
					SW_OBJ_EV_UPDATE);
			}
			else {
				/* TODO: Update tile. */
			}
		}
	}
}

void sw_world_moveobjby(struct sw_world *world, int x, int y, int dx, int dy)
{
	sw_world_moveobjto(world, x, y, x + dx, y + dy);
}

void sw_world_moveobjto(struct sw_world *world, int x, int y, int nx, int ny)
{
	struct sw_obj *o = NULL;
	struct sw_obj *dest_o = NULL;

	if (!sw_world_inbounds(world, nx, ny))
		return;

	o = SW_TILEP(world, x, y)->object;
	dest_o = world->tiles[nx][ny].object;

	if (!o || o == dest_o) {
		return;
	} else if (dest_o && dest_o->handle_event) {
		/* Fail to move only if handling the event failed. */
		if (dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_MOVE) != 0)
			return; /* Fail */
	} else if (dest_o) {
		return;
	}

	sw_world_removeobj(world, x, y);
	sw_world_freeobj(world, nx, ny);
	sw_world_placeobj(world, o, nx, ny);
	sw_tile_walkon(*SW_TILEP(world, x, y), o);
}


void sw_world_interactobj(struct sw_world *world, int x, int y, int nx, int ny)
{
	struct sw_obj *o = NULL;
	struct sw_obj *dest_o = NULL;

	if (!sw_world_inbounds(world, nx, ny))
		return;

	o = SW_TILEP(world, x, y)->object;
	dest_o = world->tiles[nx][ny].object;

	if (!o || o == dest_o) {
		return;
	} else if (dest_o && dest_o->handle_event) {
		dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_INTERACT);
	} else if (dest_o) {
		return;
	}
}


void sw_world_attackobj(struct sw_world *world, int x, int y, int nx, int ny)
{
	struct sw_obj *o = NULL;
	struct sw_obj *dest_o = NULL;

	if (!sw_world_inbounds(world, nx, ny))
		return;

	o = SW_TILEP(world, x, y)->object;
	dest_o = world->tiles[nx][ny].object;

	if (!o || o == dest_o) {
		return;
	} else if (dest_o && dest_o->handle_event) {
		dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_ATTACK);
	} else if (dest_o) {
		return;
	}
}

struct sw_world *sw_world_genstart(void)
{
	struct sw_world *world = NULL;
	struct sw_obj *tmp = NULL;
	const int HALF_BOX_SIZE = 8;
	int x;
	int y;
	int cx = SW_WORLD_WIDTH / 2;
	int cy = SW_WORLD_HEIGHT / 2;

	world = sw_world_alloc();
	if (!world)
		return NULL;

	for (x = cx - HALF_BOX_SIZE; x < cx + HALF_BOX_SIZE; ++x) {
		for (y = cy - HALF_BOX_SIZE; y < cy + HALF_BOX_SIZE; ++y) {
			*SW_TILEP(world, x, y) = sw_tile_make(SW_TILE_GRASS);
		}
	}

	tmp = sw_obj_gentype(SW_OBJ_TREE);
	sw_world_placeobj(world, tmp, cx + 1, cy + 1);
	tmp = sw_obj_gentype(SW_OBJ_BOULDER);
	sw_world_placeobj(world, tmp, cx - 1, cy - 1);
	tmp = sw_obj_gentype(SW_OBJ_ITEMS);
	sw_rucksack_additem(&tmp->rucksack, sw_item_make(SW_ITEM_PULVERIZER));
	sw_world_placeobj(world, tmp, cx - 3, cy - 1);

	for (x = 0; x < SW_WORLD_WIDTH; ++x) {
		for (y = 0; y < SW_WORLD_HEIGHT; ++y) {
			if (SW_TILEP(world, x, y)->type == SW_TILE_BLANK) {
				tmp = sw_obj_gentype(SW_OBJ_ABYSS);
				*SW_TILEP(world, x, y) = sw_tile_make(SW_TILE_GRASS);
				sw_world_placeobj(world, tmp, x, y);
			}
		}
	}

	world->home_x = cx;
	world->home_y = cy;

	return world;
}
