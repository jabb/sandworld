
#include "swobj.h"
#include "swtile.h"

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
	if (tile.object) {
		sw_obj_draw(tile.object, x, y);
	}
	else {
		sw_setfgbg(tile.fg, tile.bg, tile.attr);
		sw_putch(x, y, tile.display);
	}
}
