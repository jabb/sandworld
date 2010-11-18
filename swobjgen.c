
#include "swobj.h"
#include "swui.h"
#include "swworld.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define EVENT_HANDLER(name) \
static int name(struct sw_world *world, struct sw_obj *self, \
	struct sw_obj *from, enum sw_obj_ev ev)

EVENT_HANDLER(items_handleevent)
{
	int rv = 0;

	switch (ev) {
	/* Moving onto an object attempts to pick them all up. */
	case SW_OBJ_EV_MOVE:
		if (sw_rucksack_addrucksack(&from->rucksack, &self->rucksack)
			== 0) {
			sw_ui_addalert("You pick up some items.");
			sw_world_freeobj(world, self->x, self->y);
		} else {
			sw_ui_addalert("Your rucksack is full.");
		}
		rv = -1; /* No moving! */
		break;
	/* Interacting with an item attempts pickup. */
	case SW_OBJ_EV_INTERACT:
		sw_rsui_compare(world, &from->rucksack, &self->rucksack);
		if (sw_rucksack_takenslots(&self->rucksack) == 0)
			sw_world_freeobj(world, self->x, self->y);
		break;
	/* Attacking an item destroys it. */
	case SW_OBJ_EV_ATTACK:
		sw_ui_addalert("You stomp on the items and smush them.");
		sw_world_freeobj(world, self->x, self->y);
		break;
	default:
		rv = -1;
		break;
	}
	return rv;
}

struct a_drop {
	int chance;
	int itemid;
	int min;
	int max;
};

static void drop(struct sw_world *world, int x, int y, struct a_drop drops[],
	unsigned long size)
{
	int i;
	struct sw_item item;
	struct sw_obj *o = sw_obj_gen(SW_OBJ_ITEMS);

	for (i = 0; i < size; ++i) {
		if (sw_onein(drops[i].chance)) {
			item = sw_item_genamount(drops[i].itemid,
				sw_randint(drops[i].min, drops[i].max));
			sw_rucksack_additem(&o->rucksack, item);
		}
	}

	if (sw_rucksack_takenslots(&o->rucksack) > 0)
		sw_world_placeobj(world, o, x, y);
}

EVENT_HANDLER(dirt_handleevent)
{
	int x;
	int y;
	int rv = 0;

	struct a_drop drops[] = {
		/*Chance	Item			Amount */
		{30,		SW_ITEM_DIRT,		1,	2},
		{10,		SW_ITEM_TREESEED, 	1,	1}
	};

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		rv = -1;
		/* Fallthrough. */
	case SW_OBJ_EV_TOOL:
		if (sw_rucksack_wieldingcan(&from->rucksack, SW_ITEM_USE_DIG)) {
			/* TODO: attack the object */
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, x, y);
			drop(world, x, y, drops, ARRAY_SIZE(drops));
			sw_ui_addalert("You dig through this stuff.");
		} else {
			sw_ui_addalert("You are not wielding an item "
				"which can dig.");
		}
		break;
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Looks interesting.");
		return 0; /* Object interaction always succeeeds? */
	case SW_OBJ_EV_ATTACK:
		sw_ui_addalert("You bash the %s to no effect", self->name);
		rv = -1;
		break;
	default:
		rv = -1;
		break;
	}

	return rv;
}

EVENT_HANDLER(plant_handleevent)
{
	struct sw_obj *tmpobj = NULL;
	int dmg;
	int x;
	int y;
	int rv = 0;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("Nice %s.", self->name);
		return -1;
	case SW_OBJ_EV_ATTACK:
		dmg = sw_obj_attack(self, from);
		if (sw_obj_isdestroyed(self)) {
			sw_ui_addalert("You deal a feirce blow to the %s! "
				"(%d dmg)", self->name, dmg);
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
			tmpobj = sw_obj_gen(SW_OBJ_ITEMS);
			sw_rucksack_additem(&tmpobj->rucksack,
				sw_item_gen(SW_ITEM_WOOD));
			sw_world_placeobj(world, tmpobj, x, y);
		} else {
			sw_ui_addalert("Stupid plant... "
				"(%d dmg)", dmg);
		}
		break;
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Nice %s. Kinda mangly.", self->name);
		break; /* Object interaction always succeeeds? */
	default:
		rv = -1;
		break;
	}

	return rv;
}

EVENT_HANDLER(boulder_handleevent)
{
	struct sw_obj *tmpobj = NULL;
	int dmg;
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("Nice %s.", self->name);
		return -1;
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Mighty big boulder there.");
		return 0; /* Object interaction always succeeeds? */
	case SW_OBJ_EV_ATTACK:
		dmg = sw_obj_attack(self, from);
		if (sw_obj_isdestroyed(self)) {
			sw_ui_addalert("You injure yourself on the boulder, "
				"but it cracks. (%d dmg)", dmg);
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
			tmpobj = sw_obj_gen(SW_OBJ_ITEMS);
			sw_rucksack_additem(&tmpobj->rucksack,
				sw_item_gen(SW_ITEM_DIRT));
			sw_world_placeobj(world, tmpobj, x, y);
		} else {
			sw_ui_addalert("You smack the boulder helplessly. "
				"(%d dmg)", dmg);
		}
		return 0;
	default:
		return -1;
	}
	return -1;
}

struct sw_obj *sw_obj_gen(enum sw_obj_type type)
{
	struct sw_obj *o = sw_obj_alloc();

	if (!o)
		return NULL;

	switch (type) {
	case SW_OBJ_PLAYER:
		o->type = SW_OBJ_PLAYER;
		o->attr = SW_ATTR_NONE;
		o->fg = SW_RED;
		o->display = '@';
		o->name = "Player";
		break;
	case SW_OBJ_DIRT:
		o->type = SW_OBJ_DIRT;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = ';';
		o->handle_event = dirt_handleevent;
		o->name = "Dirt";
		break;
	case SW_OBJ_CLAY:
		o->type = SW_OBJ_CLAY;
		o->fg = SW_YELLOW;
		o->display = ';';
		o->handle_event = dirt_handleevent;
		o->name = "Clay";
		break;
	case SW_OBJ_STONE:
		o->type = SW_OBJ_STONE;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = '#';
		o->handle_event = dirt_handleevent;
		o->name = "Stone";
		break;
	case SW_OBJ_ITEMS:
		o->type = SW_OBJ_ITEMS;
		o->fg = SW_YELLOW;
		o->display = ',';
		o->handle_event = items_handleevent;
		o->name = "Items";
		break;
	case SW_OBJ_TREE:
		o->type = SW_OBJ_TREE;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_GREEN;
		o->display = 'T';
		o->handle_event = plant_handleevent;
		o->name = "Tree";
		break;
	case SW_OBJ_BOULDER:
		o->type = SW_OBJ_BOULDER;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = 'o';
		o->cur_life = 2;
		o->max_life = 2;
		o->cur_resist = 3;
		o->max_resist = 3;
		o->handle_event = boulder_handleevent;
		o->name = "Boulder";
		break;
	case SW_OBJ_BUSH:
		o->type = SW_OBJ_BUSH;
		o->fg = SW_GREEN;
		o->display = '*';
		o->handle_event = plant_handleevent;
		o->name = "Bush";
		break;
	default:
		break;
	}

	return o;
}
