
#include "swobj.h"
#include "swworld.h"

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
