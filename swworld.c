
#include "swlog.h"
#include "swobj.h"
#include "swui.h"
#include "swworld.h"

struct sw_world *sw_world_alloc(void)
{
	int x;
	int y;
	int i;
	struct sw_world *world = malloc(sizeof(struct sw_world));

	if (!world)
		return NULL;

	for (x = 0; x < SW_WORLD_WIDTH; ++x)
		for (y = 0; y < SW_WORLD_HEIGHT; ++y)
			*SW_TILEP(world, x, y) = sw_tile_make(SW_TILE_BLANK);

	world->player = sw_obj_gen(SW_OBJ_PLAYER);
	if (!world->player)
		return NULL;
	world->home_x = 0;
	world->home_y = 0;

	for (i = 0; i < SW_WORLD_LINKS; ++i)
		world->links[i] = NULL;

	return world;
}

void sw_world_free(struct sw_world *world)
{
	if (world)
		free(world->player);
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
			if (world->links[0] && xi ==world->links[0]->x &&
				xi == world->links[0]->x) {
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
	if (SW_OBJP(world, x, y))
		return -1;
	o->x = x;
	o->y = y;
	SW_OBJP(world, x, y) = o;
	return 0;
}

int sw_world_placeobjhome(struct sw_world *world, struct sw_obj *o)
{
	if (world->tiles[world->home_x][world->home_y].object)
		return -1;
	o->x = world->home_x;
	o->y = world->home_y;
	world->tiles[o->x][o->y].object = o;
	return 0;
}

struct sw_obj *sw_world_removeobj(struct sw_world *world, int x, int y)
{
	struct sw_obj *o = SW_OBJP(world, x, y);
	if (!o)
		return NULL;
	o->x = -1;
	o->y = -1;
	SW_OBJP(world, x, y) = NULL;
	return o;
}

void sw_world_freeobj(struct sw_world *world, int x, int y)
{
	if (SW_OBJP(world, x, y))
		sw_obj_free(SW_OBJP(world, x, y));
	SW_OBJP(world, x, y) = NULL;
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

	o = SW_OBJP(world, x, y);
	dest_o = SW_OBJP(world, nx, ny);

	if (!o || o == dest_o) {
		return;
	} else if (!dest_o) {
		sw_world_nullobj(world, nx, ny, o, SW_OBJ_EV_MOVE);
	} else if (dest_o && dest_o->handle_event) {
		/* Fail to move only if handling the event failed. */
		if (dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_MOVE) != 0)
			return; /* Fail */
	}

	sw_world_removeobj(world, x, y);
	sw_world_freeobj(world, nx, ny);
	sw_world_placeobj(world, o, nx, ny);
	sw_tile_walkon(*SW_TILEP(world, x, y), o);

	sw_logmsg("%s moved to %d,%d", o->name, nx, ny);
}


void sw_world_interactobj(struct sw_world *world, int x, int y, int nx, int ny)
{
	struct sw_obj *o = NULL;
	struct sw_obj *dest_o = NULL;

	if (!sw_world_inbounds(world, nx, ny))
		return;

	o = SW_OBJP(world, x, y);
	dest_o = SW_OBJP(world, nx, ny);

	if (!o || o == dest_o) {
		return;
	} else if (!dest_o) {
		sw_world_nullobj(world, nx, ny, o, SW_OBJ_EV_INTERACT);
	} else if (dest_o && dest_o->handle_event) {
		sw_logmsg("%s interacted with to %d,%d (%s)", o->name, nx, ny,
			dest_o->name);
		dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_INTERACT);
	}
}


void sw_world_attackobj(struct sw_world *world, int x, int y, int nx, int ny)
{
	struct sw_obj *o = NULL;
	struct sw_obj *dest_o = NULL;

	if (!sw_world_inbounds(world, nx, ny))
		return;

	o = SW_OBJP(world, x, y);
	dest_o = SW_OBJP(world, nx, ny);

	if (!o || o == dest_o) {
		return;
	} else if (!dest_o) {
		sw_world_nullobj(world, nx, ny, o, SW_OBJ_EV_ATTACK);
	} else if (dest_o && dest_o->handle_event) {
		sw_logmsg("%s attacked %d,%d (%s)", o->name, nx, ny,
			dest_o->name);
		dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_ATTACK);
	}
}

void sw_world_toolobj(struct sw_world *world, int x, int y, int nx, int ny)
{
	struct sw_obj *o = NULL;
	struct sw_obj *dest_o = NULL;

	if (!sw_world_inbounds(world, nx, ny))
		return;

	o = SW_OBJP(world, x, y);
	dest_o = SW_OBJP(world, nx, ny);

	if (!o || o == dest_o) {
		return;
	} else if (!dest_o) {
		sw_world_nullobj(world, nx, ny, o, SW_OBJ_EV_TOOL);
	} else if (dest_o && dest_o->handle_event) {
		sw_logmsg("%s used a tool on %d,%d (%s)", o->name, nx, ny,
			dest_o->name);
		dest_o->handle_event(world, dest_o, o, SW_OBJ_EV_TOOL);
	}
}

void sw_world_nullobj(struct sw_world *world, int x, int y, struct sw_obj *o,
	enum sw_obj_ev ev)
{
	switch (ev) {
	case SW_OBJ_EV_ATTACK:
		sw_ui_addalert("Nothing to attack!");
		break;
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Nothing to interact with!");
		break;
	case SW_OBJ_EV_TOOL:

		break;
	default:
		break;
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

	tmp = sw_obj_gen(SW_OBJ_TREE);
	sw_world_placeobj(world, tmp, cx + 1, cy + 1);
	tmp = sw_obj_gen(SW_OBJ_BOULDER);
	sw_world_placeobj(world, tmp, cx - 1, cy - 1);
	tmp = sw_obj_gen(SW_OBJ_BUSH);
	sw_world_placeobj(world, tmp, cx + 3, cy - 1);

	tmp = sw_obj_gen(SW_OBJ_ITEMS);
	sw_rucksack_additem(&tmp->rucksack, sw_item_gen(SW_ITEM_PULVERIZER));
	sw_world_placeobj(world, tmp, cx - 3, cy - 1);

	for (x = 0; x < SW_WORLD_WIDTH; ++x) {
		for (y = 0; y < SW_WORLD_HEIGHT; ++y) {
			if (SW_TILEP(world, x, y)->type == SW_TILE_BLANK) {
				tmp = sw_obj_gen(SW_OBJ_DIRT);
				*SW_TILEP(world, x, y) =
					sw_tile_make(SW_TILE_GRASS);
				sw_world_placeobj(world, tmp, x, y);
			}
		}
	}

	world->home_x = cx;
	world->home_y = cy;

	return world;
}

